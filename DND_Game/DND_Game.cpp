#include <iostream>
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <winsqlite/winsqlite3.h>
#include <algorithm>

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
#include "Enums.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool main_menu(Entity_Manager_Lambda& EM_lambda, std::unique_ptr<Party>& party);
void travel(Location_Manager_Lambda& LM_lambda, std::unique_ptr<Party>& party);
void quest_tracker(Quest_Manager_Lambda& QM_lambda, std::unique_ptr<Party>& party);
void quest_tackle(Quest_Manager_Lambda& QM_lambda, std::unique_ptr<Party>& party);

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

			if (main_menu(*EM.lambda, party))
				return EXIT_SUCCESS;

			while (true)
			{
				try
				{
					print_line(5);

					print("choose your action\n"
						"1. travel to a location\n"
						"2.	display party member details\n"
						"3. rest (whole party must be together)\n"
						"4. quests tab\n"
						"5. tackle a quest\n"
						"6. save & quit\n", 5);

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
						quest_tracker(*QM.lambda, party);
						break;

					case 5:
						quest_tackle(*QM.lambda, party);
						break;

					case 6:
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
			print_line(5);

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
					party_size = input<int>("please enter the player count (2 - 4) : ", 5);

					if (in_range(party_size, 2, 4))
						break;

					else
						print("invalid player count entered\n", 5);
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

	int party_size = party->get_party_size();
	std::array<int, 4> current_location_id;

	for (int i = 0; i < party_size; i++)
	{
		if (auto p = party->get_party()[i].second.lock())
		{
			current_location_id[i] = p->get_current_locaion_id();
		}
	}

	int choice;

	while (true)
	{
		print_line(5);

		print("choose which character wants to travel (1 - " + std::to_string(party_size) + "), -1 to show all characters current location, -2 to go back\n", 5);

		choice = input<int>("your choice : ");

		switch (choice)
		{
		case 1:
		case 2:
		case 3:
		case 4:

			if (choice > party_size)
			{
				print("invalid party member selected, must be in range of (1 - " + std::to_string(party_size) + ")\n", 5);
				break;
			}

			if (auto o = LM_lambda.get_location(current_location_id[choice]).lock())
			{
				std::vector<int> connected_location_ids = o->get_connected_location_ids();

				for (auto& v : connected_location_ids)
				{
					if (auto p = LM_lambda.get_location(v).lock())
					{
						print(std::to_string(v) + ": " + p->get_name() + ", ", 5);
					}
				}

				while (true)
				{
					int location_id_choice = input<int>("choose where you want to go (type in their id):", 5);

					bool found{ false };
					for (auto& v : connected_location_ids)
					{
						if (v == location_id_choice)
						{
							found = true;
							break;
						}
					}

					if (found)
					{
						if (auto p = party->get_party()[choice].second.lock())
						{
							p->set_current_location_id(location_id_choice);
							print("character moved to " + o->get_name() + "\n", 5);
						}
					}
					else
					{
						print("invalid location id\n", 5);
					}
				}
			}
			break;

		case -1:
			for (int i = 0; i < party_size; i++)
			{
				if (auto p = party->get_party()[i].second.lock())
				{
					if (auto o = LM_lambda.get_location(current_location_id[i]).lock())
					{
						print(p->get_name() + ", current location : " + o->get_name() + '\n', 5);
						o->display_info();
						print("\n", 5);
					}
				}
			}
			break;

		case -2:
			print("going back\n", 5);
			return;

		default:
			print("invalid choice entered\n", 5);
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void quest_tracker(Quest_Manager_Lambda& QM_lambda, std::unique_ptr<Party>& party)
{
	const auto& quests_copy = QM_lambda.get_quest_manager();
	std::unordered_map<int, std::weak_ptr<Quest>> untouched_quests;

	for (const auto& v : quests_copy)
	{
		untouched_quests[v.first] = v.second;
	}

	int party_size = party->get_party_size();
	int choice{ 0 };

	while (true)
	{
	character_selection:

		print_line(5);

		print("choose which characters's quest tab you want to open (1 - " + std::to_string(party_size) + "), -1 to show all characters details, -2 to go back\n", 5);

		choice = input<int>("your choice : ", 5);

		switch (choice)
		{
		case 1:
		case 2:
		case 3:
		case 4:

			if (choice > party_size)
			{
				print("invalid party member selected, must be in range of (1 - " + std::to_string(party_size) + ")\n", 5);
				break;
			}

			if (auto p = party->get_party()[choice - 1].second.lock())
			{
				const std::vector<int>& ongoing_quests_id_copy = p->get_ongoing_quests();
				const std::vector<int>& compeleted_quests_id_copy = p->get_completed_quests();

				std::unordered_map<int, std::weak_ptr<Quest>> ongoing_quests_copy;
				std::unordered_map<int, std::weak_ptr<Quest>> completed_quests_copy;

				for (const auto& v : ongoing_quests_id_copy)
				{
					ongoing_quests_copy[v] = quests_copy.at(v);
					untouched_quests.erase(v);
				}

				for (const auto& v : compeleted_quests_id_copy)
				{
					completed_quests_copy[v] = quests_copy.at(v);
					untouched_quests.erase(v);
				}

				while (true)
				{


					print("1. display the current ongoing quests\n"
						"2. display finished quests\n"
						"3. display availabe quests\n"
						"4. accept a new quest\n"
						"5. go back", 5);

					int inner_choice = input<int>("your choice : ");

					switch (inner_choice)
					{
					case 1:
						print_line();

						if (!ongoing_quests_copy.empty())
						{
							for (const auto& v : ongoing_quests_copy)
							{
								print(std::to_string(v.first) + " ", 5);

								if (auto o = v.second.lock())
									o->display_info_without_enemies();
							}
						}
						else
							print("no ongoing quests\n");

						break;

					case 2:
						print_line(5);

						if (!completed_quests_copy.empty())
						{
							for (const auto& v : completed_quests_copy)
							{
								print(std::to_string(v.first) + " ", 5);

								if (auto o = v.second.lock())
									o->display_info_without_enemies();
							}
						}
						else
							print("you havnt completed any quest yet\n");

						break;

					case 3:
						print_line(5);

						if (!untouched_quests.empty())
						{
							for (const auto& v : untouched_quests)
							{
								print(std::to_string(v.first) + " ", 5);

								if (auto o = v.second.lock())
									o->display_info_without_enemies();
							}
						}
						else
							print("quest container is empty\n");

						break;

					case 4:
						print_line(5);

						print("which quest do u wish to accept\n", 5);
						int quest_id = ask_id(untouched_quests, false,
							[&]{
								for (auto& v : untouched_quests)
								{
									print(std::to_string(v.first) + " ", 5);

									if (auto o = v.second.lock())
										o->display_info_without_enemies();
								}
							});
					
						p->add_ongoing_quest(quest_id);
						print("quest added\n", 5);
						break;

					case 5:
						print("going back\n", 5);
						goto character_selection;

					default:
						print("invalid choice entered\n", 5);
						break;
					}
				}
			}

			break;

		case -1:
			print_line(5);

			for (int i = 0; i < party_size; i++)
			{
				if (auto p = party->get_party()[i].second.lock())
					p->display_info();
			}
			break;

		case -2:
			print("going back\n", 5);
			return;

		default:
			print("invalid choice entered\n", 5);
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void quest_tackle(Quest_Manager_Lambda& QM_lambda, std::unique_ptr<Party>& party)
{
	print_line(5);

	print("choose which quest to progress\n", 5);

	int quest_count{ 0 };
	std::vector<std::pair<int, std::shared_ptr<Quest>>> eligable_quests;
	std::vector<int> eligable_quests_id;
	//quests will be loaded in order of their findings and printing of quest_counter
	//and in the vector the first int is the index of the party member that the quest belongs to

	for (int i = 0; i < party->get_party_size(); i++) //cycle through all party members
	{
		if (auto p = party->get_party()[i].second.lock()) //get each party member and lock check it into shared ptr
		{
			print(p->get_name() + " : \n", 5);
			for (const auto& v : p->get_ongoing_quests()) //cycle through the quests of the selected party member
			{
				if (const auto& q = QM_lambda.get_quest(v).lock()) //get each quest from QM and lock check it into shared ptr
				{
					if (q->get_location_id() == p->get_current_locaion_id()) //check to see if party member is in the quest location
					{
						eligable_quests.push_back({ i, q }); //store the quest in a vector
						eligable_quests_id.push_back(v); //store the quest's id in a vector
						quest_count++;

						print(std::to_string(quest_count) + " : " + q->get_name() + "\n    " + q->get_description() + "\n", 5);
					}
				}
			}
			print_line(5);
		}
	}

	while (true)
	{
		if (quest_count == 0) //no eligable quest
		{
			print("no quest in the current location of party members\n", 5);
			return;
		}

		int choice = input<int>("which quest do u wish to progess (enter the quest index, -1 to go back) : ", 5);

		if (choice == -1) //leaving function
			return;

		if (choice > quest_count || choice < 1) 
			print("invalid index entered\n");

		else
		{
			std::vector<int> quest_owners_index;
			std::vector<std::pair<int, std::weak_ptr<Entity>>> participating_players;

			for (const auto& v : eligable_quests) //storing everyone who owns the selcted quest for multi progression
			{
				if (v.second == eligable_quests.at(choice - 1).second)
				{
					quest_owners_index.push_back(v.first);
				}
			}

			//printing the selected quest's info
			print("chosen quest no" + std::to_string(choice) + " : " + eligable_quests.at(choice - 1).second->get_name() + '\n', 5);

			print("quest owner/owners : \n", 5);

			for (const auto& v : quest_owners_index)
			{
				if (auto p = party->get_party()[v].second.lock())
				{
					participating_players.push_back({ party->get_party()[v].first, party->get_party()[v].second}); //pushing in the quest owners first
					print("    " + p->get_name() + '\n', 5);
				}
			}

			print("these party members will participate as well due to being in the same location : \n", 5);

			for (int i = 0; i < party->get_party_size(); i++) //checking for party members in the quest area
			{
				auto it = std::find(quest_owners_index.begin(), quest_owners_index.end(), i); //making sure duplicate party members are not printed

				if (it != quest_owners_index.end())
					continue;

				if (auto p = party->get_party()[i].second.lock())
				{
					if (p->get_current_locaion_id() == eligable_quests.at(choice - 1).second->get_location_id())
					{
						//pushing in the rest of the party members participating
						participating_players.push_back({ party->get_party()[i].first, party->get_party()[i].second });

						print(p->get_name() + '\n', 5);
					}
				}
			}

			while (true)
			{
				char choice2 = input<char>("are you sure you want to proceed ? Y/N : ", 5);
				choice2 = to_upper(choice2);

				if (choice2 == 'Y')
				{
					Battle_Manager battle(Battle_Type::Quest, participating_players,
						eligable_quests.at(choice - 1).second, {});

					if (battle.main_battle(*party->lambda))
					{
						print("quest progressed\n", 5);
						
						for (const auto& v : quest_owners_index) //quest tracking and progression handeling
						{
							if (auto p = party->get_party()[v].second.lock())
							{
								p->remove_ongoing_quest(eligable_quests_id.at(choice - 1));
								p->add_completed_quest(eligable_quests_id.at(choice - 1));
							}
						}
					}
					return;
				}
				else if (choice2 == 'N')
				{
					print("going back\n", 5);
					break;
				}
				else
				{
					print("invalid choice entered\n", 5);
				}
			}
		}
	}
}