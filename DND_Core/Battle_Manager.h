#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "Enums.h"

class Quest;
class Entity;
class Battle_Entity;

struct Party_Lambda;
struct Enemies;

class Battle_Manager
{
private:
	std::unordered_map<int, std::unique_ptr<Battle_Entity>> battle_entities;
	std::vector<std::pair<int, int>> turn_ids; // first int is initiative roll, second int is the battle entity's id
	std::weak_ptr<Quest> active_quest;
	Battle_Type battle_type;
	int entity_count{ 1 }, x_max, y_max;

public:
	//BATTLE MANAGER GETTERS AND SETTERS
	const std::unordered_map<int, std::unique_ptr<Battle_Entity>>& get_battle_entities() const;
	const std::vector<std::pair<int, int>>& get_turn_ids() const;
	const std::weak_ptr<Quest> get_active_quest() const;
	const Battle_Type get_battle_type() const;
	const int get_entity_count() const;
	const int get_x_max() const;
	const int get_y_max() const;

	void set_battle_entities(std::unordered_map<int, std::unique_ptr<Battle_Entity>>& new_battle_entities);
	void set_turn_ids(std::vector<std::pair<int, int>>& new_turn_ids);
	void set_active_quest(std::weak_ptr<Quest> new_active_quest);
	void set_battle_type(Battle_Type new_battle_type);
	void set_entity_count(int new_entity_count);
	void set_x_max(int new_x_max);
	void set_y_max(int new_y_max);

	//FUNCTIONS
	void calculate_x_y_borders();

	void roll_for_initiative();

	bool is_battle_over();

	bool is_position_occupied(int x, int y, int &id);

	bool is_position_occupied(int x, int y);

	void print_battle_field();

	void move_entity_position(int x, int y, int id);

	void entity_random_placement();

	void player_turn(int id);

	void enemy_turn(int id);

	void main_battle();

	void battle_end();

	//CONSTRUCTOR
	Battle_Manager(Battle_Type new_battle_type, const std::vector<std::pair<int, std::weak_ptr<Entity>>>& players,
		std::weak_ptr<Quest> active_quest, std::vector<std::pair<int,Enemies>> enemies);
};
