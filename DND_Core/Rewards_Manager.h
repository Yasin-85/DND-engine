#pragma once

#include <unordered_map>
#include <memory>

class Rewards;

struct Rewards_Manager_Lambda;
struct World_Inventory_Lambda;
struct Data_Base_Lambda;

class Rewards_Manager
{
private:
	std::unordered_map<int, std::shared_ptr<Rewards>> rewards_manager;

public:
	std::unique_ptr<Rewards_Manager_Lambda> lambda;

	void set_get_rewards_manager();
	
	void set_load_rewards_manager();
	
	void set_load_reward_items();
	
	void set_clear_rewards_manager();

	void add_reward(const World_Inventory_Lambda& world_inventory_lambda, const Data_Base_Lambda& data_base_lambda);

	bool remove_reward(const Data_Base_Lambda& data_base_lambda);
	
	void set_get_reward();
	
	void set_display_all();

	void update_reward(const World_Inventory_Lambda& world_inventory_lambda, const Data_Base_Lambda& data_base_lambda);

	//CONSTRUCTOR
	Rewards_Manager();
};