#include <iostream>
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <winsqlite/winsqlite3.h>

#include "Utils.h"
#include "Data_Structures.h"
#include "World_Inventory.h"
#include "Entity_Manager.h"
#include "Location_Manager.h"
#include "Rewards_Manager.h"
#include "Quest_Manager.h"
#include "Data_Base.h"

int main(void)
{
	bool no_exit{ true }, inner_no_exit{ true };
	int choice;

	World_Inventory WI;
	Entity_Manager EM;
	Rewards_Manager RM;
	Location_Manager LM;
	Quest_Manager QM;
	Data_Base DB;

	try
	{
		print("loading data base\n");
		DB.load_all(*EM.lambda, *LM.lambda, *QM.lambda, *RM.lambda, *WI.lambda);
		print("loading complete\n");
	}
	catch (const std::exception& e)
	{
		print("loading failed\n");
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	print_line();
	print("D&D ADMIN PANEL\n");

	while (no_exit)
	{
		print_line();

		print("1.access World Inventory\n"
			"2.access Entity Manager\n"
			"3.access reward manager\n"
			"4.access Location Manager\n"
			"5.access quest manager\n"
			"6.exit program\n");

		choice = input<int>("your choice : ");

		switch (choice)
		{
			/////////////////////////////////////////////////////////////////////////////////////////////////////

		case 1:
			//WORLD INVENTORY
			inner_no_exit = true;
			while (inner_no_exit)
			{
				try
				{
					print_line();

					print("1.add an item\n"
						"2.remove an item\n"
						"3.display all\n"
						"4.display by type (weapon, armor, spell_scroll, consumable)\n"
						"5.find by name\n"
						"6.update an item (details)\n"
						"7.go back\n");

					choice = input<int>("your choice : ");

					switch (choice)
					{
					case 1:
						WI.add_item(*DB.lambda);
						break;

					case 2:
						if (WI.remove_item(*DB.lambda)) DB.reload(*EM.lambda, *LM.lambda, *QM.lambda, *RM.lambda, *WI.lambda);
						break;

					case 3:
						WI.lambda->display_all();
						break;

					case 4:
						WI.lambda->display_by_class_child_type();
						break;

					case 5:
						WI.lambda->display_by_name();
						break;

					case 6:
						WI.update_item(*DB.lambda);
						break;

					case 7:
						print("going back\n");
						inner_no_exit = false;
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

			break;

			/////////////////////////////////////////////////////////////////////////////////////////////////////

		case 2:
			//ENTITY MANAGER
			inner_no_exit = true;
			while (inner_no_exit)
			{
				try
				{
					print_line();

					print("1.add an entity\n"
						"2.remove an entity\n"
						"3.display all\n"
						"4.display players\n"
						"5.display none players\n"
						"6.find entity by name\n"
						"7.update an entity (display, inventory, xp gain)\n"
						"8.go back\n");

					choice = input<int>("your choice : ");

					switch (choice)
					{
					case 1:
						EM.add_entity(*DB.lambda);
						break;

					case 2:
						if (EM.remove_entity(*DB.lambda)) DB.reload(*EM.lambda, *LM.lambda, *QM.lambda, *RM.lambda, *WI.lambda);
						break;

					case 3:
						EM.lambda->display_all();
						break;

					case 4:
						EM.lambda->display_player();
						break;

					case 5:
						EM.lambda->display_none_player();
						break;

					case 6:
						EM.find_by_name();
						break;

					case 7:
						EM.update_entity(*WI.lambda, *QM.lambda, *LM.lambda, *DB.lambda);
						EM.lambda->dirty_entity_checkpoint(*DB.lambda);
						break;

					case 8:
						print("going back\n");
						inner_no_exit = false;
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

			break;

			/////////////////////////////////////////////////////////////////////////////////////////////////////

		case 3:
			//REWARD MANAGER
			inner_no_exit = true;
			while (inner_no_exit)
			{
				try
				{
					print_line();

					print("1.add a reward\n"
						"2.remove a reward\n"
						"3.display all rewards\n"
						"4.update a reward (item, details)\n"
						"5.go back\n");

					choice = input<int>("your choice : ");

					switch (choice)
					{
					case 1:
						RM.add_reward(*WI.lambda, *DB.lambda);
						break;

					case 2:
						if (RM.remove_reward(*DB.lambda)) DB.reload(*EM.lambda, *LM.lambda, *QM.lambda, *RM.lambda, *WI.lambda);
						break;

					case 3:
						RM.lambda->display_all();
						break;

					case 4:
						RM.update_reward(*WI.lambda, *DB.lambda);
						break;

					case 5:
						print("going back\n");
						inner_no_exit = false;
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

			break;

			/////////////////////////////////////////////////////////////////////////////////////////////////////

		case 4:
			//LOCATION MANAGER
			inner_no_exit = true;
			while (inner_no_exit)
			{
				try
				{
					print_line();

					print("1.add a location\n"
						"2.remove a location\n"
						"3.display all locations\n"
						"4.connect two locations\n"
						"5.disconnect two locations\n"
						"6.update a location (reward, details)\n"
						"7.go back\n");

					choice = input<int>("your choice : ");

					switch (choice)
					{
					case 1:
						LM.add_location(*DB.lambda);
						break;

					case 2:
						if (LM.remove_location(*DB.lambda)) DB.reload(*EM.lambda, *LM.lambda, *QM.lambda, *RM.lambda, *WI.lambda);
						break;

					case 3:
						LM.lambda->display_all();
						break;

					case 4:
						LM.connect_2_locations(*DB.lambda);
						break;

					case 5:
						LM.disconnect_2_locations(*DB.lambda);
						break;

					case 6:
						LM.update_location(*RM.lambda, *DB.lambda);
						break;

					case 7:
						print("going back\n");
						inner_no_exit = false;
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

			break;

			/////////////////////////////////////////////////////////////////////////////////////////////////////

		case 5:
			//QUEST MANAGER
			inner_no_exit = true;
			while (inner_no_exit)
			{
				try
				{
					print_line();

					print("1.add a quest\n"
						"2.remove a quest\n"
						"3.display all quests\n"
						"4.display by quest priority\n"
						"5.display by quest level\n"
						"6.update a quest (reward, enemy, details)\n"
						"7.go back\n");

					choice = input<int>("your choice : ");

					switch (choice)
					{
					case 1:
						QM.add_quest(*DB.lambda);
						break;

					case 2:
						if (QM.remove_quest(*DB.lambda)) DB.reload(*EM.lambda, *LM.lambda, *QM.lambda, *RM.lambda, *WI.lambda);
						break;

					case 3:
						QM.lambda->display_all();
						break;

					case 4:
						QM.lambda->display_by_priority();
						break;

					case 5:
						QM.lambda->display_by_quest_level();
						break;

					case 6:
						QM.update_quest(*EM.lambda, *RM.lambda, *DB.lambda);
						break;

					case 7:
						print("going back\n");
						inner_no_exit = false;
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

			break;

		case 6:
			print("exiting program\n");
			no_exit = false;
			break;

		default:
			print("invalid choice entered\n");
			break;
		}
	}
	return EXIT_SUCCESS;
}