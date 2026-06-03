#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include "Enums.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

class Item;
class Entity;
class Rewards;
class Location;
class Quest;


/////////////////////////////////////////////////////////////////////////////////////////////////////

//ATTRIBUTES
struct Attributes
{
	int str_, dex_, cons_, int_, wis_, cha_;
};

//OWNED ITEMS
struct Owned_Items
{
	std::weak_ptr<Item> item;
	int quantity{ 1 };
};

//ENEMIES
struct Enemies
{
	std::weak_ptr<Entity> enemy_ptr;
	int quantity;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

struct World_Inventory_Lambda;
struct Rewards_Manager_Lambda;
struct Quest_Manager_Lambda;
struct Location_Manager_Lambda;
struct Entity_Manager_Lambda;
struct Data_Base_Lambda;
struct Party_Lambda;
struct Shop_Lambda;

/////////////////////////////////////////////////////////////////////////////////////////////////////

struct World_Inventory_Lambda
{
	std::function<const std::unordered_map<int, std::shared_ptr<Item>>& ()> get_world_inventory;
	std::function<void(int, std::shared_ptr<Item>)> load_world_inventory;
	std::function<void()> clear_world_inventory;
	std::function<void()> display_all;
	std::function<void()> display_by_class_child_type;
	std::function<void()> display_by_name;
	std::function<std::weak_ptr<Item>(int)> get_item;
};

struct Rewards_Manager_Lambda
{
	std::function<const std::unordered_map<int, std::shared_ptr<Rewards>>& ()> get_rewards_manager;
	std::function<void(int, std::shared_ptr<Rewards>)> load_rewards_manager;
	std::function<void(int, int, const Owned_Items&)> load_reward_items;
	std::function<void()> clear_rewards_manager;
	std::function<std::weak_ptr<Rewards>(int)> get_reward;
	std::function<void()> display_all;
};

struct Quest_Manager_Lambda
{
	std::function<const std::unordered_map<int, std::shared_ptr<Quest>>& ()> get_quest_manager;
	std::function<void(int, std::shared_ptr<Quest>)> load_quest_manager;
	std::function<void(int, int, const Rewards_Manager_Lambda&)> load_quest_reward;
	std::function<void(int, int, const Enemies&)> load_quest_enemies;
	std::function<void()> clear_quest_manager;
	std::function<void()> display_all;
	std::function<void()> display_by_priority;
	std::function<void()> display_by_quest_level;
	std::function<std::weak_ptr<Quest>(int)> get_quest;
};

struct Location_Manager_Lambda
{
	std::function<const std::unordered_map<int, std::shared_ptr<Location>>& ()> get_location_manager;
	std::function<void(int, std::shared_ptr<Location>)> load_location_manager;
	std::function<void(int, int)> load_connected_locations;
	std::function<void(int, int, const Rewards_Manager_Lambda&)> load_location_chest;
	std::function<void()> clear_location_manager;
	std::function<void()> display_all;
};

struct Entity_Manager_Lambda
{
	std::function<const std::unordered_map<int, std::shared_ptr<Entity>>& ()> get_entity_manager;
	std::function<void(int, std::shared_ptr<Entity>)> load_entity_manager;
	std::function<void(int, int, const Owned_Items&)> load_inventories;
	std::function<void(int, int, int, const World_Inventory_Lambda&)> link_equipment;
	std::function<void(int, int, bool)> load_completed_and_ongoing_quests;
	std::function<void()> clear_entity_manager;
	std::function<void()> display_all;
	std::function<void()> display_player;
	std::function<void()> display_none_player;
	std::function<std::weak_ptr<Entity>(int)> get_entity;
	std::function<void(const Data_Base_Lambda& data_base_lambda)> dirty_entity_checkpoint;
};

struct Data_Base_Lambda
{
	//INSERTING INTO TABLES LAMBDA FUNCTIONS
	std::function<void(const Item&, int)> insert_item;
	std::function<void(const Entity&, int)> insert_entity;
	std::function<void(const Rewards&, int)> insert_reward;
	std::function<void(const Location&, int)> insert_location;
	std::function<void(const Quest&, int)> insert_quest;
	std::function<void(int, int)> insert_connected_locations;
	std::function<void(int, int, int)> insert_inventories;
	std::function<void(int, int, bool)> insert_completed_and_ongoing_quest;
	std::function<void(int, int, int)> insert_reward_items;
	std::function<void(int, int, int)> insert_quest_enemies;
	std::function<void(int, int)> insert_quest_reward;
	std::function<void(int, int)> insert_location_reward;

