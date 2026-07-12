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
#include "Party.h"
#include "Data_Base.h"
#include "Shop.h"
#include "Battle_Manager.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_menu(Entity_Manager_Lambda& EM_lambda, std::unique_ptr<Party>& party);
void travel(Location_Manager_Lambda& LM_lambda, std::unique_ptr<Party>& party);

/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
	World_Inventory WI;
	Entity_Manager EM;
	Rewards_Manager RM;
	Location_Manager LM;
	Quest_Manager QM;
	Data_Base DB;
	Shop shop_vendor(*WI.lambda);

	std::unique_ptr<Party> party;

	try
	{
		DB.load_all(*EM.lambda, *LM.lambda, *QM.lambda, *RM.lambda, *WI.lambda);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	while (true)
	{
		try
		{
			print_line(5);
			print("D&D Game Engine\n", 5);
			print_line(5);

			if (main_menu(*EM.lambda, party))
				return EXIT_SUCCESS;

			while (true)
			{
				try
				{
					print("choose your action\n"
						"1. travel to a location\n"
						"2.	display party member details\n"
						"3. rest (whole party together)\n"
						"4. quests tab\n"
						"5. save & quit\n", 5);

					int choice = input<int>("your chioce : ", 5);

					switch (choice)
					{
					case 1:

						break;

					case 2:
						party->lambda->display_party_members_details();
						break;

					case 3:
						party->party_resting(*DB.lambda, *EM.lambda, *shop_vendor.lambda);
						break;

					case 4:
						break;

					case 5:
						print_line(5);

						print("saving...\n", 5);

						EM.lambda->dirty_entity_checkpoint(*DB.lambda);

						print("progress saved! \nexiting to main menu\n", 5);
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
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}

	return EXIT_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_menu(Entity_Manager_Lambda& EM_lambda, std::unique_ptr<Party>& party)
{
	int choice;

	while (true)
	{
		try
		{
			print("1.single player\n"
				"2.multi player\n"
				"3.quit\n", 5);

			choice = input<int>("your choice : ", 5);

			switch (choice)
			{
			case 1:
			{
				party.reset();
				party = std::make_unique<Party>(true, EM_lambda);
				party->lambda->display_party_members_details();
				return false;
				break;
			}

			case 2:
			{
				party.reset();
				int party_size;

				while (true)
				{
					party_size = input<int>("please enter the player count (2 - 4) : ");

					if (in_range(party_size, 2, 4))
						break;
					else
						print("invalid player count entered\n");
				}
				party = std::make_unique<Party>(false, EM_lambda, party_size);
				party->lambda->display_party_members_details();
				return false;
				break;
			}

			case 3:
				print("exiting game\n", 5);
				print("thanks for playing\n", 5);
				return true;
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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void travel(Location_Manager_Lambda& LM_lambda, std::unique_ptr<Party>& party)
{
	print_line(5);

	for (int i = 0; i < party->get_party_size(); i++)
	{
		if (auto p = party->get_party()[i].second.lock())
		{
			int current_location_id = p->get_current_locaion_id();
		}
	}
}