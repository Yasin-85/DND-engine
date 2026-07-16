#pragma once

#include <unordered_map>
#include <memory>
#include <functional>

class Location;

struct Location_Manager_Lambda;
struct Rewards_Manager_lambda;
struct Data_Base_Lambda;

class Location_Manager
{
private:
	std::unordered_map<int, std::shared_ptr<Location>> location_manager;

public:
	std::unique_ptr<Location_Manager_Lambda> lambda;

	void set_get_location_manager();

	void set_get_location();
	
	void set_load_location_manager();
	
	void set_load_connected_locations();
	
	void set_load_location_chest();
	
	void set_clear_location_manager();

	void add_location(const Data_Base_Lambda& data_base_lambda);

	bool remove_location(const Data_Base_Lambda& data_base_lambda);
	
	void set_display_all();

	void connect_2_locations(const Data_Base_Lambda& data_base_lambda);

	void disconnect_2_locations(const Data_Base_Lambda& data_base_lambda);

	void update_location(const Rewards_Manager_Lambda& rewards_manager_lambda, const Data_Base_Lambda& data_base_lambda);

	//CONSTRUCTOR
	Location_Manager();
};
