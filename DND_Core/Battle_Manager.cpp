#include <memory>
#include <vector>
#include <cmath>
#include <algorithm>

#include "Battle_Manager.h"
#include "Enums.h"
#include "Data_Structures.h"
#include "utils.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//BATTLE MANAGER GETTERS AND SETTERS

const std::unordered_map<int, std::unique_ptr<Battle_Entity>>& Battle_Manager::get_battle_entities() const { return battle_entities; }
const std::vector<std::pair<int, int>>& Battle_Manager::get_turn_ids() const { return turn_ids; }
const std::weak_ptr<Quest> Battle_Manager::get_active_quest() const { return active_quest; }
const Battle_Type Battle_Manager::get_battle_type() const { return battle_type; }
const int Battle_Manager::get_entity_count() const { return entity_count; }
const int Battle_Manager::get_x_max() const { return x_max; }
const int Battle_Manager::get_y_max() const { return y_max; }

void Battle_Manager::set_battle_entities(std::unordered_map<int, std::unique_ptr<Battle_Entity>>& new_battle_entities)
{
	if (new_battle_entities.empty())
		throw std::invalid_argument("battle entities cannot be empty");

	battle_entities.clear();

	for (const auto& v : new_battle_entities)
	{
		battle_entities[v.first] = std::make_unique<Battle_Entity>(*v.second);
	}
}
void Battle_Manager::set_turn_ids(std::vector<std::pair<int, int>>& new_turn_ids)
{
	if (new_turn_ids.empty())
		throw std::invalid_argument("turn ids cannot be empty");

	turn_ids = new_turn_ids;
}
void Battle_Manager::set_active_quest(std::weak_ptr<Quest> new_active_quest)
{
	if (battle_type == Battle_Type::Random_Encounter)
		throw std::invalid_argument("random encounter : cannot take in a quest");

	if (new_active_quest.expired())
		throw std::runtime_error("quest no longer exists");

	active_quest = new_active_quest;
}
void Battle_Manager::set_battle_type(Battle_Type new_battle_type) { battle_type = new_battle_type; }
void Battle_Manager::set_entity_count(int new_entity_count)
{
	if (new_entity_count <= 0)
		throw std::invalid_argument("invalid entity count entered, must be positive");

	entity_count = new_entity_count;
}
void Battle_Manager::set_x_max(int new_x_max)
{
	if (new_x_max <= 0)
		throw std::invalid_argument("invalid x_max entered (must be positive)");

	x_max = new_x_max;
}
void Battle_Manager::set_y_max(int new_y_max)
{
	if (new_y_max <= 0)
		throw std::invalid_argument("invalid y_max entered (must be positive)");

	y_max = new_y_max;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Battle_Manager::calculate_x_y_borders()
{
	int battlefield_size = std::max(5, static_cast<int>(std::sqrt(entity_count * 1.5)) * 3);
	set_x_max(battlefield_size);
	set_y_max(battlefield_size);

	for (const auto& v : battle_entities)
	{
		v.second->set_x_max(x_max);
		v.second->set_y_max(y_max);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Battle_Manager::roll_for_initiative()
{
	turn_ids.clear();

	for (const auto& v : battle_entities)
	{
		int initiative_roll = dice_roll(20);
		int dex_modifier = v.second->get_entity()->get_stat_modifier(v.second->get_entity()->get_stats().dex_);
		int initiative = initiative_roll + dex_modifier;

		turn_ids.push_back({ initiative, v.first });
	}

	std::sort(turn_ids.begin(), turn_ids.end(), [](const auto& a, const auto& b) {return a.first > b.first; });
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool Battle_Manager::is_position_occupied(int x, int y)
{
	for (const auto& v : battle_entities)
	{
		if (v.second->get_x_axis() != x)
			continue;

		if (v.second->get_y_axis() != y)
			continue;

		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Battle_Manager::move_entity_position(int x, int y, int id)
{
	if (battle_entities.at(id)->get_x_axis() == x && battle_entities.at(id)->get_y_axis() == y)
	{
		print("you moved to the same place you are currently standing, brilliant work\n", 5);
		return;
	}

	if (is_position_occupied(x, y))
	{
		print("someone is already standing there, find another spot to do whatever you are doing\n", 5);
		return;
	}

	if (!in_range(x, 0, x_max - 1) || !in_range(y, 0, y_max - 1))
	{
		print("careful! you are going to desert the battle, are you sure ?\n", 5);

		char choice;
		do
		{
			choice = input<char>("(Y/N) : ", 5);

			if (to_upper(choice) == 'N')
			{
				print("did not desert battle\n", 5);
				return;
			}
			else if (to_upper(choice) == 'Y')
			{
				print("mmhm a desertion it is then, very well\n", 5);

				battle_entities.at(id)->set_x_axis(x);
				battle_entities.at(id)->set_y_axis(y);
				battle_entities.at(id)->set_status(Battle_Entity_Status::Deserted);

				print("moved character to position (" + std::to_string(x) + "," + std::to_string(y) + ") which causes a desertion in battle\n", 5);
				return;
			}
			else
			{
				print("invalid choice entered\n", 5);
			}

		} while (to_upper(choice) != 'N' && to_upper(choice) != 'Y');
	}

	battle_entities.at(id)->set_x_axis(x);
	battle_entities.at(id)->set_y_axis(y);

	print("moved character to position (" + std::to_string(x) + "," + std::to_string(y) + ")\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Battle_Manager::entity_random_placement()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////

Battle_Manager::Battle_Manager(Battle_Type new_battle_type, const std::vector<std::pair<int, std::weak_ptr<Entity>>>& players,
	std::weak_ptr<Quest> new_active_quest, std::vector<std::pair<int, Enemies>> new_enemies) : battle_type(new_battle_type)
{
	for (int i = 0; i < players.size(); i++, entity_count++)
	{
		battle_entities[entity_count] = std::make_unique<Battle_Entity>(players.at(i).first, players.at(i).second);
	}

	switch (new_battle_type)
	{
	case Battle_Type::Quest:
	{
		set_active_quest(new_active_quest);

		if (auto p = get_active_quest().lock())
		{
			for (const auto& v : p->get_enemies())
			{
				if (!v.second.enemy_ptr.expired())
				{
					for (int i = 0; i < v.second.quantity; i++, entity_count++)
					{
						battle_entities[entity_count] = std::make_unique<Battle_Entity>(v.first, v.second.enemy_ptr);
					}
				}
			}
		}
	}
		break;

	case Battle_Type::Random_Encounter:
	{
		for (const auto& v : new_enemies)
		{
			if (!v.second.enemy_ptr.expired())
			{
				for (int i = 0; i < v.second.quantity; i++, entity_count++)
				{
					battle_entities[entity_count] = std::make_unique<Battle_Entity>(v.first, v.second.enemy_ptr);
				}
			}
		}
	}
		break;
	}

	calculate_x_y_borders();
	roll_for_initiative();
}