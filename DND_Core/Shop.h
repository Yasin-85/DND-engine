#pragma once

#include <memory>
#include <array>
#include <unordered_map>

#include <Data_Structures.h> //std::array being a bitch

class Item;

struct Owned_Items;
struct World_Inventory_Lambda;

class Shop
{
private:
	std::array<std::pair<int, Owned_Items>, 5> shop_inventory;
	std::unordered_map<int, std::weak_ptr<Item>> world_inventory_backup;

public:
	//SHOP GETTERS AND SETTERS
	const std::array<std::pair<int, Owned_Items>, 5>& get_shop_inventory() const;
	const std::unordered_map<int, std::weak_ptr<Item>>& get_world_inventory_backup() const;

	void set_shop_inventory(std::array<std::pair<int, Owned_Items>, 5>& new_shop_inventory);
	void set_world_inventory_backup(std::unordered_map<int, std::weak_ptr<Item>>& new_world_inventory_backup);

	//FUNCTIONS
	void reset_shop_inventory();

	void actual_shopping();

	Shop(World_Inventory_Lambda& world_inventory_lambda);
};