	//DELETING FROM TABLES LAMBDA FUNCTIONS
	std::function<void(int, Container_Type)> delete_from_main_container;
	std::function<void(int, int, Sub_Container_Type)> delete_from_sub_container;

	//UPDATING TABLES LAMBDA FUNCTIONS
	std::function<void(int, int)> update_entity_location;
	std::function<void(int, int, Item_Type)> update_entity_equipped_item;
	std::function<void(int, int, int, Sub_Container_Type)> update_sub_container_quantity;
	std::function<void(std::vector<std::string>, Container_Type, int)> update_properties;
	std::function<void(const Item&, int)> update_item;
	std::function<void(const Entity&, int)> update_entity;
	std::function<void(const Rewards&, int)> update_reward;
	std::function<void(const Location&, int)> update_location;
	std::function<void(const Quest&, int)> update_quest;
};

struct Party_Lambda
{
	std::function<void()> display_party_members_details;
	std::function<const Party_State()> get_party_state;

	std::function<void(int, int)> take_gold;
	std::function<void(int, int)> give_gold;

	std::function<void(int, int, int)> take_item;
	std::function<void(int, int, int, const World_Inventory_Lambda&)> give_item;

	std::function<int(std::string)> ask_index;
};

struct Shop_lambda
{

};

/////////////////////////////////////////////////////////////////////////////////////////////////////

//ITEM
class Item
{
private:
	//ITEM VARIABLES
	std::string name, type;
	std::vector<std::string> properties;
	int value;
	bool is_magical;

public:
	//ITEM GETTERS AND SETTERS
	const std::string& get_name() const;
	const std::string& get_type() const;
	const std::vector<std::string>& get_properties() const;
	const int& get_value() const;
	const bool& get_is_magical() const;

	void set_properties(std::vector<std::string> new_properties);
	void add_property(std::string new_property);
	void remove_property(std::string new_property);
	void set_name(std::string new_name);
	void set_type(std::string new_type);
	void set_value(int new_value);
	void set_magical(bool state);

	//CLASS CHILD TYPE
	virtual Item_Type child_type() const = 0;

	//WEAPON GETTERS AND SETTERS
	virtual const int& get_dmg_dice() const;
	virtual const int& get_dmg_bonus() const;
	virtual const int& get_dmg_count() const;
	virtual const float& get_range() const;

	virtual void set_dmg_dice(int D);
	virtual void set_dmg_bonus(int D);
	virtual void set_dmg_count(int D);
	virtual void set_range(float R);

	//ARMOR GETTERS AND SETTERS
	virtual const int& get_armor_class() const;
	virtual const int& get_dex_bonus() const;
	virtual const bool& get_stealth_friendly() const;

	virtual void set_armor_class(int new_ac);
	virtual void set_dex_bonus(int new_dex_bonus);
	virtual void set_stealth_friendly(bool new_stealth);

	//SPELL_SCROLL GETTERS AND SETTERS
	virtual const std::string& get_element() const;
	virtual const int& get_mana_cost() const;

	virtual void set_element(std::string new_element);
	virtual void set_mana_cost(int new_mana_cost);

