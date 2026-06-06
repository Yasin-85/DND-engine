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

void Shop::display_shop_inventory()
{
	for (int i = 0; i < 5; i++)
	{
		if (auto p = shop_inventory[i].second.item.lock())
		{
			print(std::to_string(i + 1) + " ", 5);
			p->display_details();
			p->display_properties();
			print("amount : " + std::to_string(shop_inventory[i].second.quantity) + '\n', 5);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Shop::set_actual_shopping()
{
	lambda->actual_shopping = [this](int player_index, const Party_Lambda& party_lambda)
		{
			while (true)
			{
				try
				{
					print_line(5);

					print("welcome to my shop, my name is gokron what do u wish to do ?\n"
						"1.buy stuff\n"
						"2.sell stuff\n"
						"3.reset the shop, 10 gold cost\n"
						"4.exit\n"
						"(careful, if you sell an item you cant buy it back)\n", 5);

					int choice = input<int>("your choice : ", 5);

					switch (choice)
					{
					case 1:
					{
						print_line(5);

						party_lambda.display_party_member_inventory_and_details(player_index);

						this->display_shop_inventory();

						int item_index = input<int>("which item to buy : ", 5);
						int amount = input<int>("amount to buy : ", 5);

						if (!in_range(item_index, 1, 5))
							throw std::invalid_argument("invalid item index entered");

						if (!in_range(amount, 1, this->shop_inventory[item_index].second.quantity))
							throw std::invalid_argument("invalid amount entered");

						this->shop_inventory[item_index - 1].second.quantity -= amount;

						party_lambda.give_item(player_index, item_index, amount, this->shop_inventory[item_index - 1].second.item);

						if (auto p = this->shop_inventory[item_index - 1].second.item.lock())
						{
							party_lambda.give_gold(player_index, p->get_value() * amount);
						}

						print("BOUGHT! BOUGHT! BOUGHT!, enjoy your probably not useless item(s) and i will enjoy this money to pay off rent\n", 5);
						party_lambda.display_party_member_inventory_and_details(player_index);
					}
					break;

					case 2:
					{
						print_line(5);

						party_lambda.display_party_member_inventory_and_details(player_index);
						int item_id = input<int>("items id to sell : ", 5);
						int amount = input<int>("amount to sell : ", 5);

						party_lambda.take_item(player_index, item_id, amount);
						party_lambda.give_gold(player_index, this->get_world_inventory_backup().at(item_id).lock()->get_value() * amount);

						print("SOLD! SOLD! SOLD!, enjoy your money and i will enjoy this/these item(s) that hopefully are not useless\n", 5);
						party_lambda.display_party_member_inventory_and_details(player_index);
					}
					break;

					case 3:
						print_line(5);

						party_lambda.take_gold(player_index, 10);
						this->reset_shop_inventory();

						print("new stocks came in through!\n", 5);
						break;

					case 4:
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
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

Shop::Shop(World_Inventory_Lambda& world_inventory_lambda) : lambda(std::make_unique<Shop_Lambda>())
{
	const auto& world_inv = world_inventory_lambda.get_world_inventory();
	for (const auto& [id, ptr] : world_inv)
	{
		world_inventory_backup[id] = ptr;  // shared_ptr to weak_ptr
	}

	set_actual_shopping();
}