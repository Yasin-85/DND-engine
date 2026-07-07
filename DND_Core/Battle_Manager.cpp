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

bool Battle_Manager::is_battle_over()
{
	bool any_player_alive{ false }, any_enemy_alive{ false };

	for (const auto& v : battle_entities)
	{
		if (v.second->get_status() != Battle_Entity_Status::Alive)
			continue;

		if (v.second->get_is_party_member())
			any_player_alive = true;

		else
			any_enemy_alive = true;
	}

	if (any_player_alive || any_enemy_alive)
		return false;

	else
		return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool Battle_Manager::is_position_occupied(int x, int y, int &id)
{
	for (const auto& v : battle_entities)
	{
		if (v.second->get_x_axis() != x)
			continue;

		if (v.second->get_y_axis() != y)
			continue;

		if (v.second->get_status() == Battle_Entity_Status::Dead)
			continue;

		id = v.first;
		return true;
	}

	return false;
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

		if (v.second->get_status() == Battle_Entity_Status::Dead)
			continue;

		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Battle_Manager::print_battle_field()
{
	print_line(5);

	print("every \033[32m@\033[0m represents a party member and every \033[31m#\033[0m represents an enemy\n", 5);

	// Find max digit length for spacing
	int max_digits = std::to_string(y_max - 1).length();

	for (int y = y_max - 1; y >= 0; y--)
	{
		// print row number with proper padding
		std::string y_str = std::to_string(y);
		print(std::string(max_digits - y_str.length(), ' ') + y_str + " ");


		for  (int x = 0; x < x_max; x++)
		{
			int id{ 0 };
			if (is_position_occupied(x, y, id))
			{
				if (battle_entities.at(id)->get_is_party_member())
					print("\033[32m@\033[0m "); // green @ for party members

				else
					print("\033[31m#\033[0m "); // red # for enemies
			}
			else
				print("* ");
		}
		print("\n");
	}

	// print column numbers at the bottom
	print(std::string(max_digits + 1, ' ')); // padding for row numbers
	for (int x = 0; x < x_max; x++)
	{
		print(std::to_string(x) + " ");
	}
	print("\n");

	print("\nparty members : \n", 5);

	// printing party members locations on the battlefield 
	for (const auto& v : battle_entities)
	{
		if (v.second->get_is_party_member())
		{
			print(std::to_string(v.first) + " " + v.second->get_entity()->get_name() +
				" (" + std::to_string(v.second->get_x_axis()) + "," + std::to_string(v.second->get_y_axis()) + "), ");
		}
	}

	print("\nenemies : \n", 5);

	// printing enemy's locations on the battlefield 
	for (const auto& v : battle_entities)
	{
		if (!v.second->get_is_party_member())
		{
			print(std::to_string(v.first) + " " + v.second->get_entity()->get_name() +
				" (" + std::to_string(v.second->get_x_axis()) + "," + std::to_string(v.second->get_y_axis()) + "), ");
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Battle_Manager::move_entity_position(int x, int y, int id)
{
	bool is_player = battle_entities.at(id)->get_is_party_member();

	if (battle_entities.at(id)->get_x_axis() == x && battle_entities.at(id)->get_y_axis() == y)
	{
		if (is_player)
			print("you moved to the same place you are currently standing, brilliant work\n", 5);

		return;
	}

	if (is_position_occupied(x, y))
	{
		if (is_player)
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

	print("character " + battle_entities.at(id)->get_entity()->get_name() + " moved to position (" + std::to_string(x) + "," + std::to_string(y) + ")\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Battle_Manager::entity_random_placement()
{
	for (const auto& v : battle_entities)
	{
		try
		{
			int x, y, fall_back{ 0 }; 
			double segmant_height{ std::round(y_max / 3) };

			if (v.second->get_is_party_member())
			{
				fall_back = 0;

				do
				{
					fall_back++;
					x = dice_roll(x_max) - 1;
					y = dice_roll(segmant_height) - 1;

					if (fall_back >= 100)
					{
						for (int j = 0; j < segmant_height; j++)
						{
							for (int i = 0; i < x_max; i++)
							{
								if (!is_position_occupied(i, j))
								{
									v.second->set_x_axis(i);
									v.second->set_y_axis(j);
									throw std::invalid_argument("");
								}
							}
						}
						throw std::runtime_error("too many player entities");
					}
				} while (is_position_occupied(x, y));

				v.second->set_x_axis(x);
				v.second->set_y_axis(y);
			}
			else
			{
				fall_back = 0;

				do
				{
					fall_back++;
					x = dice_roll(x_max) - 1;
					y = dice_roll(segmant_height) - 1;

					if (fall_back >= 100)
					{
						for (int j = 0; j < segmant_height; j++)
						{
							for (int i = 0; i < x_max; i++)
							{
								if (!is_position_occupied(i, y_max - j))
								{
									v.second->set_x_axis(i);
									v.second->set_y_axis(y_max - j);
									throw std::invalid_argument("");
								}
							}
						}
						throw std::runtime_error("too many enemy entities");
					}

				} while (is_position_occupied(x, y_max - y));

				v.second->set_x_axis(x);
				v.second->set_y_axis(y_max - y);
			}
		}
		catch (const std::invalid_argument) {}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Battle_Manager::player_turn(int id)
{
	while (true)
	{
		try
		{
			print_line(5);

			print("choose your action\n"
				"1. move 1 tile (2 if dex > 14) <counts as an action>\n"
				"2. cast a spell/attack an in range enemy <counts as an action>\n"
				"3. use a consumable\n <counts as an action>"
				"4. print the battle field\n"
				"5. manage equipped weapons and armor\n"
				"6. view party member's stats\n"
				"7. view enemy's stats"
				"8. skip turn\n", 5);

			char choice = input<int>("your choice : ", 5);

			switch (choice)
			{
			case 1: // move
			{
				print_line(5);

				while (true)
				{
					int tiles_to_move = input<int>("tiles to move (1-2, -1: view battlefield, -2: cancel): ", 5);

					// cancel
					if (tiles_to_move == -2)
						break;

					// view battlefield (free action, stay in menu)
					if (tiles_to_move == -1)
					{
						print_battle_field();
						continue;
					}

					// validate tile count
					if (tiles_to_move < 1 || tiles_to_move > 2)
					{
						print("invalid input\n", 5);
						continue;
					}

					// check dex for 2-tile move
					if (tiles_to_move == 2 && battle_entities.at(id)->get_entity()->get_stats().dex_ <= 14)
					{
						print("not enough dexterity for 2 tiles (need 15+)\n", 5);
						continue;
					}

					// show current position
					print("current position: (" + std::to_string(battle_entities.at(id)->get_x_axis()) + "," +
						std::to_string(battle_entities.at(id)->get_y_axis()) + ")\n", 5);

					// get movement direction
					while (true)
					{
						char axis = input<char>("axis (x/y, b to cancel): ", 5);
						axis = to_upper(axis);

						if (axis == 'B')
						{
							print("movement cancelled\n", 5);
							break;
						}

						if (axis != 'X' && axis != 'Y')
						{
							print("invalid axis\n", 5);
							continue;
						}

						char direction = input<char>("direction (+/-): ", 5);
						if (direction != '+' && direction != '-')
						{
							print("invalid direction\n", 5);
							continue;
						}

						// calculate target position
						int dx = 0, dy = 0;
						int steps = tiles_to_move;
						int current_x = battle_entities.at(id)->get_x_axis();
						int current_y = battle_entities.at(id)->get_y_axis();

						if (axis == 'X')
							dx = (direction == '+') ? steps : -steps;
						else
							dy = (direction == '+') ? steps : -steps;

						int target_x = current_x + dx;
						int target_y = current_y + dy;

						// check path for 2-tile movement (check the intermediate tile)
						bool path_clear = true;
						if (steps == 2)
						{
							int mid_x = current_x + (dx / 2);
							int mid_y = current_y + (dy / 2);
							if (is_position_occupied(mid_x, mid_y))
							{
								print("path blocked at intermediate tile\n", 5);
								continue;
							}
						}

						// check if destination is occupied
						if (is_position_occupied(target_x, target_y))
						{
							print("destination occupied\n", 5);
							continue;
						}

						// move the entity
						move_entity_position(target_x, target_y, id);
						return;
					}
				}
				break;
			}
				
			case 2:
				break;

			case 3:
				break;

			case 4:
				print_battle_field();
				break;

			case 5:
				print_line(5);

				battle_entities.at(id)->get_entity()->equip_item();
				break;

			case 6:
				print_line(5);

				for (const auto& v : battle_entities)
				{
					if (!v.second->get_is_party_member())
						continue;

					print(std::to_string(v.first) + " ");
					v.second->display_details();
				}
				break;

			case 7:
				print_line(5);

				for (const auto& v : battle_entities)
				{
					if (v.second->get_is_party_member())
						continue;

					print(std::to_string(v.first) + " ");
					v.second->display_details();
				}
				break;

			case 8:
				print("turn skipped\n", 5);
				return;

			default:
				print("invalid choice entered\n", 5);
				break;
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Battle_Manager::enemy_turn(int id)
{
	int nearest_player_id{ -1 };

	int x, y;
	x = battle_entities.at(id)->get_x_axis();
	y = battle_entities.at(id)->get_y_axis();

	int player_x{ 0 }, player_y{ 0 };

	float nearest_player_position{ FLT_MAX };

	for (const auto& v : battle_entities)
	{
		if (!v.second->get_is_party_member())
			continue;

		player_x = v.second->get_x_axis();
		player_y = v.second->get_y_axis();

		float distnace = std::sqrt(std::pow(x - player_x, 2) + std::pow(y - player_y, 2));

		if (distnace <= nearest_player_position)
		{
			nearest_player_id = v.first;
			nearest_player_position = distnace;
		}
	}

	if (nearest_player_id == -1)
		return;

	if (auto p = battle_entities.at(id)->get_entity()->get_equipped_weapon().second.lock()) // with weapon
	{
		if (nearest_player_position <= p->get_range())
			goto attack;

		else
			goto move;

	}
	else // unarmed
	{
		if (nearest_player_position <= 1.5)
			goto attack;

		else
			goto move;
	}

attack:
	{
		auto p = battle_entities.at(id)->get_entity(); //enemy aka the one who's turn it is
		auto o = battle_entities.at(nearest_player_id)->get_entity(); //chosen player

		int player_ac = o->get_armorclass();

		int str = p->get_stats().str_;
		int str_modifier = p->get_stat_modifier(str);
		int proficiency_bonus = p->get_proficiency_bonus(p->get_level());

		if (is_it_a_hit(player_ac, str_modifier, proficiency_bonus))
		{
			int dmg = p->attack();
			print(p->get_name() + " hit " + o->get_name() + " for " + std::to_string(dmg) + " damage\n", 5);
			
			o->set_current_hp(o->get_current_hp() - dmg);
			o->display_info();
			return;
		}
		else
		{
			print(p->get_name() + " missed a hit on " + o->get_name() + '\n', 5);
			return;
		}
	}

move:
	{
		int dx = x - player_x;
		int dy = y - player_y;

		int roll;

		if (dx != 0 && dy != 0)
		{
			roll = dice_roll(2) - 1;

			if (roll == 0)
			{
				if (!is_position_occupied(x, y - (dy / std::abs(dy))))
					move_entity_position(x, y - (dy / std::abs(dy)), id);

				else if (!is_position_occupied(x - (dx / std::abs(dx)), y))
					move_entity_position(x - (dx / std::abs(dx)), y, id);

				else
					print("path blocked\n", 5);
					
				return;
			}
			else
			{
				if (!is_position_occupied(x - (dx / std::abs(dx)), y))
					move_entity_position(x - (dx / std::abs(dx)), y, id);

				else if (!is_position_occupied(x, y - (dy / std::abs(dy))))
					move_entity_position(x, y - (dy / std::abs(dy)), id);

				else
					print("path blocked\n", 5);

				return;
			}
		}
		else if (dx == 0)
		{
			if (!is_position_occupied(x, y - (dy / std::abs(dy))))
				move_entity_position(x, y - (dy / std::abs(dy)), id);

			else
				print("path blocked\n", 5);

			return;
		}
		else
		{
			if (!is_position_occupied(x - (dx / std::abs(dx)), y))
				move_entity_position(x - (dx / std::abs(dx)), y, id);

			else
				print("path blocked\n", 5);

			return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Battle_Manager::main_battle()
{
	print_battle_field();

	while (!is_battle_over())
	{
		for (const auto& v : turn_ids) //.first is initiative roll and .second is the entity's id
		{
			int id = v.second;

			if (is_battle_over())
				break;

			switch (battle_entities.at(id)->get_status())
			{
			case Battle_Entity_Status::Dead:
				continue;
				break;

			case Battle_Entity_Status::Deserted:
				continue;
				break;

			case Battle_Entity_Status::Alive:

				print("its " + battle_entities.at(id)->get_entity()->get_name() + "'s turn\n", 5);

				if (battle_entities.at(id)->get_is_party_member())
					player_turn(id);

				else
					enemy_turn(id);

				break;
			}
		}
	}

	battle_end();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Battle_Manager::battle_end()
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
				if (v.second.enemy_ptr.expired())
					continue;

				for (int i = 0; i < v.second.quantity; i++, entity_count++)
				{
					battle_entities[entity_count] = std::make_unique<Battle_Entity>(v.first, v.second.enemy_ptr);
				}
			}
		}
	}
	break;

	case Battle_Type::Random_Encounter:
	{
		for (const auto& v : new_enemies)
		{
			if (v.second.enemy_ptr.expired())
				continue;

			for (int i = 0; i < v.second.quantity; i++, entity_count++)
			{
				battle_entities[entity_count] = std::make_unique<Battle_Entity>(v.first, v.second.enemy_ptr);
			}
		}
	}
	break;
	}

	calculate_x_y_borders();
	roll_for_initiative();
	entity_random_placement();
}