	//CONSUMABLE GETTERS AND SETTERS
	virtual const int& get_health_restore() const;
	virtual const int& get_mana_restore() const;
	virtual const int& get_level_requirement() const;

	virtual void set_health_restore(int new_health_restore);
	virtual void set_mana_restore(int new_mana_restore);
	virtual void set_level_requirement(int new_level_requirement);

	//ITEM FUNCTIONS
	virtual void display_details() const;

	void display_properties() const;

	//CONSTRUCTOR
	Item(std::string new_name, std::string new_type, int new_value, bool new_magical, std::vector<std::string> new_properties);

	virtual ~Item() = default;
};

//WEAPON
class Weapon : public Item
{
private:
	//WEAPON VARIABLES
	int dmg_dice, dmg_bonus, dmg_dice_count;
	float range;

public:
	//GETTERS AND SETTERS
	const int& get_dmg_dice() const override;
	const int& get_dmg_bonus() const override;
	const int& get_dmg_count() const override;
	const float& get_range() const override;

	void set_dmg_dice(int D) override;
	void set_dmg_bonus(int D) override;
	void set_dmg_count(int D) override;
	void set_range(float R) override;

	//CLASS CHILD TYPE
	Item_Type child_type() const override;

	//FUNCTIONS
	void display_details() const override;

	//CONSTRUCTOR
	Weapon(std::string new_name, std::string new_type, int new_value, bool new_magical,
		std::vector<std::string> new_properties, int new_dice, int new_bonus,
		int new_dice_count, float new_range);
};

//SPELL_SCROLL
class Spell_Scroll : public Weapon
{
private:
	//SPELL_SCROLL VARIABLES
	std::string element;
	int mana_cost;

public:
	//SPELL_SCROLL GETTERS AND SETTERS
	const std::string& get_element() const override;
	const int& get_mana_cost() const override;

	void set_element(std::string new_element) override;
	void set_mana_cost(int new_mana_cost) override;

	//CLASS CHILD TYPE
	Item_Type child_type() const override;

	//FUNCTIONS
	void display_details() const override;

	//CONSTRUCTOR
	Spell_Scroll(std::string new_name, std::string new_type, int new_value, bool new_magical,
		std::vector<std::string> new_properties, int new_dice, int new_bonus,
		int new_dice_count, float new_range, std::string new_element, int new_mana_cost);
};

//ARMOR
class Armor : public Item
{
private:
	int armor_class, max_dex_bonus;
	bool stealth_friendly;

public:
	//ARMOR GETTERS AND SETTERS
	const int& get_armor_class() const override;
	const int& get_dex_bonus() const override;
	const bool& get_stealth_friendly() const override;

	void set_armor_class(int new_ac) override;
	void set_dex_bonus(int new_dex_bonus) override;

	void set_stealth_friendly(bool new_stealth) override;

	//CLASS CHILD TYPE 
	Item_Type child_type() const override;

	//FUNCTIONS
	void display_details() const override;

	//CONSTRUCTOR
	Armor(std::string new_name, std::string new_type, int new_value, bool new_magical,
		std::vector<std::string> new_properties, int new_ac, int new_dex_bonus, bool new_stealth);
};

//CONSUMABLE
class Consumable : public Item
{
private:
	//CONSUMABLE VARIABLES
	int health_restore, mana_restore, level_requirement;

public:
	//VARIABLE GETTERS AND SETTERS
	const int& get_health_restore() const override;
	const int& get_mana_restore() const override;
	const int& get_level_requirement() const override;

	void set_health_restore(int new_health_restore) override;
	void set_mana_restore(int new_mana_restore) override;
	void set_level_requirement(int new_level_requirement) override;

	//CLASS CHILD TYPE
	Item_Type child_type() const override;

	//FUNCTIONS
	void display_details() const override;

