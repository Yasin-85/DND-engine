#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <sstream>
#include <limits>
#include <algorithm>
#include <stdexcept>

#include "Utils.h"
#include "Data_Structures.h"
#include "Location_Manager.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Location_Manager::set_get_location_manager()
{
	lambda->get_location_manager = [this]() -> const std::unordered_map<int, std::shared_ptr<Location>>&
		{
			return this->location_manager;
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Location_Manager::set_load_location_manager()
{
	lambda->load_location_manager = [this](int id, std::shared_ptr<Location> location)
		{
			this->location_manager[id] = location;
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Location_Manager::set_load_connected_locations()
{
	lambda->load_connected_locations = [this](int origin_location_id, int target_location_id)
		{
			this->location_manager.at(origin_location_id)->add_connected_location_id(target_location_id);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Location_Manager::set_load_location_chest() 
{
	lambda->load_location_chest = [this](int id, int reward_id, const Rewards_Manager_Lambda& rewards_manager_lambda)
		{
			this->location_manager.at(id)->set_chest(rewards_manager_lambda.get_reward(reward_id));
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Location_Manager::set_clear_location_manager()
{
	lambda->clear_location_manager = [this]()
		{
			this->location_manager.clear();
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Location_Manager::add_location(const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	print("please enter the location details in order\n");

	int id = ask_id(lambda->get_location_manager(), true, lambda->display_all);
	std::string name, property, line;
	std::vector<std::string> properties;

	name = input<std::string>("name : ");

	print("enter properties separated by spaces (if no properties exist just press enter) : \n");

	if (std::cin.peek() == '\n')
		std::cin.ignore();

	getline(std::cin, line);
	std::istringstream stream1(line);
	while (stream1 >> property)
		properties.push_back(property);

	try
	{
		location_manager[id] = std::make_shared<Location>(name, properties);
		data_base_lambda.insert_location(*location_manager.at(id), id);
		print("new location made\n");
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		print("location creation failed\n");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool Location_Manager::remove_location(const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	if (!lambda->get_location_manager().empty())
	{
		print("please enter the id of the location you wish to delete\n");

		char choice;
		int id = ask_id(lambda->get_location_manager(), false, lambda->display_all);

		print("are you sure you want to delete this location ? \n");
		location_manager.at(id)->display_info();

		do
		{
			choice = input<char>("Y/N : ");

			if (to_upper(choice) == 'Y')
			{
				location_manager.erase(id);
				data_base_lambda.delete_from_main_container(id, Container_Type::Location);
				print("location deleted\n");
				return true;
			}
			else if (to_upper(choice) == 'N')
			{
				print("deletion aborted\n");
				return false;
			}

			else
				print("invalid choice eneterd\n");
		} while (to_upper(choice) != 'Y' && to_upper(choice) != 'N');
	}
	else
		print("location container is empty\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Location_Manager::set_display_all()
{
	lambda->display_all = [this]()
		{
			print_line();

			if (!this->lambda->get_location_manager().empty())
			{
				for (const auto& v : this->lambda->get_location_manager())
				{
					print(std::to_string(v.first) + " ");
					v.second->display_info();
				}
			}
			else
				print("location container is empty\n");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Location_Manager::connect_2_locations(const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	if (!lambda->get_location_manager().empty())
	{
		print("please enter the two location ids you wish to connect\n");

		int id1 = ask_id(lambda->get_location_manager(), false, lambda->display_all);
		int id2 = ask_id(lambda->get_location_manager(), false, lambda->display_all);

		if (id1 == id2)
			throw std::invalid_argument("cannot connect a location to itself");

		auto it1 = find(location_manager.at(id1)->get_connected_location_ids().begin(), location_manager.at(id1)->get_connected_location_ids().end(), id2);
		auto it2 = find(location_manager.at(id2)->get_connected_location_ids().begin(), location_manager.at(id2)->get_connected_location_ids().end(), id1);

		if (it1 == location_manager.at(id1)->get_connected_location_ids().end() &&
			it2 == location_manager.at(id2)->get_connected_location_ids().end())
		{
			location_manager.at(id1)->add_connected_location_id(id2);
			location_manager.at(id2)->add_connected_location_id(id1);
		}
		else if (it2 == location_manager.at(id2)->get_connected_location_ids().end())
			location_manager.at(id2)->add_connected_location_id(id1);

		else if (it1 == location_manager.at(id1)->get_connected_location_ids().end())
			location_manager.at(id1)->add_connected_location_id(id2);

		else
			throw std::invalid_argument("two locations already connected");

		data_base_lambda.insert_connected_locations(id1, id2);
		data_base_lambda.insert_connected_locations(id2, id1);

		print("two locations \n" + std::to_string(id1) + " ");
		location_manager.at(id1)->display_info();
		print("and location \n" + std::to_string(id2) + " ");
		location_manager.at(id2)->display_info();
		print("are now connected\n");
	}
	else
		print("location container is empty\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Location_Manager::disconnect_2_locations(const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	if (!lambda->get_location_manager().empty())
	{
		print("please enter the two location ids you wish to disconnect\n");

		int id1 = ask_id(lambda->get_location_manager(), false, lambda->display_all);
		int id2 = ask_id(lambda->get_location_manager(), false, lambda->display_all);

		if (id1 == id2)
			throw std::invalid_argument("cannot disconnect a location from itself");

		auto it1 = find(location_manager.at(id1)->get_connected_location_ids().begin(), location_manager.at(id1)->get_connected_location_ids().end(), id2);
		auto it2 = find(location_manager.at(id2)->get_connected_location_ids().begin(), location_manager.at(id2)->get_connected_location_ids().end(), id1);

		if (it1 != location_manager.at(id1)->get_connected_location_ids().end() &&
			it2 != location_manager.at(id2)->get_connected_location_ids().end())
		{
			location_manager.at(id1)->remove_connected_location_id(id2);
			location_manager.at(id2)->remove_connected_location_id(id1);
		}
		else if (it1 != location_manager.at(id1)->get_connected_location_ids().end())
			location_manager.at(id1)->remove_connected_location_id(id2);

		else if (it2 != location_manager.at(id2)->get_connected_location_ids().end())
			location_manager.at(id2)->remove_connected_location_id(id1);

		else
			throw std::invalid_argument("two locations already disconnected\n");

		data_base_lambda.delete_from_sub_container(id1, id2, Sub_Container_Type::Connected_Location);
		data_base_lambda.delete_from_sub_container(id2, id1, Sub_Container_Type::Connected_Location);

		print("two locations \n" + std::to_string(id1) + " ");
		location_manager.at(id1)->display_info();
		print("and location \n" + std::to_string(id2) + " ");
		location_manager.at(id2)->display_info();
		print("are now disconnected\n");
	}
	else
		print("location container is empty\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Location_Manager::update_location(const Rewards_Manager_Lambda& rewards_manager_lambda, const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	if (!lambda->get_location_manager().empty())
	{
		print("please enter the id of the location you wish to update\n");

		int id = ask_id(lambda->get_location_manager(), false, lambda->display_all);
		int choice;
		bool no_exit{ true };

		while (no_exit)
		{
			try
			{
				print_line();

				print("1.add a reward to the location\n"
					"2.remove a reward from the location\n"
					"3.add a property to the location\n"
					"4.remove a property from the location\n"
					"5.update the location's info\n"
					"6.go back\n");

				choice = input<int>("your choice : ");

				switch (choice)
				{
				case 1:
				{
					print_line();

					print("please enter the reward id you wish to add\n");

					int reward_id = ask_id(rewards_manager_lambda.get_rewards_manager(), false, rewards_manager_lambda.display_all);

					location_manager.at(id)->set_chest(rewards_manager_lambda.get_reward(reward_id));
					data_base_lambda.insert_location_reward(id, reward_id);

					print("reward added\n");
					break;
				}

				case 2:
				{
					print_line();

					char deletion_choice;

					if (auto p = location_manager.at(id)->get_chest().lock())
					{
						print("are you sure you want to delete this reward ?\n");
						p->display_details();
					}

					else
						throw std::runtime_error("ne reward to remove");

					do
					{
						deletion_choice = input<char>("Y/N : ");

						if (to_upper(deletion_choice) == 'Y')
							location_manager.at(id)->remove_chest();

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

					std::string property;

					property = input<std::string>("property you wish to add : ");

					location_manager.at(id)->add_property(property);
					data_base_lambda.update_properties(location_manager.at(id)->get_properties(), Container_Type::Location, id);

					location_manager.at(id)->display_info();
					print("done\n");
					break;
				}

				case 4:
				{
					print_line();

					std::string property;
					char deletion_choice;

					location_manager.at(id)->display_info();

					property = input<std::string>("proeprty you wish to remove : ");

					print("are you sure you want to remove this property ?\n");

					do
					{
						deletion_choice = input<char>("Y/N : ");
						deletion_choice = to_upper(deletion_choice);

						if (deletion_choice == 'Y')
						{
							location_manager.at(id)->remove_property(property);
							data_base_lambda.update_properties(location_manager.at(id)->get_properties(), Container_Type::Location, id);
							location_manager.at(id)->display_info();
							print("done\n");
						}

						else if (deletion_choice == 'N')
							print("removal aborted\n");

						else
							print("invalid choice entered\n");

					} while (deletion_choice != 'Y' && deletion_choice != 'N');

					break;
				}

				case 5:
				{
					print_line();

					location_manager.at(id)->display_info();

					std::string name, property, line;
					std::vector<std::string> properties;

					print("please enter the updated details\n");
					name = input<std::string>("name : ");

					print("enter properties separated by spaces :\n");
					std::cin.ignore(1000, '\n');
					getline(std::cin, line);
					std::istringstream stream1(line);
					while (stream1 >> property)
						properties.push_back(property);

					location_manager.at(id)->set_name(name);
					location_manager.at(id)->set_properties(properties);

					data_base_lambda.update_location(*location_manager.at(id), id);

					location_manager.at(id)->display_info();
					print("done\n");
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
		print("location container is empty\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

Location_Manager::Location_Manager() : lambda(std::make_unique<Location_Manager_Lambda>())
{
	set_get_location_manager();
	set_load_location_manager();
	set_load_connected_locations();
	set_load_location_chest();
	set_clear_location_manager();
	set_display_all();
}