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

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_menu(World_Inventory& WI,
	Entity_Manager& EM,
	Rewards_Manager& RM,
	Location_Manager& LM,
	Quest_Manager& QM,
	Data_Base& DB,
	std::shared_ptr<Party> party);

/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
	World_Inventory WI;
	Entity_Manager EM;
	Rewards_Manager RM;
	Location_Manager LM;
	Quest_Manager QM;
	Data_Base DB;

	std::shared_ptr<Party> party;

	try
	{
		DB.load_all(*EM.lambda, *LM.lambda, *QM.lambda, *RM.lambda, *WI.lambda);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	print_line(5);
	print("D&D Game Engine\n", 5);
	print_line(5);

	if (main_menu(WI, EM, RM, LM, QM, DB, party))
		return EXIT_SUCCESS;

	return EXIT_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_menu(World_Inventory& WI,
	Entity_Manager& EM,
	Rewards_Manager& RM,
	Location_Manager& LM,
	Quest_Manager& QM,
	Data_Base& DB,
	std::shared_ptr<Party> party)
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
				party = std::make_shared<Party>(true, *EM.lambda);
				party->display_party_member_details();
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
				party = std::make_shared<Party>(false, *EM.lambda, party_size);
				party->display_party_member_details();
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