	//CONSTRUCTOR
	Consumable(std::string new_name, std::string new_type, int new_value, bool new_magical, std::vector<std::string> new_properties,
		int new_health_restore, int new_mana_restore, int new_level_requirement);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

//ENTITY
class Entity
{
private:
	//ENTITY VARIABLES
	std::string name, background, class_type;
	Attributes stats;
	int level{ 1 }, xp{ 0 }, max_hp{ 100 }, current_hp{ max_hp }, max_mana{ 160 }, current_mana{ max_mana }, armorclass{ 10 }, gold{ 0 }, current_location_id{ 0 };
	bool is_player, is_dead{ false }, dirty{ false };
	std::vector<int> completed_quests;
	std::vector<int> ongoing_quests;
	std::unordered_map<int, Owned_Items> character_inventory;
	std::pair<int, std::weak_ptr<Item>> equipped_weapon;
	std::pair<int, std::weak_ptr<Item>> equipped_armor;

public:
	//ENTITY GETTERS AND SETTERS
	const std::string& get_name() const;
	const std::string& get_background() const;
	const std::string& get_class_type() const;
	const Attributes& get_stats() const;
	const int& get_level() const;
	const int& get_xp() const;
	const int& get_max_hp() const;
	const int& get_current_hp() const;
	const int& get_max_mana() const;
	const int& get_current_mana() const;
	const int& get_armorclass() const;
	const int& get_gold() const;
	const int& get_current_locaion_id() const;
	const bool& get_is_player() const;
	const bool& get_is_dead() const;
	const bool& get_dirty() const;
	const std::vector<int>& get_completed_quests() const;
	const std::vector<int>& get_ongoing_quests() const;
	const std::unordered_map<int, Owned_Items>& get_inventory() const;
	const std::pair<int, std::weak_ptr<Item>>& get_equipped_weapon() const;
	const std::pair<int, std::weak_ptr<Item>>& get_equipped_armor() const;

	void set_name(std::string new_name);
	void set_background(std::string new_background);
	void set_class_type(std::string new_class_type);
	void set_stats(Attributes new_stats);
	void set_level(int new_level);
	void set_xp(int new_xp);
	void set_max_hp(int new_max_hp);
	void set_current_hp(int new_current_hp);
	void set_max_mana(int new_max_mana);
	void set_current_mana(int new_current_mana);
	void set_armorclass(int new_armorclass);
	void set_gold(int new_gold);
	void set_current_location_id(int new_location_id);
	void set_is_player(bool new_is_player);
	void set_is_dead(bool new_is_dead);
	void set_dirty(bool new_dirty);
	void add_completed_quest(int new_completed_quest);
	void remove_completed_quest(int new_id);
	void add_ongoing_quest(int new_ongoing_quest);
	void remove_ongoing_quest(int new_id);
	void set_equipped_weapon(std::pair<int, std::weak_ptr<Item>> new_equipped_weapon);
	void set_equipped_armor(std::pair<int, std::weak_ptr<Item>> new_equipped_armor);

	//FUNCTIONS
	int get_proficiency_bonus(int new_level) const;

	int get_stat_modifier(int stat_value) const { return (stat_value - 10) / 2; }

	//DISPLAYS
	void display_stats() const;

	void display_info() const;

	void display_inventory();

	//EQUIP AND DE EQUIP
	void equip_item();

	void de_equip_item(std::string weapon_or_armor);

	void set_equipped_weapon_internal(std::pair<int, std::weak_ptr<Item>> new_weapon);

	void set_equipped_armor_internal(std::pair<int, std::weak_ptr<Item>> new_armor);

	//ADD AND REMOVE INTO AND FROM INVENTORY
	void add_item_to_inventory(int item_id, const Owned_Items& new_item);

	void remove_item_from_inventory(int id);

	void remove_item_from_inventory(int id, int count);

	//XP AND LEVEL UP + STATS
	void stats_upgrade(int points);

	void gain_xp(int new_xp);

	void gain_gold(int new_gold);

	void level_up();

	//ITEM USAGE FROM INVENTORY
	int attack() const;

