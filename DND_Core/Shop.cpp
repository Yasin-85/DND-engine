#include <memory>
#include <unordered_map>

#include "Shop.h"
#include "Data_Structures.h"
#include "Utils.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

const std::array<std::pair<int, Owned_Items>, 5>& Shop::get_shop_inventory() const
{
	return shop_inventory;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

const std::unordered_map<int, std::weak_ptr<Item>>& Shop::get_world_inventory_backup() const
{
	return world_inventory_backup;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Shop::set_shop_inventory(std::array<std::pair<int, Owned_Items>, 5>& new_shop_inventory)
{
	shop_inventory = new_shop_inventory;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Shop::set_world_inventory_backup(std::unordered_map<int, std::weak_ptr<Item>>& new_world_inventory_backup)
{
	world_inventory_backup = new_world_inventory_backup;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Shop::reset_shop_inventory()
{
	if (get_world_inventory_backup().empty())
	{
		print("no items available to stock the shop\n");
		return;
	}
	for (int i = 0; i < 5; i++)
	{
		int random_num = dice_roll(get_world_inventory_backup().size()) - 1;

		auto it = std::next(get_world_inventory_backup().begin(), random_num);

		shop_inventory[i].first = it->first;
		shop_inventory[i].second.item = it->second;
		shop_inventory[i].second.quantity = dice_roll(10);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Shop::actual_shopping(int player_index, Party_Lambda& party_lambda)
{
	while (true)
	{
		try
		{
			print_line(5);

			print("welcome to my shop, my name is gokron what do u wish to do ?\n"
				"1.buy stuff\n"
				"2.sell stuff\n"
				"3.exit\n"
				"(careful, if you sell an item you cant buy it back)\n", 5);

			int choice = input<int>("your choice : ", 5);

			switch (choice)
			{
			case 1:
				break;

			case 2:
				break;

			case 3:
				print("bye bye, come back later, i need your money to live :D\n", 5);
				return;

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

Shop::Shop(World_Inventory_Lambda& world_inventory_lambda)
{
	const auto& world_inv = world_inventory_lambda.get_world_inventory();
	for (const auto& [id, ptr] : world_inv)
	{
		world_inventory_backup[id] = ptr;  // shared_ptr to weak_ptr
	}
}