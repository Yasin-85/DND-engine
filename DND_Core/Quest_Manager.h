#pragma once

#include <unordered_map>
#include <memory>

class Quest;

struct Quest_Manager_Lambda;
struct Rewards_Manager_Lambda;
struct Location_Manager_Lambda;
struct Entity_Manager_Lambda;
struct Data_Base_Lambda;

class Quest_Manager
{
private :
	std::unordered_map<int, std::shared_ptr<Quest>> quest_manager;

public :
	std::unique_ptr<Quest_Manager_Lambda> lambda;

	void set_get_quest_manager();
	
	void set_load_quest_manager();
	
	void set_load_quest_reward();
	
	void set_load_quest_enemies();
	
	void set_clear_quest_manager();

	void add_quest(const Data_Base_Lambda& data_base_lambda);

	bool remove_quest(const Data_Base_Lambda& data_base_lambda);
	
	void set_display_all();
	
	void set_display_by_priority();
	
	void set_display_by_quest_level();
	
	void set_get_quest();

	void update_quest(const Entity_Manager_Lambda& entity_manager_lambda, const Rewards_Manager_Lambda& rewards_manager_lambda, const Data_Base_Lambda& data_base_lambda);

	//CONSTRUCTOR
	Quest_Manager();
};
