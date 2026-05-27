#pragma once

#include <vector>
#include <winsqlite/winsqlite3.h>
#include <unordered_map>
#include <functional>

#include "Enums.h"

class Item;
class Entity;
class Rewards;
class Location;
class Quest;

struct Data_Base_Lambda_Lambda;
struct Entity_Manager_Lambda;
struct Location_Manager_Lambda;
struct Quest_Manager_Lambda;
struct Rewards_Manager_Lambda;
struct World_Inventory_Lambda;

class Data_Base
{
private:
	sqlite3* db;
	std::unordered_map<int, std::pair<int, int>> temp_equipment_map;

public:
	std::unique_ptr<Data_Base_Lambda> lambda;

	//STARTUP
	void startup();

	//LOADING FROM TABLES INTO MANAGERS
	void load_EM(const Entity_Manager_Lambda& EM_lambda);

	void load_LM(const Location_Manager_Lambda& LM_lambda, const Rewards_Manager_Lambda& RM_lambda);

	void load_QM(const Quest_Manager_Lambda& QM_lambda, const Rewards_Manager_Lambda& RM_lambda);

	void load_RM(const Rewards_Manager_Lambda& RM_lambda);

	void load_WI(const World_Inventory_Lambda& WI_lambda);

	void load_connected_locations(const Location_Manager_Lambda& LM_lambda);

	void load_inventories(const Entity_Manager_Lambda& EM_lambda, const World_Inventory_Lambda& WI_lambda);

	void link_entity_equipment(const Entity_Manager_Lambda& EM_lambda, const World_Inventory_Lambda& WI_lambda);

	void load_completed_and_ongoing_quests(const Entity_Manager_Lambda& EM_lambda);

	void load_reward_items(const Rewards_Manager_Lambda& RM_lambda, const World_Inventory_Lambda& WI_lambda);

	void load_quest_enemies(const Quest_Manager_Lambda& QM_lambda, const Entity_Manager_Lambda& EM_lambda);

	void load_all(const Entity_Manager_Lambda& EM_lambda,
		const Location_Manager_Lambda& LM_lambda,
		const Quest_Manager_Lambda& QM_lambda,
		const Rewards_Manager_Lambda& RM_lambda,
		const World_Inventory_Lambda& WI_lambda);

	void clear_all_containers(const Entity_Manager_Lambda& EM_lambda,
		const Location_Manager_Lambda& LM_lambda,
		const Quest_Manager_Lambda& QM_lambda,
		const Rewards_Manager_Lambda& RM_lambda,
		const World_Inventory_Lambda& WI_lambda);

	void reload(const Entity_Manager_Lambda& EM_lambda,
		const Location_Manager_Lambda& LM_lambda,
		const Quest_Manager_Lambda& QM_lambda,
		const Rewards_Manager_Lambda& RM_lambda,
		const World_Inventory_Lambda& WI_lambda);

	//SET INSERTING INTO TABLES LAMBDA FUNCTIONS
	void set_insert_item();

	void set_insert_entity();

	void set_insert_reward();

	void set_insert_location();
	
	void set_insert_quest();

	void set_insert_connected_locations();

	void set_insert_inventories();

	void set_insert_completed_and_ongoing_quest();

	void set_insert_reward_items();

	void set_insert_quest_enemies();

	void set_insert_quest_reward();

	void set_insert_location_reward();

	//SET DELETING FROM TABLES LAMBDA FUNCTIONS
	void set_delete_from_main_container();

	void set_delete_from_sub_container();

	//SET UPDATING TABLES LAMBDA FUNCTIONS
	void set_update_entity_location();

	void set_update_entity_equipped_item();

	void set_update_sub_container_quantity();

	void set_update_properties();

	void set_update_item();

	void set_update_entity();

	void set_update_reward();

	void set_update_location();

	void set_update_quest();

	//CONSTRUCTOR
	Data_Base();

	//DECONSTRUCTOR
	~Data_Base();
};