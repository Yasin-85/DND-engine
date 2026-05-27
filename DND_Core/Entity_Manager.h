#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>

class Entity;

struct Entity_Manager_Lambda;
struct World_Inventory_Lambda;
struct Quest_Manager_Lambda;
struct Location_Manager_Lambda;
struct Data_Base_Lambda;

class Entity_Manager
{
private:
	std::unordered_map<int, std::shared_ptr<Entity>> entity_manager;

public:
	std::unique_ptr<Entity_Manager_Lambda> lambda;

	void set_get_entity_manager();
	
	void set_load_entity_manager();
	
	void set_load_inventories();
	
	void set_link_equipment();
	
	void set_load_comeplted_and_ongoing_quests();
	
	void set_clear_entity_manager();

	void add_entity(const Data_Base_Lambda& data_base_lambda);

	bool remove_entity(const Data_Base_Lambda& data_base_lambda);
	
	void set_display_all();
	
	void set_display_player();
	
	void set_display_none_player();
	
	std::vector<int> find_by_name(bool player_only = false, bool show_id = true);
	
	void set_get_entity();

	void set_dirty_entity_checkpoint();

	void update_entity(const World_Inventory_Lambda& world_inventory_lambda, 
		const Quest_Manager_Lambda& quest_manager_lambda, 
		const Location_Manager_Lambda& location_manager_lambda,
		const Data_Base_Lambda& data_base_lambda);

	//CONSTRUCTOR
	Entity_Manager();
};