	void update_armor_class();

	int cast_spell(int id);

	void use_consumable(int id);

	void update_death();

	//CONSTRUCTOR
	Entity(std::string new_name, std::string new_background, std::string new_class_type, Attributes new_stats, bool new_is_player);

	Entity(std::string new_name, std::string new_background, std::string new_class_type, Attributes new_stats, int new_level, int new_xp, int new_max_hp, int new_current_hp, 
		int new_max_mana, int new_current_mana, int new_armorclass, int new_gold, int new_current_location_id, bool new_is_player, bool new_is_dead);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

//REWARDS
class Rewards
{
private:
	//REWARDS VARIABLES
	std::unordered_map<int ,Owned_Items> reward_item_pointers;
	int gold, xp;

public:
	//REWARDS GETTERS AND SETTERS
	const std::unordered_map<int, Owned_Items>& get_reward_item_pointers() const;
	const int& get_gold() const;
	const int& get_xp() const;

	void set_reward_item_pointers(std::unordered_map<int, Owned_Items> new_reward_item_pointers);
	void set_gold(int new_gold);
	void set_xp(int new_xp);

	void add_item_pointers(int item_id, const Owned_Items& new_item);
	void remove_item_pointers(int item_id);
	void remove_item_pointers(int item_id, int count);

	//FUNCTIONS
	void display_details() const;

	//CONSTRUCTOR
	Rewards(int new_gold, int new_xp);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

// LOCATION
class Location
{
private:
	//LOCATION VARIABLES
	std::string name;
	std::vector<std::string> properties;
	std::vector<int> connected_location_ids;
	std::weak_ptr<Rewards> chest;

public:
	//LOCATION GETTERS AND SETTERS
	const std::string& get_name() const;
	const std::vector<std::string>& get_properties() const;
	const std::vector<int>& get_connected_location_ids() const;
	const std::weak_ptr<Rewards>& get_chest() const;

	void set_name(std::string new_name);
	void set_properties(std::vector<std::string> new_properties);
	void add_property(std::string new_property);
	void remove_property(std::string new_property);
	void set_connected_location_ids(std::vector<int> new_connected_location_ids);
	void add_connected_location_id(int new_location_id);
	void remove_connected_location_id(int new_location_id);
	void set_chest(std::weak_ptr<Rewards> new_chest);
	void remove_chest();

	//FUNCTIONS
	void display_info() const;

	//CONSTRUCTOR
	Location(std::string new_name, std::vector<std::string> new_properties);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

// QUEST
class Quest
{
private:
	//QUEST VARIABLES
	std::string name, description, quest_giver;
	int quest_level, priority;
	std::weak_ptr<Rewards> reward;
	std::unordered_map<int, Enemies> enemies;

public:
	//QUEST GETTERS AND SETTERS
	const std::string& get_name() const;
	const std::string& get_description() const;
	const std::string& get_quest_giver() const;
	const int& get_quest_level() const;
	const int& get_priority() const;
	const std::weak_ptr<Rewards>& get_reward() const;
	const std::unordered_map<int, Enemies>& get_enemies() const;

	void set_name(std::string new_name);
	void set_description(std::string new_description);
	void set_quest_giver(std::string new_quest_giver);
	void set_quest_level(int new_quest_level);
	void set_priority(int new_priority);
	void set_reward(std::weak_ptr<Rewards> new_reward);
	void remove_reward();
	void set_enemies(std::unordered_map<int, Enemies> new_enemies);
	bool add_enemy(int new_enemy_id, const Enemies& new_enemy);
	void remove_enemy(int enemy_id);
	void remove_enemy(int enemy_id, int quantity);

	//FUNCTIONS
	void display_info_without_enemies() const;

	void display_info() const;

	//CONSTRUCTOR
	Quest(std::string new_name, std::string new_description, std::string new_quest_giver, int new_quest_level, int new_priority);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////