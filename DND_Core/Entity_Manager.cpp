#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <string>
#include <memory>
#include <stdexcept>

#include "Utils.h"
#include "Data_Structures.h"
#include "Entity_Manager.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity_Manager::set_get_entity_manager()
{
	lambda->get_entity_manager = [this]() -> const std::unordered_map<int, std::shared_ptr<Entity>>&
		{
			return this->entity_manager;
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity_Manager::set_load_entity_manager()
{
	lambda->load_entity_manager = [this](int id, std::shared_ptr<Entity> entity)
		{
			this->entity_manager[id] = entity;
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity_Manager::set_load_inventories() 
{
	lambda->load_inventories = [this](int entity_id, int item_id, const Owned_Items& new_item)
		{
			this->entity_manager.at(entity_id)->add_item_to_inventory(item_id, new_item);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity_Manager::set_link_equipment() 
{
	lambda->link_equipment = [this](int entity_id, int weapon_id, int armor_id, const World_Inventory_Lambda& world_inventory_lambda)
		{
			std::shared_ptr<Entity> entity = this->entity_manager.at(entity_id);

			if (weapon_id > 0)
			{
				const auto& inventory = entity->get_inventory();

				if (inventory.find(weapon_id) != inventory.end())
					entity->set_equipped_weapon_internal(std::make_pair(weapon_id, world_inventory_lambda.get_item(weapon_id)));
			}

			if (armor_id > 0)
			{
				const auto& inventory = entity->get_inventory();

				if (inventory.find(armor_id) != inventory.end())
					entity->set_equipped_armor_internal(std::make_pair(armor_id, world_inventory_lambda.get_item(armor_id)));
			}
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity_Manager::set_load_comeplted_and_ongoing_quests()
{
	lambda->load_completed_and_ongoing_quests = [this](int entity_id, int quest_id, bool completed)
		{
			if (completed)
				this->entity_manager.at(entity_id)->add_completed_quest(quest_id);

			else
				this->entity_manager.at(entity_id)->add_ongoing_quest(quest_id);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity_Manager::set_clear_entity_manager()
{
	lambda->clear_entity_manager = [this]()
		{
			this->entity_manager.clear();
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity_Manager::add_entity(const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	int new_id;
	std::string new_name, new_background, new_class_type;
	Attributes new_stats;
	bool new_is_player;

	print("please enter the entity details in order\n");

	new_id = ask_id(lambda->get_entity_manager(), true, lambda->display_all);
	new_name = input<std::string>("name : ");
	new_background = input<std::string>("background : ");
	new_class_type = input<std::string>("class type : ");
	new_is_player = input<bool>("is player (1 for yes, 0 for no) : ");
	new_stats.cha_ = input<int>("charisma points (1 - 20) : ");
	new_stats.cons_ = input<int>("constitution points (1 - 20) : ");
	new_stats.dex_ = input<int>("dextirity points (1 - 20) : ");
	new_stats.int_ = input<int>("inteliigence points (1 - 20) : ");
	new_stats.str_ = input<int>("strength points (1 - 20) : ");
	new_stats.wis_ = input<int>("wisdom points (1 - 20) : ");

	try
	{
		entity_manager[new_id] = std::make_shared<Entity>(new_name, new_background, new_class_type, new_stats, new_is_player);
		data_base_lambda.insert_entity(*entity_manager.at(new_id), new_id);
		print("new entity made\n");
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		print("entity creation failed\n");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool Entity_Manager::remove_entity(const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	int id;
	char choice;
	if (!lambda->get_entity_manager().empty())
	{
		print("please enter the entity id you wish to delete\n");

		id = ask_id(lambda->get_entity_manager(), false, lambda->display_all);

		entity_manager.at(id)->display_info();
		entity_manager.at(id)->display_stats();

		do
		{
			choice = input<char>("are you sure you want to delete this entity ?  Y/N : ");

			if (to_upper(choice) == 'Y')
			{
				entity_manager.erase(id);
				data_base_lambda.delete_from_main_container(id, Container_Type::Entity);
				print("entity deleted\n");
				return true;
			}
			else if (to_upper(choice) == 'N')
			{
				print("deletion aborted\n");
				return false;
			}

			else
				print("invalid input entered\n");
		} while (to_upper(choice) != 'Y' && to_upper(choice) != 'N');
	}
	else
		print("entity container is empty\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity_Manager::set_display_all()
{
	lambda->display_all = [this]()
		{
			print_line();

			if (!this->lambda->get_entity_manager().empty())
			{
				for (const auto& v : this->lambda->get_entity_manager())
				{
					print(std::to_string(v.first) + " ");
					v.second->display_info();
				}
			}
			else
				print("entity container is empty\n");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity_Manager::set_display_player()
{
	lambda->display_player = [this]()
		{
			print_line();

			int count{ 0 };
			if (!this->lambda->get_entity_manager().empty())
			{
				for (const auto& v : this->lambda->get_entity_manager())
				{
					if (v.second->get_is_player())
					{
						print(std::to_string(v.first) + " ");
						v.second->display_info();
						count++;
					}
				}
				if (count == 0)
					print("no player exists in the container\n");
			}
			else
				print("entity container is empty\n");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity_Manager::set_display_none_player()
{
	lambda->display_none_player = [this]()
		{
			print_line();

			int count{ 0 };
			if (!this->lambda->get_entity_manager().empty())
			{
				for (const auto& v : this->lambda->get_entity_manager())
				{
					if (!v.second->get_is_player())
					{
						print(std::to_string(v.first) + " ");
						v.second->display_info();
						count++;
					}
				}
				if (count == 0)
					print("no none player exists in the container\n");
			}
			else
				print("entity container is empty\n");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<int> Entity_Manager::find_by_name(bool player_only, bool show_id)
{
	print_line();

	if (!lambda->get_entity_manager().empty())
	{
		std::string name = input<std::string>("name to search for : ");
		std::vector<int> ids;

		for (const auto& v : entity_manager)
		{
			if (v.second->get_name() == name)
			{
				if (player_only && v.second->get_is_player())
				{
					ids.push_back(v.first);
					continue;
				}

				ids.push_back(v.first);
			}
		}

		if (ids.empty())
		{
			print("no entity with name " + name + " was found\n");

			return {};
		}
		else
		{
			print(std::to_string(ids.size()) + " entity(ies) with name " + name + " was found\n");

			for (const auto& v : ids)
			{
				if (show_id)
					print(std::to_string(v) + " ");

				entity_manager.at(v)->display_info();
			}

			return ids;
		}
	}
	else
	{
		print("entity container is empty\n");
		return {};
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity_Manager::set_get_entity()
{
	lambda->get_entity = [this](int id)
		{
			if (!this->lambda->get_entity_manager().empty())
				return this->entity_manager.at(id);

			else
				throw std::out_of_range("entity container is empty");
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity_Manager::set_dirty_entity_checkpoint()
{
	lambda->dirty_entity_checkpoint = [this](const Data_Base_Lambda& data_base_lambda)
		{
			for (const auto& [id, entity] : this->entity_manager)
			{
				if (entity->get_dirty())
				{
					data_base_lambda.update_entity(*entity, id);
					entity->set_dirty(false);
				}
			}
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity_Manager::update_entity(const World_Inventory_Lambda& world_inventory_lambda, 
	const Quest_Manager_Lambda& quest_manager_lambda, 
	const Location_Manager_Lambda& location_manager_lambda, 
	const Data_Base_Lambda& data_base_lambda)
{
	print_line();

	if (!lambda->get_entity_manager().empty())
	{
		print("please enter the id of the entity you wish to update\n");

		int id = ask_id(lambda->get_entity_manager(), false, lambda->display_all);
		bool no_exit{ true };
		int choice;
		int item_id, quest_id;

		while (no_exit)
		{
			try
			{
				print_line();

				print("1.display all info about this entity\n"
					"2.add an item to entity's inventory\n"
					"3.remove an item from entity's inventory\n"
					"4.add a completed quest to the entity\n"
					"5.remove a completed quest from the entity\n"
					"6.add an ongoing quest to the entity\n"
					"7.remove an ongoing quest from the entity\n"
					"8.display all items in inventory\n"
					"9.equip an item from entity's inventory\n"
					"10.mandatory xp gain\n"
					"11.update entity info\n"
					"12.go back\n");

				choice = input<int>("your choice : ");

				switch (choice)
				{
				case 1:
					print_line();

					entity_manager.at(id)->display_stats();
					entity_manager.at(id)->display_info();
					entity_manager.at(id)->display_inventory();
					break;

				case 2:
				{
					print_line();

					print("please enter the item id you wish to add to the entity's inventory\n");

					item_id = ask_id(world_inventory_lambda.get_world_inventory(), false, world_inventory_lambda.display_all);
					Owned_Items owned;
					owned.quantity = input<int>("quantity of item : ");
					owned.item = world_inventory_lambda.get_item(item_id);

					entity_manager.at(id)->add_item_to_inventory(item_id, owned);
					data_base_lambda.insert_inventories(id, item_id, owned.quantity);
					print("item added\n");
					break;
				}

				case 3:
				{
					print_line();

					print("please enter the item id you wish to remove from the entity's inventory\n");

					int item_id = ask_id(entity_manager.at(id)->get_inventory(), false, [this, id]() {this->entity_manager.at(id)->display_inventory(); });

					print("are you sure you want to remove this item from the entity's inventory?\n");

					if (auto p = world_inventory_lambda.get_item(item_id).lock())
						p->display_details();
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
								entity_manager.at(id)->remove_item_from_inventory(item_id);
								data_base_lambda.delete_from_sub_container(id, item_id, Sub_Container_Type::Inventory);
							}
							else
							{
								// remove specific quantity
								entity_manager.at(id)->remove_item_from_inventory(item_id, qty);
								data_base_lambda.update_sub_container_quantity(id, item_id, qty, Sub_Container_Type::Inventory);
							}
						}
						else if (deletion_choice == 'N')
							print("deletion aborted\n");

						else
							print("invalid choice entered\n");

					} while (deletion_choice != 'Y' && deletion_choice != 'N');

					break;
				}

				case 4:
				{
					print_line();
					
					print("please enter the completed quest id you wish to add\n");

					quest_id = ask_id(quest_manager_lambda.get_quest_manager(), false, lambda->display_all);

					entity_manager.at(id)->add_completed_quest(quest_id);
					entity_manager.at(id)->remove_ongoing_quest(quest_id);
					data_base_lambda.insert_completed_and_ongoing_quest(id, quest_id, true);
					break;
				}

				case 5:
				{
					print_line();

					if (entity_manager.at(id)->get_completed_quests().empty())
						throw std::runtime_error("container empty");

					print("please enter the completed quest id you with to remove\n");

					char deletion_choice;
					quest_id = ask_id(quest_manager_lambda.get_quest_manager(), false, lambda->display_all);

					print("are you sure you want to remove this completed quest from the entity\n");

					if (auto p = quest_manager_lambda.get_quest(quest_id).lock())
						p->display_info_without_enemies();

					else
						throw std::out_of_range("weak_ptr failed");

					do
					{
						deletion_choice = input<char>("Y/N : ");
						deletion_choice = to_upper(deletion_choice);

						if (deletion_choice == 'Y')
						{
							entity_manager.at(id)->remove_completed_quest(quest_id);
							data_base_lambda.delete_from_sub_container(id, quest_id, Sub_Container_Type::Completed_And_Ongoing_quest);
						}

						else if (deletion_choice == 'N')
							print("deletion aborted\n");

						else
							print("invalid choice entered\n");

					} while (deletion_choice != 'Y' && deletion_choice != 'N');
					break;
				}

				case 6:
				{
					print_line();

					print("please enter the ongoing quest id you with to add\n");

					quest_id = ask_id(quest_manager_lambda.get_quest_manager(), false, lambda->display_all);

					entity_manager.at(id)->add_ongoing_quest(quest_id);
					entity_manager.at(id)->remove_completed_quest(quest_id);
					data_base_lambda.insert_completed_and_ongoing_quest(id, quest_id, false);
					break;
				}

				case 7:
					print_line();

					print("please enter the ongoing quest id you with to remove\n");

					char deletion_choice;
					quest_id = ask_id(quest_manager_lambda.get_quest_manager(), false, lambda->display_all);

					print("are you sure you want to remove this ongoing quest from the entity\n");

					if (auto p = quest_manager_lambda.get_quest(quest_id).lock())
						p->display_info_without_enemies();

					else
						throw std::out_of_range("weak_ptr failed");

					do
					{
						deletion_choice = input<char>("Y/N : ");
						deletion_choice = to_upper(deletion_choice);

						if (deletion_choice == 'Y')
						{
							entity_manager.at(id)->remove_ongoing_quest(quest_id);
							data_base_lambda.delete_from_sub_container(id, quest_id, Sub_Container_Type::Completed_And_Ongoing_quest);
						}

						else if (deletion_choice == 'N')
							print("deletion aborted\n");

						else
							print("invalid choice entered\n");

					} while (deletion_choice != 'Y' && deletion_choice != 'N');
					break;

				case 8:
					print_line();
					entity_manager.at(id)->display_inventory();
					break;

				case 9:
				{
					print_line();
					if (!entity_manager.at(id)->get_inventory().empty())
					{
						entity_manager.at(id)->equip_item();
					}
					else
					{
						throw std::runtime_error("character inventory is empty");
					}
					break;
				}

				case 10:
				{
					print_line();
					int xp = input<int>("xp gain up count : ");

					entity_manager.at(id)->gain_xp(xp);
					entity_manager.at(id)->level_up();
					break;
				}

				case 11:
				{
					print_line();

					entity_manager.at(id)->display_info();
					entity_manager.at(id)->display_stats();

					std::string name, background, class_type;
					Attributes stats;
					int level, xp, max_hp, current_hp, max_mana, current_mana, armorclass, gold, current_location_id;
					bool is_player, is_dead;

					print("please enter the updated details\n");

					name = input<std::string>("name : ");
					class_type = input<std::string>("class type : ");
					background = input<std::string>("background : ");
					is_player = input<bool>("is player (1 for yes, 0 for no) : ");
					is_dead = input<bool>("is dead (1 for yes, 0 for no) : ");
					stats.cha_ = input<int>("charisma points (1 - 20) : ");
					stats.cons_ = input<int>("constitution points (1 - 20) : ");
					stats.dex_ = input<int>("dextirity points (1 - 20) : ");
					stats.int_ = input<int>("inteliigence points (1 - 20) : ");
					stats.str_ = input<int>("strength points (1 - 20) : ");
					stats.wis_ = input<int>("wisdom points (1 - 20) : ");
					level = input<int>("level : ");
					xp = input<int>("xp : ");
					max_hp = input<int>("max_hp : ");
					current_hp = input<int>("current_hp : ");
					max_mana = input<int>("max mana : ");
					current_mana = input<int>("current mana : ");
					armorclass = input<int>("armorclass : ");
					gold = input<int>("gold : ");
					if (!location_manager_lambda.get_location_manager().empty())
						current_location_id = ask_id(location_manager_lambda.get_location_manager(), false, lambda->display_all, "current location id : ");
					else
						current_location_id = 0;

					entity_manager.at(id)->set_name(name);
					entity_manager.at(id)->set_background(background);
					entity_manager.at(id)->set_class_type(class_type);
					entity_manager.at(id)->set_is_player(is_player);
					entity_manager.at(id)->set_is_dead(is_dead);
					entity_manager.at(id)->set_stats(stats);
					entity_manager.at(id)->set_level(level);
					entity_manager.at(id)->set_xp(xp);
					entity_manager.at(id)->set_max_hp(max_hp);
					entity_manager.at(id)->set_current_hp(current_hp);
					entity_manager.at(id)->set_max_mana(max_mana);
					entity_manager.at(id)->set_current_mana(current_mana);
					entity_manager.at(id)->set_armorclass(armorclass);
					entity_manager.at(id)->set_gold(gold);
					entity_manager.at(id)->set_current_location_id(current_location_id);
					entity_manager.at(id)->set_is_player(is_player);
					entity_manager.at(id)->set_is_dead(is_dead);

					print("done\n");

					entity_manager.at(id)->display_info();

					data_base_lambda.update_entity(*entity_manager.at(id), id);
					break;
				}

				case 12:
					print("going back\n");
					no_exit = false;
					break;

				default:
					print("invalid choice enterd\n");
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
		print("entity container is empty\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

Entity_Manager::Entity_Manager() : lambda(std::make_unique<Entity_Manager_Lambda>())
{
	set_get_entity_manager();
	set_load_entity_manager();
	set_load_inventories();
	set_link_equipment();
	set_load_comeplted_and_ongoing_quests();
	set_clear_entity_manager();
	set_display_all();
	set_display_player();
	set_display_none_player();
	set_get_entity();
	set_dirty_entity_checkpoint();
}