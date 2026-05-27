#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include "Utils.h"
#include "Data_Structures.h"
#include "Quest_Manager.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Quest_Manager::set_get_quest_manager()
{
	lambda->get_quest_manager = [this]() -> const std::unordered_map<int, std::shared_ptr<Quest>>&
		{
			return this->quest_manager;
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Quest_Manager::set_load_quest_manager()
{
	lambda->load_quest_manager = [this](int id, std::shared_ptr<Quest> quest)
		{
			this->quest_manager[id] = quest;
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Quest_Manager::set_load_quest_reward()
{
	lambda->load_quest_reward = [this](int id, int reward_id, const Rewards_Manager_Lambda& rewards_manager_lambda)
		{
			this->quest_manager.at(id)->set_reward(rewards_manager_lambda.get_reward(reward_id));
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Quest_Manager::set_load_quest_enemies()
{
	lambda->load_quest_enemies = [this](int id, int entity_id, Enemies new_enemies)
		{
			this->quest_manager.at(id)->add_enemy(entity_id, new_enemies);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Quest_Manager::set_clear_quest_manager()
{
	lambda->clear_quest_manager = [this]()
		{
			this->quest_manager.clear();
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Quest_Manager::add_quest(const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	std::string name, description, quest_giver;
	int quest_level, priority;

	print("please enter the quest details in order\n");

	int id = ask_id(lambda->get_quest_manager(), true, lambda->display_all);
	name = input<std::string>("name : ");
	description = input<std::string>("description : ");
	quest_giver = input<std::string>("quest_giver : ");
	quest_level = input<int>("quest level : ");
	priority = input<int>("prioriry : ");

	try
	{
		quest_manager[id] = std::make_shared<Quest>(name, description, quest_giver, quest_level, priority);
		data_base_lambda.insert_quest(*quest_manager.at(id), id);
		print("new quest made\n");
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		print("quest creation failed\n");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool Quest_Manager::remove_quest(const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	if (!lambda->get_quest_manager().empty())
	{
		print("please enter the id of the quest you with to delete\n");

		int id = ask_id(lambda->get_quest_manager(), false, lambda->display_all);
		char choice;

		print("are you sure you want to delete this quest ?\n");

		quest_manager.at(id)->display_info();

		do
		{
			choice = input<char>("Y/N : ");

			if (to_upper(choice) == 'Y')
			{
				quest_manager.erase(id);
				data_base_lambda.delete_from_main_container(id, Container_Type::Quest);
				print("quest deleted\n");
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
		print("quest container is empty\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Quest_Manager::set_display_all()
{
	lambda->display_all = [this]()
		{
			print_line();

			if (!this->lambda->get_quest_manager().empty())
			{
				for (const auto& v : this->lambda->get_quest_manager())
				{
					print(std::to_string(v.first) + " ");

					v.second->display_info();
				}
			}
			else
				print("quest container is empty\n");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Quest_Manager::set_display_by_priority()
{
	lambda->display_by_priority = [this]()
		{
			print_line();

			bool yes{ true };

			if (!this->lambda->get_quest_manager().empty())
			{
				int priority = input<int>("priority : ");

				for (const auto& v : this->lambda->get_quest_manager())
				{
					if (v.second->get_priority() == priority)
					{
						print(std::to_string(v.first) + " ");

						v.second->display_info();
						yes = false;
					}
				}

				if (yes)
					print("no quest with this priority\n");
			}
			else
				print("quest container is empty\n");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Quest_Manager::set_display_by_quest_level()
{
	lambda->display_by_quest_level = [this]()
		{
			print_line();

			bool yes{ true };

			if (!this->lambda->get_quest_manager().empty())
			{
				int quest_level = input<int>("quest_level : ");

				for (const auto& v : this->lambda->get_quest_manager())
				{
					if (v.second->get_quest_level() == quest_level)
					{
						print(std::to_string(v.first) + " ");

						v.second->display_info();
						yes = false;
					}
				}

				if (yes)
					print("no quest with this level\n");
			}
			else
				print("quest container is empty\n");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Quest_Manager::set_get_quest()
{
	lambda->get_quest = [this](int id)
		{
			print_line();

			if (!this->lambda->get_quest_manager().empty())
				return this->quest_manager.at(id);

			else
				throw std::out_of_range("quest container is empty");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Quest_Manager::update_quest(const Entity_Manager_Lambda& entity_manager_lambda, const Rewards_Manager_Lambda& rewards_manager_lambda, const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	if (!lambda->get_quest_manager().empty())
	{
		print("please enter the id of the quest you wish to update\n");
		int id = ask_id(lambda->get_quest_manager(), false, lambda->display_all);
		int choice;
		bool no_exit{ true };

		while (no_exit)
		{
			try
			{
				print_line();

				print("1.set reward for the quest\n"
					"2.remove a reward from the quest\n"
					"3.add enemies to the quest\n"
					"4.remove enemies from the quest\n"
					"5.update the quest's info\n"
					"6.go back\n");

				choice = input<int>("your choice : ");

				switch (choice)
				{
				case 1:
				{
					print_line();

					print("please enter the reward id you wish to set\n");

					int reward_id = ask_id(rewards_manager_lambda.get_rewards_manager(), false, rewards_manager_lambda.display_all);

					quest_manager.at(id)->set_reward(rewards_manager_lambda.get_reward(reward_id));
					data_base_lambda.insert_quest_reward(id, reward_id);

					print("reward added to quest\n");
					break;
				}

				case 2:
				{
					print_line();

					if (auto p = quest_manager.at(id)->get_reward().lock())
					{
						print("are you sure you want to delete this reward from quest ?\n");
						p->display_details();
					}
					else
					{
						print("no reward to remove\n");
						break;
					}

					char deletion_choice;

					do
					{
						deletion_choice = input<char>("Y/N : ");

						if (to_upper(deletion_choice) == 'Y')
							quest_manager.at(id)->remove_reward();

						else if (to_upper(deletion_choice) == 'N')
							print("removal aborted\n");

						else
							print("invalid choice entered\n");

					} while (to_upper(deletion_choice) != 'Y' && to_upper(deletion_choice) != 'N');
					break;
				}

				case 3:
				{
					print_line();

					int entity_id = ask_id(entity_manager_lambda.get_entity_manager(), false, entity_manager_lambda.display_none_player);
					int quantity = input<int>("quantity : ");

					if (auto p = entity_manager_lambda.get_entity(entity_id).lock())
					{
						if (!p->get_is_player() && quantity > 0)
						{
							Enemies new_enemy{ p, quantity };

							if (quest_manager.at(id)->add_enemy(entity_id, new_enemy))
								data_base_lambda.insert_quest_enemies(id, entity_id, quantity);

							else
								data_base_lambda.update_sub_container_quantity(id, entity_id, quest_manager.at(id)->get_enemies().at(entity_id).quantity + quantity, Sub_Container_Type::Quest_Enemy);

							no_exit = false;

							quest_manager.at(id)->display_info();
							print("done\n");
						}
						else
							print("invalid id entered its a player or quantity is less than 1\n");
					}
					else
						throw std::out_of_range("weak_ptr failed");

					break;
				}

				case 4:
				{
					print_line();

					print("please enter the enemy id you wish to remove from the quest\n");

					int enemy_id = ask_id(quest_manager.at(id)->get_enemies(), false, [this, id]() { this->quest_manager.at(id)->display_info(); });

					print("are you sure you want to remove this enemy from the quest?\n");

					if (auto p = quest_manager.at(id)->get_enemies().at(enemy_id).enemy_ptr.lock())
						p->display_info();
					else
						throw std::out_of_range("weak_ptr failed");

					char deletion_choice = 0;

					do
					{
						deletion_choice = to_upper(input<char>("Y/N : "));

						if (deletion_choice == 'Y')
						{
							// ask for quantity
							print("enter the quantity to remove (0 or negative = remove entire item)\n");
							int qty = input<int>("quantity: ");

							if (qty <= 0)
							{
								// remove whole item
								quest_manager.at(id)->remove_enemy(enemy_id);
								data_base_lambda.delete_from_sub_container(id, enemy_id, Sub_Container_Type::Quest_Enemy);
								print("entire enemy removed\n");
							}
							else
							{
								// remove specific quantity
								quest_manager.at(id)->remove_enemy(enemy_id, qty);
								data_base_lambda.update_sub_container_quantity(id, enemy_id, qty, Sub_Container_Type::Quest_Enemy);
								print("removed ");
								print(std::to_string(qty) + " instances of enemy\n");
							}
						}
						else if (deletion_choice == 'N')
							print("deletion aborted\n");

						else
							print("invalid choice entered\n");

					} while (deletion_choice != 'Y' && deletion_choice != 'N');
					break;
				}

				case 5:
				{
					print_line();

					quest_manager.at(id)->display_info_without_enemies();

					std::string name, description, quest_giver;
					int quest_level, priority;

					print("please enter the updated details\n");
					name = input<std::string>("name : ");
					description = input<std::string>("description : ");
					quest_giver = input<std::string>("quest_giver : ");
					quest_level = input<int>("quest level : ");
					priority = input<int>("prioriry : ");

					quest_manager.at(id)->set_name(name);
					quest_manager.at(id)->set_description(description);
					quest_manager.at(id)->set_quest_giver(quest_giver);
					quest_manager.at(id)->set_priority(priority);

					print("done\n");

					quest_manager.at(id)->display_info_without_enemies();

					data_base_lambda.update_quest(*quest_manager.at(id), id);
					break;
				}

				case 6:
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
		print("quest container is empty\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

Quest_Manager::Quest_Manager() : lambda(std::make_unique<Quest_Manager_Lambda>())
{
	set_get_quest_manager();
	set_load_quest_manager();
	set_load_quest_reward();
	set_load_quest_enemies();
	set_clear_quest_manager();
	set_display_all();
	set_display_by_priority();
	set_display_by_quest_level();
	set_get_quest();
}