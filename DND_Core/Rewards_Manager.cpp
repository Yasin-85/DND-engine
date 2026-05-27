#include <iostream>

#include "Utils.h"
#include "Data_Structures.h"
#include "Rewards_Manager.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Rewards_Manager::set_get_rewards_manager()
{
	lambda->get_rewards_manager = [this]() -> const std::unordered_map<int, std::shared_ptr<Rewards>>&
		{
			return this->rewards_manager;
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Rewards_Manager::set_load_rewards_manager()
{
	lambda->load_rewards_manager = [this](int id, std::shared_ptr<Rewards> rewards)
		{
			this->rewards_manager[id] = rewards;
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Rewards_Manager::set_load_reward_items()
{
	lambda->load_reward_items = [this](int reward_id, int item_id, const Owned_Items new_item)
		{
			rewards_manager.at(reward_id)->add_item_pointers(item_id, new_item);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Rewards_Manager::set_clear_rewards_manager()
{
	lambda->clear_rewards_manager = [this]()
		{
			this->rewards_manager.clear();
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Rewards_Manager::add_reward(const World_Inventory_Lambda& world_inventory_lambda, const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	int gold, xp, id;
	
	print("please enter the reward details in order\n");

	id = ask_id(lambda->get_rewards_manager(), true, lambda->display_all);
	gold = input<int>("gold : ");
	xp = input<int>("xp : ");

	try
	{
		rewards_manager[id] = std::make_shared<Rewards>(gold, xp);
		data_base_lambda.insert_reward(*rewards_manager.at(id), id);
		print("new reward made\n");
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		print("reward creation failed\n");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool Rewards_Manager::remove_reward(const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	if (!lambda->get_rewards_manager().empty())
	{
		print("please enter the id of the reward you with to delete\n");

		int id = ask_id(lambda->get_rewards_manager(), false, lambda->display_all);
		char choice;

		print("are you sure you want to delete this reward ? \n");
		rewards_manager.at(id)->display_details();

		do
		{
			choice = input<char>("Y/N : ");

			if (to_upper(choice) == 'Y')
			{
				rewards_manager.erase(id);
				data_base_lambda.delete_from_main_container(id, Container_Type::Reward);
				print("reward delete\n");
				return true;
			}
			else if (to_upper(choice) == 'N')
			{
				print("deletion aborted\n");
				return false;
			}

			else
				print("invalid choice entered\n");
		} while (to_upper(choice) != 'Y' && to_upper(choice) != 'N');
	}
	else
		print("rewards container is empty\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Rewards_Manager::set_get_reward()
{
	lambda->get_reward = [this](int id)
		{
			if (!this->lambda->get_rewards_manager().empty())
				return this->rewards_manager.at(id);

			else
				throw std::out_of_range("rewards container is empty");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Rewards_Manager::set_display_all()
{
	lambda->display_all = [this]()
		{
			print_line();

			if (!this->lambda->get_rewards_manager().empty())
			{
				for (const auto& v : this->rewards_manager)
				{
					print(std::to_string(v.first) + " ");
					v.second->display_details();
				}
			}
			else
				print("rewards container is empty\n");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void  Rewards_Manager::update_reward(const World_Inventory_Lambda& world_inventory_lambda, const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	if (!lambda->get_rewards_manager().empty())
	{
		print("please enter the id of the reward you wish to update\n");

		int id = ask_id(lambda->get_rewards_manager(), false, lambda->display_all);
		int choice;
		bool no_exit{ true };

		while (no_exit)
		{
			try
			{
				print_line();

				print("1.add an item to the reward pool\n"
					"2.remove an item from the reward pool\n"
					"3.update gold and xp\n"
					"4.go back\n");

				choice = input<int>("your choice : ");

				switch (choice)
				{
				case 1:
				{
					print_line();

					print("please enter the item id you wish to add\n");

					Owned_Items owned;
					int item_id = ask_id(world_inventory_lambda.get_world_inventory(), false, world_inventory_lambda.display_all);
					owned.item = world_inventory_lambda.get_item(item_id);
					owned.quantity = input<int>("quantity : ");

					rewards_manager.at(id)->add_item_pointers(item_id, owned);
					data_base_lambda.insert_reward_items(id, item_id, owned.quantity);

					print("done\n");

					rewards_manager.at(id)->display_details();
					break;
				}

				case 2:
				{
					print_line();

					print("please enter the item id you wish to remove\n");

					int item_id = ask_id(world_inventory_lambda.get_world_inventory(), false, [this, id]() { print_line();  this->rewards_manager.at(id)->display_details(); });
					std::weak_ptr<Item> item_pointer = world_inventory_lambda.get_item(item_id);

					print("are you sure you want to remove this item from the reward pool?\n");

					if (auto p = item_pointer.lock())
						p->display_details();
					else
						throw std::out_of_range("weak_ptr failed");

					char deletion_choice;

					do
					{
						deletion_choice = to_upper(input<char>("Y/N : "));

						if (deletion_choice == 'Y')
						{
							// ask for quantity to remove
							print("enter the quantity to remove (0 or negative = remove entire item):\n");
							int qty = input<int>("quantity: ");

							if (qty <= 0)
							{
								// remove whole item from reward pool
								rewards_manager.at(id)->remove_item_pointers(item_id);
								data_base_lambda.delete_from_sub_container(id, item_id, Sub_Container_Type::Reward_Item);
								print("entire item removed from reward\n");
							}
							else
							{
								// remove only a specific amount
								rewards_manager.at(id)->remove_item_pointers(item_id, qty);
								data_base_lambda.update_sub_container_quantity(id, item_id, qty, Sub_Container_Type::Reward_Item);
								print("removed ");
								print(std::to_string(qty) + " instances of reward item\n");
							}
						}
						else if (deletion_choice == 'N')
							print("deletion aborted\n");
						
						else
							print("invalid choice entered\n");

					} while (deletion_choice != 'Y' && deletion_choice != 'N');

					break;
				}

				case 3:
				{
					print_line();

					rewards_manager.at(id)->display_details();

					int new_gold, new_xp;

					print("please enter the updated details\n");
					new_gold = input<int>("gold : ");
					new_xp = input<int>("xp : ");

					rewards_manager.at(id)->set_gold(new_gold);
					rewards_manager.at(id)->set_xp(new_xp);

					print("done\n");

					rewards_manager.at(id)->display_details();

					data_base_lambda.update_reward(*rewards_manager.at(id), id);
					break;
				}

				case 4:
					print("going back\n");
					no_exit = false;
					break;

				default:
					print("invalid choice entered\n");
					break;
				}
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
	}
	else
		print("rewards container is empty\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

Rewards_Manager::Rewards_Manager() : lambda(std::make_unique<Rewards_Manager_Lambda>())
{
	set_get_rewards_manager();
	set_load_rewards_manager();
	set_load_reward_items();
	set_clear_rewards_manager();
	set_get_reward();
	set_display_all();
}