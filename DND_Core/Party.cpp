#include <array>
#include <vector>
#include <memory>
#include <string>

#include "Party.h"
#include "Data_Structures.h"
#include "Enums.h"
#include "Utils.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

const std::array<std::pair<int, std::weak_ptr<Entity>>, 4>& Party::get_party() const { return party; }

/////////////////////////////////////////////////////////////////////////////////////////////////////

const std::array<std::pair<int, std::vector<std::shared_ptr<Rewards>>>, 4>& Party::get_reward_buffer() const { return reward_buffer; }

/////////////////////////////////////////////////////////////////////////////////////////////////////

const Party_State Party::get_party_state() const { return party_state; }

/////////////////////////////////////////////////////////////////////////////////////////////////////

const bool Party::get_single_player() const { return single_player; }

/////////////////////////////////////////////////////////////////////////////////////////////////////

const int Party::get_avg_level() const { return avg_level; }

/////////////////////////////////////////////////////////////////////////////////////////////////////

const int Party::get_party_size() const { return party_size; }

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_party(std::array<std::pair<int, std::weak_ptr<Entity>>, 4>& new_party) { party = new_party; }

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_reward_buffer(std::array<std::pair<int, std::vector<std::shared_ptr<Rewards>>>, 4>& new_reward_buffer) { reward_buffer = new_reward_buffer; }

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_party_state(Party_State new_party_state) { party_state = new_party_state; }

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_single_player(bool new_single_player) { single_player = new_single_player; }

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_avg_level(int new_avg_level)
{
	if (avg_level <= 0)
		throw std::invalid_argument("invalid avg_level entered must be positive");
	else
		avg_level = new_avg_level;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_party_size(int new_party_size)
{
	if (in_range(new_party_size, 1, 4))
	{
		party_size = new_party_size;
	}
	else
		throw std::invalid_argument("invalid party_size entered must be in (1 - 4)");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::assign_party_members(Entity_Manager_Lambda& entity_manager_lambda)
{
	if (entity_manager_lambda.get_entity_manager().empty())
		throw std::runtime_error("no player to choose");
	std::array<int, 4> ids;

	party.fill({ 0, {} });
	reward_buffer.fill({ 0, {} });

	while (true)
	{
		try
		{
			ids.fill(0);

			print_line(5);

			if (get_single_player())
			{
				print("please enter the id of the chosen character\n", 5);

				ids[0] = ask_id(entity_manager_lambda.get_entity_manager(), false, entity_manager_lambda.display_player, "player id : ", 5);
			}
			else
			{
				print("please enter the id of the 4 chosen characters", 5);

				ids[0] = ask_id(entity_manager_lambda.get_entity_manager(), false, entity_manager_lambda.display_player, "player 1 id : ", 5);
				ids[1] = ask_id(entity_manager_lambda.get_entity_manager(), false, entity_manager_lambda.display_player, "player 2 id : ", 5);
				ids[2] = ask_id(entity_manager_lambda.get_entity_manager(), false, entity_manager_lambda.display_player, "player 3 id : ", 5);
				ids[3] = ask_id(entity_manager_lambda.get_entity_manager(), false, entity_manager_lambda.display_player, "player 4 id : ", 5);

				check_for_duplicates(ids);
			}

			for (int i = 0; i < get_party_size(); i++)
			{
				if (auto p = entity_manager_lambda.get_entity(ids[i]).lock())
				{
					if (!p->get_is_player())
						throw std::invalid_argument("chosen character is not a player");
				}

				reward_buffer[i].first = ids[i];
				party[i].first = ids[i];
				party[i].second = entity_manager_lambda.get_entity(ids[i]);
			}

			break;
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::assign_member_rewards(Data_Base_Lambda& data_base_lambda, Entity_Manager_Lambda& entity_manager_lambda)
{
	for (int i = 0; i < get_party_size(); i++) // iterate through both arrays
	{
		if (auto p = party[i].second.lock()) // check for entity's existance
		{
			for (const auto& v : reward_buffer[i].second) // iterate through reward vector for the selected player
			{

				p->gain_xp(v->get_xp());
				p->gain_gold(v->get_gold());

				for (const auto& [item_id, owned_item] : v->get_reward_item_pointers()) // iterate through item pointer map for the selected reward
				{
					p->add_item_to_inventory(item_id, owned_item);
				}

			}
		}
	}

	reward_buffer.fill({ 0, {} });
	entity_manager_lambda.dirty_entity_checkpoint(data_base_lambda);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::rest_heal()
{
	for (int i = 0; i < get_party_size(); i++)
	{
		if (auto p = party[i].second.lock())
		{
			p->set_current_hp(p->get_max_hp());
			p->set_current_mana(p->get_max_mana());
			p->set_is_dead(false);
		}
	}
	print("Party fully healed!\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::party_resting(const Data_Base_Lambda& data_base_lambda, const Entity_Manager_Lambda& entity_manager_lambda, const Shop_Lambda& shop_lambda)
{
	set_party_state(Party_State::Resting);

	int location_id;

	if (auto p = party[0].second.lock())
	{
		location_id = p->get_current_locaion_id();
	}

	for (int i = 0; i < get_party_size(); i++)
	{
		if (auto p = party[i].second.lock())
		{
			if (location_id != p->get_current_locaion_id())
			{
				print("party members are scattered They must gather to rest\n", 5);
				print("travel to the same location first\n", 5);
				return;
			}
		}
	}

	int choice, inner_choice, inner_choice1;

	rest_heal();

	while (true)
	{
		;

		try
		{
			print_line(5);

			print("1. check for level up\n"
				"2. equip/change equipped weapon or armor\n"
				"3. access the interdimentional vendor, gokron\n"
				"4. share gold between party members\n"
				"5. share item between party members\n"
				"6. stop resting\n", 5);

			choice = input<int>("your choice : ", 5);

			switch (choice)
			{
			case 1:
				for (int i = 0; i < get_party_size(); i++)
				{
					print_line(5);
					if (auto p = party[i].second.lock())
						p->level_up();
				}
				break;

			case 2:
				while (true)
				{
					print_line(5);

					inner_choice = lambda->ask_index("choose which player to equip item\n");
					if (auto p = party[inner_choice - 1].second.lock())
					{
						p->equip_item();
					}
				}
				break;

			case 3:
			{
				print_line(5);

				int player_index = lambda->ask_index("choose which player is shopping");
				shop_lambda.actual_shopping(player_index, *lambda);
			}
			break;

			case 4:
				while (true)
				{
					print_line(5);

					print("choose which player to take gold from and which to give\n", 5);

					lambda->display_party_members_details();

					inner_choice = lambda->ask_index("from player\n");
					inner_choice1 = lambda->ask_index("to player\n");
					int amount = input<int>("the amount of gold to pass : ", 5);

					share_gold(inner_choice - 1, inner_choice1 - 1, amount);

					if (inner_choice == inner_choice1)
					{
						if (auto p = party[inner_choice - 1].second.lock())
						{
							p->display_info();
							print("you took gold from yourself and gave it back to yourself, tf did u expect was going to happen ?\n");
						}
					}
					else
					{
						if (auto p = party[inner_choice - 1].second.lock())
						{
							p->display_info();
						}

						if (auto p = party[inner_choice1 - 1].second.lock())
						{
							p->display_info();
						}
					}
				}
				break;

			case 5:
				while (true)
				{
					print_line(5);

					print("choose which player to take an item or items from and which to give\n", 5);

					lambda->display_party_members_details();

					inner_choice = lambda->ask_index("from player\n");
					inner_choice1 = lambda->ask_index("to player\n");

					int item_id = input<int>("choose which item to give (item's id) : ", 5);

					if (auto p = party[inner_choice - 1].second.lock())
						p->display_inventory();

					int amount = input<int>("the amount of items to pass : ");

					share_item(inner_choice - 1, inner_choice1 - 1, item_id, amount);

					if (inner_choice == inner_choice1)
					{
						if (auto p = party[inner_choice - 1].second.lock())
						{
							p->display_inventory();
							print("you took an item / items from yourself and gave it back to yourself, tf did u expect was going to happen ?\n");
						}
					}
					else
					{
						if (auto p = party[inner_choice - 1].second.lock())
						{
							p->display_info();
							p->display_inventory();
						}

						if (auto p = party[inner_choice1 - 1].second.lock())
						{
							p->display_info();
							p->display_inventory();
						}
					}
				}
				break;

			case 6:
				print("the party stops resting and packs their belongings getting ready to adventure\n", 5);
				return;
				break;

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

	entity_manager_lambda.dirty_entity_checkpoint(data_base_lambda);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::calculate_avg_level()
{
	int avg{ 0 };

	for (int i = 0; i < get_party_size(); i++)
	{
		if (auto p = party[i].second.lock())
		{
			avg += p->get_level();
		}
	}

	set_avg_level(avg / get_party_size());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::share_gold(int from_index, int to_index, int amount)
{
	if (!in_range(1, get_party_size(), from_index))
		throw std::runtime_error("from index does not exist in party");

	if (!in_range(1, get_party_size(), to_index))
		throw std::runtime_error("to index does not exist in party");

	if (amount <= -1)
		throw std::invalid_argument("invalid gold amount entered");


	if (auto p = party[from_index].second.lock())
	{
		if (p->get_gold() < amount)
			throw std::invalid_argument("not enough gold to give");

		p->set_gold(p->get_gold() - amount);
	}

	if (auto p = party[to_index].second.lock())
	{
		p->set_gold(p->get_gold() + amount);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::share_item(int from_index, int to_index, int item_id, int count)
{
	if (!in_range(from_index, 1, get_party_size()))
		throw std::runtime_error("from index does not exist in party");

	if (!in_range(to_index, 1, get_party_size()))
		throw std::runtime_error("to index does not exist in party");

	if (count <= -1)
		throw std::invalid_argument("invalid item count entered");

	if (auto p = party[from_index].second.lock())
	{
		if (!p->get_inventory().count(item_id))
			throw std::invalid_argument("item does not exist in " + p->get_name() + "'s inventory");

		if (p->get_inventory().at(item_id).quantity < count)
			throw std::invalid_argument("not enough item to give");

		if (auto o = party[to_index].second.lock())
		{
			o->add_item_to_inventory(item_id, Owned_Items{ p->get_inventory().at(item_id).item, count });
		}

		p->remove_item_from_inventory(item_id, count);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_get_party()
{
	lambda->get_party = [this]()
		{
			return this->party;
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_get_party_member_stats()
{
	lambda->get_party_member_stats = [this](int index)
		{
			if (auto p = this->party.at(index).second.lock())
				return p->get_stats();

			else
				throw std::runtime_error("weak ptr failed");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_get_party_size()
{
	lambda->get_party_size = [this]()
		{
			return this->party_size;
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_display_party_member_details()
{
	lambda->display_party_members_details = [this]()
		{
			for (int i = 0; i < this->get_party_size(); i++)
			{
				if (auto p = this->party[i].second.lock())
				{
					print(std::to_string(i + 1) + ". ", 5);
					p->display_info();
					p->display_stats();
				}
			}
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_get_party_state()
{
	lambda->get_party_state = [this]()
		{
			return this->party_state;
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_take_gold()
{
	lambda->take_gold = [this](int index, int amount)
		{
			if (!in_range(index, 1, this->get_party_size()))
				throw std::invalid_argument("invalid player index entered");

			if (auto p = this->party[index - 1].second.lock())
			{
				if (amount > p->get_gold())
					throw std::invalid_argument("not enough gold");

				p->set_gold(p->get_gold() - amount);
			}
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_give_gold()
{
	lambda->give_gold = [this](int index, int amount)
		{
			if (!in_range(index, 1, this->get_party_size()))
				throw std::invalid_argument("invalid player index entered");

			if (auto p = this->party[index - 1].second.lock())
				p->set_gold(p->get_gold() + amount);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_take_item()
{
	lambda->take_item = [this](int index, int item_id, int quantity)
		{
			if (!in_range(index, 1, this->get_party_size()))
				throw std::invalid_argument("invalid player index entered");

			if (auto p = this->party[index - 1].second.lock())
			{
				auto it = p->get_inventory().find(item_id);

				if (it == p->get_inventory().end())
					throw std::invalid_argument("item doesnt exist in players inventory");

				if (it->second.quantity < quantity)
					throw std::invalid_argument("not enough items");

				p->remove_item_from_inventory(item_id, quantity);
			}
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_give_item()
{
	lambda->give_item = [this](int index, int item_id, int quantity, std::weak_ptr<Item> item)
		{
			if (item.expired())
				throw std::runtime_error("item no longer exists");

			if (!in_range(index, 1, this->get_party_size()))
				throw std::invalid_argument("invalid player index entered");

			if (auto p = this->party[index - 1].second.lock())
			{
				Owned_Items owned_items{ item, quantity };
				p->add_item_to_inventory(item_id, owned_items);
			}
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_ask_index()
{
	lambda->ask_index = [this](const std::string& prompt)
		{
			while (true)
			{
				print(prompt, 5);
				lambda->display_party_members_details();

				int index = input<int>("your choice (-1 to exit) : ", 5);

				if (in_range(index, 1, this->get_party_size()))
				{
					return index;
				}
				else if (index == -1)
				{
					print("going back\n", 5);
					throw std::exception("user canceled");
				}
				else
					print("invalid choice entered\n", 5);
			}
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Party::set_display_party_member_inventory_and_details()
{
	lambda->display_party_member_inventory_and_details = [this](int index)
		{
			if (!in_range(index, 1, get_party_size()))
				throw std::runtime_error("out of range index entered");

			if (auto p = this->party[index].second.lock())
			{
				p->display_info();
				p->display_inventory();
			}
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

Party::Party(bool new_single_player, Entity_Manager_Lambda& entity_manager_lambda) : single_player(new_single_player), lambda(std::make_unique<Party_Lambda>())
{
	set_party_size(1);
	assign_party_members(entity_manager_lambda);
	calculate_avg_level();
	set_get_party();
	set_get_party_member_stats();
	set_get_party_size();
	set_display_party_member_details();
	set_get_party_state();
	set_take_gold();
	set_give_gold();
	set_take_item();
	set_give_item();
	set_display_party_member_inventory_and_details();
	set_ask_index();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

Party::Party(bool new_single_player, Entity_Manager_Lambda& entity_manager_lambda, int new_party_size) : single_player(new_single_player), lambda(std::make_unique<Party_Lambda>())
{
	set_party_size(new_party_size);
	assign_party_members(entity_manager_lambda);
	calculate_avg_level();
	set_get_party();
	set_get_party_member_stats();
	set_get_party_size();
	set_display_party_member_details();
	set_get_party_state();
	set_take_gold();
	set_give_gold();
	set_take_item();
	set_give_item();
	set_display_party_member_inventory_and_details();
	set_ask_index();
}