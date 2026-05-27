#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <sstream>
#include <limits>
#include <algorithm>

#include "Utils.h"
#include "Data_Structures.h"
#include "World_Inventory.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

void World_Inventory::set_get_world_inventory()
{
	lambda->get_world_inventory = [this]() -> const std::unordered_map<int, std::shared_ptr<Item>>&
		{
			return this->world_inventory;
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void World_Inventory::set_load_world_inventory()
{
	lambda->load_world_inventory = [this](int id, std::shared_ptr<Item> item)
		{
			this->world_inventory[id] = item;
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void World_Inventory::set_clear_world_inventory()
{
	lambda->clear_world_inventory = [this]() { this->world_inventory.clear(); };
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void World_Inventory::add_item(const Data_Base_Lambda& data_base_lambda)
{
	int id;
	int choice;
	bool no_exit{ true };

	std::string name, type;
	std::vector<std::string> properties;
	std::string property, line;
	int value;
	bool is_magical;

	int dmg_dice, dmg_bonus, dmg_dice_count;
	float range;

	int armor_class, max_dex_bonus;
	bool stealth_friendly;

	std::string element;
	int mana_cost;

	int health_restore, mana_restore, level_requirement;
	while (no_exit)
	{
		print_line();

		print("what item do u wish to make 1.weapon  2.armor  3.spell scroll  4.consumable  5.go back\n");
		choice = input<int>("your choice : ");

		properties.clear();

		switch (choice)
		{
		case 1:
			//WEAPON
		{
			print_line();
			print("please enter the weapon details in order\n");

			id = ask_id(lambda->get_world_inventory(), true, lambda->display_all);
			name = input<std::string>("name : ");
			type = input<std::string>("type : ");
			value = input<int>("value (gold) : ");
			is_magical = input<bool>("is it magical (1 for true, 0 for false) : ");

			dmg_dice = input<int>("dice type (4, 6, 8, 10, 12, 20, 100) : D");
			dmg_dice_count = input<int>("dice count : ");
			dmg_bonus = input<int>("damage bonus : ");
			range = input<float>("range (meters) : ");

			if (std::cin.peek() == '\n')
				std::cin.ignore();
			print("enter properties separated by spaces (if no properties exist just press enter) : \n");
			std::cin.ignore(1000, '\n');
			getline(std::cin, line);
			std::istringstream stream1(line);
			while (stream1 >> property)
				properties.push_back(property);

			try
			{
				world_inventory[id] = std::make_shared<Weapon>(name, type, value, is_magical,
					properties, dmg_dice, dmg_bonus,
					dmg_dice_count, range);

				data_base_lambda.insert_item(*world_inventory.at(id), id);
				print("new weapon created\n");
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << '\n';
				print("weapon creation failed\n");
			}
			break;
		}
		case 2:
			//ARMOR
		{
			print_line();
			print("please enter the armor details in order\n");

			id = ask_id(lambda->get_world_inventory(), true, lambda->display_all);
			name = input<std::string>("name : ");
			type = input<std::string>("type : ");
			value = input<int>("value (gold) : ");
			is_magical = input<bool>("is it magical (1 for true, 0 for false) : ");

			armor_class = input<int>("armor class (0 - 20) : ");
			max_dex_bonus = input<int>("maximum dex bonus : ");
			stealth_friendly = input<bool>("is it stealthy (1 for true, 0 for false) : ");

			if (std::cin.peek() == '\n')
				std::cin.ignore();
			print("enter properties separated by spaces (if no properties exist just press enter) : \n");
			std::cin.ignore(1000, '\n');
			getline(std::cin, line);
			std::istringstream stream2(line);
			while (stream2 >> property)
				properties.push_back(property);

			try
			{
				world_inventory[id] = std::make_shared<Armor>(name, type, value, is_magical,
					properties, armor_class,
					max_dex_bonus, stealth_friendly);

				data_base_lambda.insert_item(*world_inventory.at(id), id);
				print("new armor created\n");
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << '\n';
				print("armor creation failed\n");
			}
			break;
		}
		case 3:
			//SPELL
		{
			print_line();
			print("please enter the spell Scroll details in order\n");

			id = ask_id(lambda->get_world_inventory(), true, lambda->display_all);
			name = input<std::string>("name : ");
			type = input<std::string>("type : ");
			value = input<int>("value (gold) : ");
			is_magical = input<bool>("is it magical (1 for true, 0 for false) : ");

			dmg_dice = input<int>("dice type (4, 6, 8, 10 ,12 ,20 ,100) : D");
			dmg_dice_count = input<int>("dice count : ");
			dmg_bonus = input<int>("damage bonus : ");
			range = input<float>("range (meters) : ");
			element = input<std::string>("element : ");
			mana_cost = input<int>("mana cost : ");

			if (std::cin.peek() == '\n')
				std::cin.ignore();
			print("enter properties separated by spaces (if no properties exist just press enter) : \n");
			std::cin.ignore(1000, '\n');
			getline(std::cin, line);
			std::istringstream stream3(line);
			while (stream3 >> property)
				properties.push_back(property);

			try
			{
				world_inventory[id] = std::make_shared<Spell_Scroll>(name, type, value, is_magical,
					properties, dmg_dice, dmg_bonus,
					dmg_dice_count, range, element, mana_cost);

				data_base_lambda.insert_item(*world_inventory.at(id), id);
				print("new spell Scroll created\n");
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << '\n';
				print("spell Scroll creation failed\n");
			}
			break;
		}
		case 4:
			//CONSUMABLE
		{
			print_line();
			print("please enter the Consumable details in order\n");

			id = ask_id(lambda->get_world_inventory(), true, lambda->display_all);
			name = input<std::string>("name : ");
			type = input<std::string>("type : ");
			value = input<int>("value (gold) : ");
			is_magical = input<bool>("is it magical (1 for true, 0 for false) : ");

			health_restore = input<int>("health restoration points : ");
			mana_restore = input<int>("mana resoration points : ");
			level_requirement = input<int>("level requirement : ");

			print("enter properties separated by spaces (if no properties exist just press enter) : \n");
			std::cin.ignore(1000, '\n');
			getline(std::cin, line);
			std::istringstream stream3(line);
			while (stream3 >> property)
				properties.push_back(property);

			try
			{
				world_inventory[id] = std::make_shared<Consumable>(name, type, value, is_magical,
					properties, health_restore, mana_restore, level_requirement);

				data_base_lambda.insert_item(*world_inventory.at(id), id);
				print("new Consumable created\n");
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << '\n';
				print("Consumable creation failed\n");
			}
			break;
		}

		case 5:
			print("going back\n");
			no_exit = false;
			break;

		default:
			throw std::invalid_argument("wrong item type entered (weapon, armor, spell)");
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool World_Inventory::remove_item(const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	if (!lambda->get_world_inventory().empty())
	{
		print("please enter the item id you wish to delete\n");

		int id = ask_id(lambda->get_world_inventory(), false, lambda->display_all);
		char choice;

		print("are u sure u want to delete this item ?\n");
		world_inventory.at(id)->display_details();
		world_inventory.at(id)->display_properties();

		do
		{
			choice = input<char>("Y/N : ");
			choice = to_upper(choice);

			if (choice == 'Y')
			{
				world_inventory.erase(id);
				data_base_lambda.delete_from_main_container(id, Container_Type::Item);
				print("item deleted\n");
				return true;
			}

			else if (choice == 'N')
			{
				print("deletion aborted\n");
				return false;
			}

			else
				print("invalid choice entered\n");

		} while (choice != 'Y' && choice != 'N');
	}
	else
		print("item container is empty\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void World_Inventory::set_display_all()
{
	lambda->display_all = [this]()
		{
			print_line();

			if (!this->lambda->get_world_inventory().empty())
			{
				for (const auto& it : this->world_inventory)
				{
					print(std::to_string(it.first) + " ");
					it.second->display_details();
					it.second->display_properties();
				}
			}
			else
				print("item container is empty\n");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void World_Inventory::set_display_by_class_child_type()
{
	lambda->display_by_class_child_type = [this]()
		{
			print_line();

			if (!this->lambda->get_world_inventory().empty())
			{
				try
				{
					Item_Type type = input<Item_Type>("please enter the item type (weapon, armor, spell_scroll, consumable) : ");

					if (type != Item_Type::Weapon && type != Item_Type::Armor && type != Item_Type::Spell_Scroll && type != Item_Type::Consumable)
					{
						throw std::invalid_argument("invalid item type entered (weapon, armor, spell_scroll, consumable)");
					}

					for (const auto& it : this->world_inventory)
					{
						if (it.second->child_type() == type)
						{
							print(std::to_string(it.first) + " ");
							it.second->display_details();
							it.second->display_properties();
						}
					}
				}
				catch (const std::exception& e)
				{
					std::cerr << e.what() << '\n';
				}
			}
			else
				print("item container is empty\n");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void World_Inventory::set_display_by_name()
{
	lambda->display_by_name = [this]()
		{
			print_line();

			if (!this->lambda->get_world_inventory().empty())
			{
				std::vector<int> ids;
				std::string name;
				name = input<std::string>("please enter the name of the item : ");

				for (const auto& it : this->world_inventory)
				{
					if (it.second->get_name() == name)
						ids.push_back(it.first);
				}

				if (!ids.empty())
				{
					print(std::to_string(ids.size()) + " items found\n");

					for (const auto& v : ids)
					{
						print(std::to_string(v) + " ");
						world_inventory.at(v)->display_details();
						world_inventory.at(v)->display_properties();
					}
				}
				else
					print("no item with the name of " + name + " was found\n");
			}
			else
				print("item container is empty\n");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void World_Inventory::set_get_item()
{
	lambda->get_item = [this](int id)
		{
			if (!this->lambda->get_world_inventory().empty())
				return this->world_inventory.at(id);

			else
				throw std::out_of_range("item container is empty");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void World_Inventory::update_item(const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	if (!lambda->get_world_inventory().empty())
	{
		print("please enter the id of the item you wish to update\n");
		int id = ask_id(lambda->get_world_inventory(), false, lambda->display_all);
		int choice;
		bool no_exit{ true };

		while (no_exit)
		{
			try
			{
				print_line();
				print("1.add a property to properties\n"
					"2.remove a proeprty from properties\n"
					"3.update the item's info\n"
					"4.go back\n");

				choice = input<int>("your choice : ");

				switch (choice)
				{
				case 1:
				{
					print_line();

					std::string property;
					
					world_inventory.at(id)->display_properties();

					property = input<std::string>("property you wish to add : ");

					world_inventory.at(id)->add_property(property);
					data_base_lambda.update_properties(world_inventory.at(id)->get_properties(), Container_Type::Item, id);

					world_inventory.at(id)->display_properties();
					print("done\n");
					break;
				}

				case 2:
				{
					print_line();

					std::string property;
					char deletion_choice;

					world_inventory.at(id)->display_properties();

					property = input<std::string>("proeprty you wish to remove : ");

					print("are you sure you want to remove this property ?\n");

					do
					{
						deletion_choice = input<char>("Y/N : ");
						deletion_choice = to_upper(deletion_choice);

						if (deletion_choice == 'Y')
						{
							world_inventory.at(id)->remove_property(property);
							data_base_lambda.update_properties(world_inventory.at(id)->get_properties(), Container_Type::Item, id);
						}

						else if (deletion_choice == 'N')
							print("removal aborted\n");

						else
							print("invalid choice entered\n");

					} while (deletion_choice != 'Y' && deletion_choice != 'N');
					break;
				}

				case 3:
				{
					print_line();

					world_inventory.at(id)->display_details();

					std::string name, type;
					std::vector<std::string> properties;
					std::string property;
					int value;
					bool is_magical;

					int dmg_dice, dmg_bonus, dmg_dice_count;
					float range;

					int armor_class, max_dex_bonus;
					bool stealth_friendly;

					int mana_cost;
					std::string element;

					int health_restore, mana_restore, level_requirement;

					print("please enter the updated details\n");
					name = input<std::string>("name : ");
					type = input<std::string>("type : ");
					value = input<int>("value : ");
					is_magical = input<bool>("is it magical (1 for true, 0 for false) : ");

					world_inventory.at(id)->set_name(name);
					world_inventory.at(id)->set_type(type);
					world_inventory.at(id)->set_value(value);
					world_inventory.at(id)->set_magical(is_magical);

					switch (world_inventory.at(id)->child_type())
					{
					case Item_Type::Weapon:
					{
						dmg_dice = input<int>("dice type (4, 6, 8, 10, 12, 20, 100) : D");
						dmg_dice_count = input<int>("dice count : ");
						dmg_bonus = input<int>("damage bonus : ");
						range = input<float>("range (meters) : ");

						world_inventory.at(id)->set_dmg_dice(dmg_dice);
						world_inventory.at(id)->set_dmg_count(dmg_dice_count);
						world_inventory.at(id)->set_dmg_bonus(dmg_bonus);
						world_inventory.at(id)->set_range(range);

						break;
					}

					case Item_Type::Armor:
					{
						armor_class = input<int>("armor class : ");
						max_dex_bonus = input<int>("maximum dex bonus : ");
						stealth_friendly = input<bool>("is it stealthy (1 for true, 0 for false) : ");

						world_inventory.at(id)->set_armor_class(armor_class);
						world_inventory.at(id)->set_dex_bonus(max_dex_bonus);
						world_inventory.at(id)->set_stealth_friendly(stealth_friendly);

						break;
					}

					case Item_Type::Spell_Scroll:
					{
						dmg_dice = input<int>("dice type (4, 6, 8, 10, 12, 20, 100) : D");
						dmg_dice_count = input<int>("dice count : ");
						dmg_bonus = input<int>("damage bonus : ");
						range = input<float>("range (meters) : ");
						mana_cost = input<int>("mana cost : ");
						element = input<std::string>("element : ");

						world_inventory.at(id)->set_dmg_dice(dmg_dice);
						world_inventory.at(id)->set_dmg_count(dmg_dice_count);
						world_inventory.at(id)->set_dmg_bonus(dmg_bonus);
						world_inventory.at(id)->set_range(range);
						world_inventory.at(id)->set_mana_cost(mana_cost);
						world_inventory.at(id)->set_element(element);

						break;
					}

					case Item_Type::Consumable:
					{
						health_restore = input<int>("health restore amount : ");
						mana_restore = input<int>("mana restore amount : ");
						level_requirement = input<int>("level requirement : ");

						world_inventory.at(id)->set_health_restore(health_restore);
						world_inventory.at(id)->set_mana_restore(mana_restore);
						world_inventory.at(id)->set_level_requirement(level_requirement);

						break;
					}

					default:
						throw std::runtime_error("invalid item type");
						break;
					}

					world_inventory.at(id)->display_details();

					data_base_lambda.update_item(*world_inventory.at(id), id);

					print("done\n");
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
		print("item container is empty\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

World_Inventory::World_Inventory() : lambda(std::make_unique<World_Inventory_Lambda>())
{
	set_get_world_inventory();
	set_load_world_inventory();
	set_clear_world_inventory();
	set_display_all();
	set_display_by_class_child_type();
	set_display_by_name();
	set_get_item();
}