#pragma once
#include <array>
#include <vector>
#include <memory>

#include "Enums.h"

class Entity;
class Rewards;

struct Entity_Manager_Lambda;
struct Data_Base_Lambda;

// PARTY
class Party
{
private:
	std::array<std::pair<int, std::weak_ptr<Entity>>, 4> party;
	std::array<std::pair<int, std::vector<std::weak_ptr<Rewards>>>, 4> reward_buffer;
	Party_State party_state;
	bool single_player;
	int avg_level, party_size;

public:
	//PARTY GETTERS AND SETTERS
	const std::array<std::pair<int, std::weak_ptr<Entity>>, 4>& get_party() const;
	const std::array<std::pair<int, std::vector<std::weak_ptr<Rewards>>>, 4>& get_reward_buffer() const;
	const Party_State& get_party_state() const;
	const bool get_single_player() const;
	const int get_avg_level() const;
	const int get_party_size() const;

	void set_party(std::array<std::pair<int, std::weak_ptr<Entity>>, 4>& new_party);
	void set_reward_buffer(std::array<std::pair<int, std::vector<std::weak_ptr<Rewards>>>, 4>& new_reward_buffer);
	void set_party_state(Party_State new_party_state);
	void set_single_player(bool new_single_player);
	void set_avg_level(int new_avg_level);
	void set_party_size(int new_party_size);

	//FUNCTIONS
	void assign_party_members(Entity_Manager_Lambda& entity_manager_lambda);

	void assign_member_rewards(Data_Base_Lambda& data_base_lambda, Entity_Manager_Lambda& entity_manager_lambda);

	void party_resting(Data_Base_Lambda& data_base_lambda, Entity_Manager_Lambda& entity_manager_lambda);

	void calculate_avg_level();

	void share_gold(int from_index, int to_index, int amount);

	void share_item(int from_index, int to_index, int item_id, int count);

	void display_party_member_details();

	//CONSTRUCTOR
	Party(bool new_single_player, Entity_Manager_Lambda& entity_manager_lambda);

	Party(bool new_single_player, Entity_Manager_Lambda& entity_manager_lambda, int new_party_size);
};
