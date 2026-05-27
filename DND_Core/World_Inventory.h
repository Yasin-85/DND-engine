#pragma once

#include <memory>
#include <unordered_map>

class Item;

struct World_Inventory_Lambda;
struct Data_Base_Lambda;

class World_Inventory
{
private:
	std::unordered_map<int, std::shared_ptr<Item>> world_inventory;

public:
	std::unique_ptr<World_Inventory_Lambda> lambda;

	void set_get_world_inventory();

	void set_load_world_inventory();

	void set_clear_world_inventory();

	void add_item(const Data_Base_Lambda& data_base_lambda);

	bool remove_item(const Data_Base_Lambda& data_base_lambda);

	void set_display_all();

	void set_display_by_class_child_type();

	void set_display_by_name();

	void set_get_item();

	void update_item(const Data_Base_Lambda& data_base_lambda);

	//CONSTRUCTOR
	World_Inventory();
};
