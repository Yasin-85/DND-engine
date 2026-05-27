#include <iostream>
#include <winsqlite/winsqlite3.h>
#include <string>

#include "Data_Structures.h"
#include "Data_Base.h"

static std::string get_str(sqlite3_stmt* stmt, int index) 
{
	const unsigned char* raw = sqlite3_column_text(stmt, index);
	return raw ? reinterpret_cast<const char*>(raw) : "";
}

static int get_fk(sqlite3_stmt* stmt, int index, int sentinel = -1) 
{
	if (sqlite3_column_type(stmt, index) == SQLITE_NULL)
		return sentinel;

	return sqlite3_column_int(stmt, index);
}

void Data_Base::startup()
{
	int rc = sqlite3_open("Save_File.db", &db);
	sqlite3_stmt* stmt{ nullptr };

	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("database open failed");
	}

	rc = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("pragma failed");
	}

	// ITEMS
	const char* sql1 = "CREATE TABLE IF NOT EXISTS items ( "
		"id INTEGER PRIMARY KEY, "
		"name TEXT NOT NULL, "
		"type TEXT NOT NULL, "
		"item_child_type TEXT NOT NULL, "
		"properties TEXT, "
		"value INTEGER CHECK (value > -1) NOT NULL, "
		"is_magical INTEGER CHECK (is_magical IN (0,1)) NOT NULL, "
		"dmg_dice INTEGER CHECK (dmg_dice IN (4,6,8,10,12,20,100)), "
		"dmg_bonus INTEGER CHECK (dmg_bonus > -1), "
		"dmg_dice_count INTEGER CHECK (dmg_dice_count > 0), "
		"range REAL CHECK (range > 0), "
		"armor_class INTEGER CHECK (armor_class > 0), "
		"max_dex_bonus INTEGER CHECK (max_dex_bonus > -1), "
		"stealth_friendly INTEGER CHECK (stealth_friendly IN (0,1)), "
		"element TEXT, "
		"mana_cost INTEGER CHECK (mana_cost > -1), "
		"health_restore INTEGER, "
		"mana_restore INTEGER, "
		"level_requirement INTEGER CHECK (level_requirement > 0) );";

	rc = sqlite3_prepare_v2(db, sql1, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		throw std::runtime_error("step failed");
	}

	sqlite3_finalize(stmt);

	// REWARDS
	const char* sql2 = "CREATE TABLE IF NOT EXISTS rewards ("
		"id INTEGER PRIMARY KEY, "
		"gold INTEGER CHECK (gold > -1), "
		"xp INTEGER CHECK (xp > -1) );";

	rc = sqlite3_prepare_v2(db, sql2, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		throw std::runtime_error("step failed");
	}

	sqlite3_finalize(stmt);

	// LOCATIONS
	const char* sql3 = "CREATE TABLE IF NOT EXISTS locations ("
		"id INTEGER PRIMARY KEY, "
		"name TEXT NOT NULL, "
		"properties TEXT, "
		"reward_id INTEGER DEFAULT NULL REFERENCES rewards(id) ON DELETE SET NULL );";

	rc = sqlite3_prepare_v2(db, sql3, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		throw std::runtime_error("step failed");
	}

	sqlite3_finalize(stmt);

	// ENTITIES
	const char* sql4 = "CREATE TABLE IF NOT EXISTS entities ( "
		"id INTEGER PRIMARY KEY, "
		"name TEXT NOT NULL, "
		"background TEXT NOT NULL, "
		"class_type TEXT NOT NULL, "
		"str INTEGER CHECK (str BETWEEN 1 AND 20) NOT NULL, "
		"dex INTEGER CHECK (dex BETWEEN 1 AND 20) NOT NULL, "
		"cons INTEGER CHECK (cons BETWEEN 1 AND 20) NOT NULL, "
		"int INTEGER CHECK (int BETWEEN 1 AND 20) NOT NULL, "
		"wis INTEGER CHECK (wis BETWEEN 1 AND 20) NOT NULL, "
		"cha INTEGER CHECK (cha BETWEEN 1 AND 20) NOT NULL, "
		"level INTEGER CHECK (level > 0) NOT NULL, "
		"xp INTEGER CHECK (xp > -1) NOT NULL, "
		"max_hp INTEGER CHECK (max_hp > 0) NOT NULL, "
		"current_hp INTEGER CHECK (current_hp BETWEEN 0 AND max_hp) NOT NULL, "
		"max_mana INTEGER CHECK (max_mana > -1) NOT NULL, "
		"current_mana INTEGER CHECK (current_mana BETWEEN 0 AND max_mana) NOT NULL, "
		"armorclass INTEGER CHECK (armorclass > -1) NOT NULL, "
		"gold INTEGER CHECK (gold > -1) NOT NULL, "
		"current_location_id INTEGER DEFAULT NULL REFERENCES locations(id) ON DELETE SET NULL, "
		"is_player INTEGER CHECK (is_player IN (0,1)) NOT NULL, "
		"is_dead INTEGER CHECK (is_dead IN (0,1)) NOT NULL, "
		"equipped_weapon_id INTEGER DEFAULT NULL REFERENCES items(id) ON DELETE SET NULL, "
		"equipped_armor_id INTEGER DEFAULT NULL REFERENCES items(id) ON DELETE SET NULL );";

	rc = sqlite3_prepare_v2(db, sql4, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		throw std::runtime_error("step failed");
	}

	sqlite3_finalize(stmt);

	// QUESTS
	const char* sql5 = "CREATE TABLE IF NOT EXISTS quests ("
		"id INTEGER PRIMARY KEY, "
		"name TEXT NOT NULL, "
		"description TEXT NOT NULL, "
		"quest_giver TEXT NOT NULL, "
		"quest_level INTEGER NOT NULL, "
		"priority INTEGER NOT NULL, "
		"reward_id INTEGER DEFAULT NULL REFERENCES rewards(id) ON DELETE SET NULL );";

	rc = sqlite3_prepare_v2(db, sql5, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		throw std::runtime_error("step failed");
	}

	sqlite3_finalize(stmt);

	// CONNECTED LOCATIONS
	const char* sql6 = "CREATE TABLE IF NOT EXISTS connected_locations ("
		"origin_location_id INTEGER NOT NULL REFERENCES locations(id) ON DELETE CASCADE, "
		"target_location_id INTEGER NOT NULL REFERENCES locations(id) ON DELETE CASCADE, "
		"CHECK (target_location_id != origin_location_id), "
		"PRIMARY KEY (origin_location_id, target_location_id) );";

	rc = sqlite3_prepare_v2(db, sql6, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		throw std::runtime_error("step failed");
	}

	sqlite3_finalize(stmt);

	// INVENTORIES
	const char* sql7 = "CREATE TABLE IF NOT EXISTS inventories ("
		"entity_id INTEGER NOT NULL REFERENCES entities(id) ON DELETE CASCADE, "
		"item_id INTEGER NOT NULL REFERENCES items(id) ON DELETE CASCADE, "
		"quantity INTEGER NOT NULL CHECK (quantity > 0), "
		"PRIMARY KEY (entity_id, item_id) );";

	rc = sqlite3_prepare_v2(db, sql7, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		throw std::runtime_error("step failed");
	}

	sqlite3_finalize(stmt);

	// COMPLETED AND ONGOING QUESTS
	const char* sql8 = "CREATE TABLE IF NOT EXISTS completed_quests ("
		"entity_id INTEGER NOT NULL REFERENCES entities(id) ON DELETE CASCADE, "
		"quest_id INTEGER NOT NULL REFERENCES quests(id) ON DELETE CASCADE, "
		"completed INTEGER CHECK (completed IN (0,1)) NOT NULL, "
		"PRIMARY KEY (entity_id, quest_id) );";

	rc = sqlite3_prepare_v2(db, sql8, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		throw std::runtime_error("step failed");
	}

	sqlite3_finalize(stmt);

	//REWARD ITEMS
	const char* sql9 = "CREATE TABLE IF NOT EXISTS reward_items ("
		"reward_id INTEGER NOT NULL REFERENCES rewards(id) ON DELETE CASCADE, "
		"item_id INTEGER NOT NULL REFERENCES items(id) ON DELETE CASCADE, "
		"quantity INTEGER CHECK(quantity > 0), "
		"PRIMARY KEY (reward_id, item_id) );";

	rc = sqlite3_prepare_v2(db, sql9, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		throw std::runtime_error("step failed");
	}

	sqlite3_finalize(stmt);

	//QUEST ENEMIES
	const char* sql10 = "CREATE TABLE IF NOT EXISTS quest_enemies ("
		"quest_id INTEGER NOT NULL REFERENCES quests(id) ON DELETE CASCADE, "
		"enemy_id INTEGER NOT NULL REFERENCES entities(id) ON DELETE CASCADE, "
		"quantity INTEGER CHECK(quantity > 0), "
		"PRIMARY KEY (quest_id, enemy_id) );";

	rc = sqlite3_prepare_v2(db, sql10, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		throw std::runtime_error("step failed");
	}

	sqlite3_finalize(stmt);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::load_EM(const Entity_Manager_Lambda& EM_lambda)
{
	sqlite3_stmt* stmt{ nullptr };
	int rc{ 0 };

	const char* sql = "SELECT id, name, background, class_type, str, dex, cons, int, wis, cha, level, xp, max_hp, current_hp, max_mana, current_mana, armorclass, gold, "
		"current_location_id, is_player, is_dead, equipped_weapon_id, equipped_armor_id FROM entities;";

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int id = sqlite3_column_int(stmt, 0);
		std::string name = get_str(stmt, 1);
		std::string background = get_str(stmt, 2);
		std::string class_type = get_str(stmt, 3);

		Attributes stats;
		stats.str_ = sqlite3_column_int(stmt, 4);
		stats.dex_ = sqlite3_column_int(stmt, 5);
		stats.cons_ = sqlite3_column_int(stmt, 6);
		stats.int_ = sqlite3_column_int(stmt, 7);
		stats.wis_ = sqlite3_column_int(stmt, 8);
		stats.cha_ = sqlite3_column_int(stmt, 9);

		int level = sqlite3_column_int(stmt, 10);
		int xp = sqlite3_column_int(stmt, 11);
		int max_hp = sqlite3_column_int(stmt, 12);
		int current_hp = sqlite3_column_int(stmt, 13);
		int max_mana = sqlite3_column_int(stmt, 14);
		int current_mana = sqlite3_column_int(stmt, 15);
		int armorclass = sqlite3_column_int(stmt, 16);
		int gold = sqlite3_column_int(stmt, 17);
		int current_location_id = get_fk(stmt, 18, 0);
		bool is_player = sqlite3_column_int(stmt, 19);
		bool is_dead = sqlite3_column_int(stmt, 20);
		int weapon_id = get_fk(stmt, 21, -1);
		int armor_id = get_fk(stmt, 22, -1);

		temp_equipment_map[id] = std::make_pair(weapon_id, armor_id);

		EM_lambda.load_entity_manager(id, std::make_shared<Entity>(name, background, class_type, stats, level, xp, max_hp, current_hp, max_mana, current_mana,
			armorclass, gold, current_location_id, is_player, is_dead));
	}

	sqlite3_finalize(stmt);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::load_LM(const Location_Manager_Lambda& LM_lambda, const Rewards_Manager_Lambda& RM_lambda)
{
	sqlite3_stmt* stmt{ nullptr };
	int rc{ 0 };

	const char* sql = "SELECT id, name, properties, reward_id FROM locations;";

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int id = sqlite3_column_int(stmt, 0);
		std::string name = get_str(stmt, 1);

		std::vector<std::string> properties;
		std::string props_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		if (!props_str.empty()) {
			size_t pos = 0;
			size_t found;
			std::string token;
			while ((found = props_str.find("||", pos)) != std::string::npos) {
				token = props_str.substr(pos, found - pos);
				if (!token.empty()) {
					properties.push_back(token);
				}
				pos = found + 2;  // Skip "||"
			}
		}

		LM_lambda.load_location_manager(id, std::make_shared<Location>(name, properties));

		int reward_id = get_fk(stmt, 3);

		if (reward_id > 0)
			LM_lambda.load_location_chest(id, reward_id, RM_lambda);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::load_QM(const Quest_Manager_Lambda& QM_lambda, const Rewards_Manager_Lambda& RM_lambda)
{
	sqlite3_stmt* stmt;
	int rc{ 0 };

	const char* sql = "SELECT id, name, description, quest_giver, quest_level, priority, reward_id FROM quests;";

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int id = sqlite3_column_int(stmt, 0);
		std::string name = get_str(stmt, 1);
		std::string description = get_str(stmt, 2);
		std::string quest_giver = get_str(stmt, 3);
		int quest_level = sqlite3_column_int(stmt, 4);
		int priority = sqlite3_column_int(stmt, 5);

		QM_lambda.load_quest_manager(id, std::make_shared<Quest>(name, description, quest_giver, quest_level, priority));

		int reward_id = get_fk(stmt, 6);

		if (reward_id > 0)
			QM_lambda.load_quest_reward(id, reward_id, RM_lambda);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::load_RM(const Rewards_Manager_Lambda& RM_lambda)
{
	sqlite3_stmt* stmt;
	int rc{ 0 };
	
	//REWARDS
	const char* sql = "SELECT id, gold, xp FROM rewards;";

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int id = sqlite3_column_int(stmt, 0);
		int gold = sqlite3_column_int(stmt, 1);
		int xp = sqlite3_column_int(stmt, 2);

		RM_lambda.load_rewards_manager(id, std::make_shared<Rewards>(gold, xp));
	}

	sqlite3_finalize(stmt);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::load_WI(const World_Inventory_Lambda& WI_lambda)
{
	sqlite3_stmt* stmt;
	int rc{ 0 };


	//ITEMS
	const char* sql = "SELECT id, name, type, item_child_type, properties, value, is_magical, "
		"dmg_dice, dmg_bonus, dmg_dice_count, range, "
		"armor_class, max_dex_bonus, stealth_friendly, "
		"element, mana_cost, "
		"health_restore, mana_restore, level_requirement FROM items;";

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int id = sqlite3_column_int(stmt, 0);
		std::string name = get_str(stmt, 1);
		std::string type = get_str(stmt, 2);

		Item_Type item_child_type;
		std::string item_type = get_str(stmt, 3);
		if (item_type == "Weapon")
			item_child_type = Item_Type::Weapon;

		else if (item_type == "Spell_Scroll")
			item_child_type = Item_Type::Spell_Scroll;

		else if (item_type == "Armor")
			item_child_type = Item_Type::Armor;

		else if (item_type == "Consumable")
			item_child_type = Item_Type::Consumable;

		else
			throw std::runtime_error("invalid item child type");

		std::vector<std::string> properties;
		std::string props_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
		if (!props_str.empty()) {
			size_t pos = 0;
			size_t found;
			std::string token;
			while ((found = props_str.find("||", pos)) != std::string::npos) {
				token = props_str.substr(pos, found - pos);
				if (!token.empty()) {
					properties.push_back(token);
				}
				pos = found + 2;  // Skip "||"
			}
		}

		int value = sqlite3_column_int(stmt, 5);
		bool is_magical = sqlite3_column_int(stmt, 6);

		switch (item_child_type)
		{
		case Item_Type::Weapon:
		{
			int dmg_dice = sqlite3_column_int(stmt, 7);
			int dmg_bonus = sqlite3_column_int(stmt, 8);
			int dmg_dice_count = sqlite3_column_int(stmt, 9);
			float range = sqlite3_column_double(stmt, 10);

			WI_lambda.load_world_inventory(id, std::make_shared<Weapon>(name, type, value, is_magical,
				properties, dmg_dice, dmg_bonus,
				dmg_dice_count, range));
			break;
		}

		case Item_Type::Armor:
		{
			int armor_class = sqlite3_column_int(stmt, 11);
			int	max_dex_bonus = sqlite3_column_int(stmt, 12);
			bool stealth_friendly = sqlite3_column_int(stmt, 13);

			WI_lambda.load_world_inventory(id, std::make_shared<Armor>(name, type, value, is_magical,
				properties, armor_class,
				max_dex_bonus, stealth_friendly));
			break;
		}

		case Item_Type::Spell_Scroll:
		{
			int dmg_dice = sqlite3_column_int(stmt, 7);
			int dmg_bonus = sqlite3_column_int(stmt, 8);
			int dmg_dice_count = sqlite3_column_int(stmt, 9);
			float range = sqlite3_column_double(stmt, 10);
			std::string element = get_str(stmt, 14);
			int mana_cost = sqlite3_column_int(stmt, 15);

			WI_lambda.load_world_inventory(id, std::make_shared<Spell_Scroll>(name, type, value, is_magical,
				properties, dmg_dice, dmg_bonus,
				dmg_dice_count, range, element, mana_cost));
			break;
		}

		case Item_Type::Consumable:
		{
			int health_restore = sqlite3_column_int(stmt, 16);
			int mana_restore = sqlite3_column_int(stmt, 17);
			int level_requirement = sqlite3_column_int(stmt, 18);

			WI_lambda.load_world_inventory(id, std::make_shared<Consumable>(name, type, value, is_magical,
				properties, health_restore, mana_restore, level_requirement));
			break;
		}

		default:
			throw std::runtime_error("invalid item child type");
			break;
		}
	}

	sqlite3_finalize(stmt);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::load_connected_locations(const Location_Manager_Lambda& LM_lambda)
{
	sqlite3_stmt* stmt{ nullptr };
	int rc{ 0 };

	const char* sql = "SELECT origin_location_id, target_location_id FROM connected_locations;";

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int origin_location_id = sqlite3_column_int(stmt, 0);
		int target_location_id = sqlite3_column_int(stmt, 1);

		LM_lambda.load_connected_locations(origin_location_id, target_location_id);
	}

	sqlite3_finalize(stmt);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::load_inventories(const Entity_Manager_Lambda& EM_lambda, const World_Inventory_Lambda& WI_lambda) {
	sqlite3_stmt* stmt = nullptr;
	int rc = 0;

	const char* sql = "SELECT entity_id, item_id, quantity FROM inventories;";

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK) 
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) 
	{
		int entity_id = sqlite3_column_int(stmt, 0);
		int item_id = sqlite3_column_int(stmt, 1);
		int quantity = sqlite3_column_int(stmt, 2);

		Owned_Items new_item;
		new_item.item = WI_lambda.get_item(item_id);
		new_item.quantity = quantity;

		EM_lambda.load_inventories(entity_id, item_id, new_item);
	}

	sqlite3_finalize(stmt);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::link_entity_equipment(const Entity_Manager_Lambda& EM_lambda, const World_Inventory_Lambda& WI_lambda)
{
	for (auto it = temp_equipment_map.begin(); it != temp_equipment_map.end(); ++it) 
	{
		int entity_id = it->first;
		int weapon_id = it->second.first;
		int armor_id = it->second.second;

		EM_lambda.link_equipment(entity_id, weapon_id, armor_id, WI_lambda);
	}
	temp_equipment_map.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::load_completed_and_ongoing_quests(const Entity_Manager_Lambda& EM_lambda)
{
	sqlite3_stmt* stmt = nullptr;
	int rc = 0;

	const char* sql = "SELECT entity_id, quest_id, completed FROM completed_quests;";

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int entity_id = sqlite3_column_int(stmt, 0);
		int quest_id = sqlite3_column_int(stmt, 1);
		bool completed = sqlite3_column_int(stmt, 2);

		EM_lambda.load_completed_and_ongoing_quests(entity_id, quest_id, completed);
	}

	sqlite3_finalize(stmt);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::load_reward_items(const Rewards_Manager_Lambda& RM_lambda, const World_Inventory_Lambda& WI_lambda)
{

	sqlite3_stmt* stmt = nullptr;
	int rc = 0;

	const char* sql = "SELECT reward_id, item_id, quantity FROM reward_items;";

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int reward_id = sqlite3_column_int(stmt, 0);
		int item_id = sqlite3_column_int(stmt, 1);
		int quantity = sqlite3_column_int(stmt, 2);

		Owned_Items new_item;
		new_item.item = WI_lambda.get_item(item_id);
		new_item.quantity = quantity;

		RM_lambda.load_reward_items(reward_id, item_id, new_item);
	}

	sqlite3_finalize(stmt);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::load_quest_enemies(const Quest_Manager_Lambda& QM_lambda, const Entity_Manager_Lambda& EM_lambda)
{
	sqlite3_stmt* stmt{ nullptr };
	int rc{ 0 };

	const char* sql = "SELECT quest_id, enemy_id, quantity FROM quest_enemies;";

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << sqlite3_errmsg(db) << '\n';
		throw std::runtime_error("prepare failed");
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int quest_id = sqlite3_column_int(stmt, 0);
		int enemy_id = sqlite3_column_int(stmt, 1);
		int quantity = sqlite3_column_int(stmt, 2);

		Enemies new_enemies;
		new_enemies.enemy_ptr = EM_lambda.get_entity(enemy_id);
		new_enemies.quantity = quantity;

		QM_lambda.load_quest_enemies(quest_id, enemy_id, new_enemies);
	}

	sqlite3_finalize(stmt);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::load_all(const Entity_Manager_Lambda& EM_lambda,
	const Location_Manager_Lambda& LM_lambda,
	const Quest_Manager_Lambda& QM_lambda,
	const Rewards_Manager_Lambda& RM_lambda,
	const World_Inventory_Lambda& WI_lambda)
{
	load_WI(WI_lambda);
	load_RM(RM_lambda);
	load_LM(LM_lambda, RM_lambda);
	load_EM(EM_lambda);
	load_QM(QM_lambda, RM_lambda);

	load_connected_locations(LM_lambda);
	load_inventories(EM_lambda, WI_lambda);
	link_entity_equipment(EM_lambda, WI_lambda);
	load_completed_and_ongoing_quests(EM_lambda);
	load_reward_items(RM_lambda, WI_lambda);
	load_quest_enemies(QM_lambda, EM_lambda);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::clear_all_containers(const Entity_Manager_Lambda& EM_lambda,
	const Location_Manager_Lambda& LM_lambda,
	const Quest_Manager_Lambda& QM_lambda,
	const Rewards_Manager_Lambda& RM_lambda,
	const World_Inventory_Lambda& WI_lambda)
{
	EM_lambda.clear_entity_manager();
	LM_lambda.clear_location_manager();
	QM_lambda.clear_quest_manager();
	WI_lambda.clear_world_inventory();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::reload(const Entity_Manager_Lambda& EM_lambda,
	const Location_Manager_Lambda& LM_lambda,
	const Quest_Manager_Lambda& QM_lambda,
	const Rewards_Manager_Lambda& RM_lambda,
	const World_Inventory_Lambda& WI_lambda)
{
	clear_all_containers(EM_lambda, LM_lambda, QM_lambda, RM_lambda, WI_lambda);

	load_all(EM_lambda, LM_lambda, QM_lambda, RM_lambda, WI_lambda);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_insert_item()
{
	lambda->insert_item = [this](const Item& item, int id) 
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			switch (item.child_type())
			{
				//WEAPON
			case Item_Type::Weapon:
			{
				const char* sql = "INSERT INTO items (id, name, type, item_child_type, properties, value, is_magical, dmg_dice, dmg_bonus, dmg_dice_count, range) "
					"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

				rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
				if (rc != SQLITE_OK)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					throw std::runtime_error("prepare failed");
				}

				//BIND PARAMETERS
				sqlite3_bind_int(stmt, 1, id);
				sqlite3_bind_text(stmt, 2, item.get_name().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_text(stmt, 3, item.get_type().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_text(stmt, 4, "Weapon", -1, SQLITE_TRANSIENT);
				std::string properties{};
				for (std::string s : item.get_properties())
				{
					properties += s + "||";
				}
				sqlite3_bind_text(stmt, 5, properties.c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 6, item.get_value());
				sqlite3_bind_int(stmt, 7, item.get_is_magical() ? 1 : 0);
				sqlite3_bind_int(stmt, 8, item.get_dmg_dice());
				sqlite3_bind_int(stmt, 9, item.get_dmg_bonus());
				sqlite3_bind_int(stmt, 10, item.get_dmg_count());
				sqlite3_bind_double(stmt, 11, item.get_range());

				rc = sqlite3_step(stmt);
				if (rc != SQLITE_DONE)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					sqlite3_finalize(stmt);
					throw std::runtime_error("step failed");
				}

				sqlite3_finalize(stmt);
				break;
			}

			//ARMOR
			case Item_Type::Armor:
			{
				const char* sql = "INSERT INTO items (id, name, type, item_child_type, properties, value, is_magical, armor_class, max_dex_bonus, stealth_friendly) "
					"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

				rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
				if (rc != SQLITE_OK)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					throw std::runtime_error("prepare failed");
				}

				sqlite3_bind_int(stmt, 1, id);
				sqlite3_bind_text(stmt, 2, item.get_name().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_text(stmt, 3, item.get_type().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_text(stmt, 4, "Armor", -1, SQLITE_TRANSIENT);
				std::string properties{};
				for (std::string s : item.get_properties())
				{
					properties += s + "||";
				}
				sqlite3_bind_text(stmt, 5, properties.c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 6, item.get_value());
				sqlite3_bind_int(stmt, 7, item.get_is_magical() ? 1 : 0);
				sqlite3_bind_int(stmt, 8, item.get_armor_class());
				sqlite3_bind_int(stmt, 9, item.get_dex_bonus());
				sqlite3_bind_int(stmt, 10, item.get_stealth_friendly() ? 1 : 0);

				rc = sqlite3_step(stmt);
				if (rc != SQLITE_DONE)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					sqlite3_finalize(stmt);
					throw std::runtime_error("step failed");
				}

				sqlite3_finalize(stmt);
				break;
			}

			//SPELL SCROLL
			case Item_Type::Spell_Scroll:
			{
				const char* sql = "INSERT INTO items (id, name, type, item_child_type, properties, value, is_magical, dmg_dice, dmg_bonus, dmg_dice_count, range, element, mana_cost) "
					"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

				rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
				if (rc != SQLITE_OK)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					throw std::runtime_error("prepare failed");
				}

				sqlite3_bind_int(stmt, 1, id);
				sqlite3_bind_text(stmt, 2, item.get_name().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_text(stmt, 3, item.get_type().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_text(stmt, 4, "Spell_Scroll", -1, SQLITE_TRANSIENT);
				std::string properties{};
				for (std::string s : item.get_properties())
				{
					properties += s + "||";
				}
				sqlite3_bind_text(stmt, 5, properties.c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 6, item.get_value());
				sqlite3_bind_int(stmt, 7, item.get_is_magical() ? 1 : 0);
				sqlite3_bind_int(stmt, 8, item.get_dmg_dice());
				sqlite3_bind_int(stmt, 9, item.get_dmg_bonus());
				sqlite3_bind_int(stmt, 10, item.get_dmg_count());
				sqlite3_bind_double(stmt, 11, item.get_range());
				sqlite3_bind_text(stmt, 12, item.get_element().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 13, item.get_mana_cost());

				rc = sqlite3_step(stmt);
				if (rc != SQLITE_DONE)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					sqlite3_finalize(stmt);
					throw std::runtime_error("step failed");
				}

				sqlite3_finalize(stmt);
				break;
			}

			//CONSUMABLE
			case Item_Type::Consumable:
			{
				const char* sql = "INSERT INTO items (id, name, type, item_child_type, properties, value, is_magical, health_restore, mana_restore, level_requirement) "
					"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

				rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
				if (rc != SQLITE_OK)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					throw std::runtime_error("prepare failed");
				}

				sqlite3_bind_int(stmt, 1, id);
				sqlite3_bind_text(stmt, 2, item.get_name().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_text(stmt, 3, item.get_type().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_text(stmt, 4, "Consumable", -1, SQLITE_TRANSIENT);
				std::string properties{};
				for (std::string s : item.get_properties())
				{
					properties += s + "||";
				}
				sqlite3_bind_text(stmt, 5, properties.c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 6, item.get_value());
				sqlite3_bind_int(stmt, 7, item.get_is_magical() ? 1 : 0);
				sqlite3_bind_int(stmt, 8, item.get_health_restore());
				sqlite3_bind_int(stmt, 9, item.get_mana_restore());
				sqlite3_bind_int(stmt, 10, item.get_level_requirement());

				rc = sqlite3_step(stmt);
				if (rc != SQLITE_DONE)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					sqlite3_finalize(stmt);
					throw std::runtime_error("step failed");
				}

				sqlite3_finalize(stmt);
				break;
			}

			default:
				throw std::runtime_error("sqlite insertion faile : item child class is invalid");
				break;
			}
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_insert_entity()
{
	lambda->insert_entity = [this](const Entity& entity, int id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "INSERT INTO entities (id, name, background, class_type, str, dex, cons, int, wis, cha, level, xp, "
				"max_hp, current_hp, max_mana, current_mana, armorclass, gold, is_player, is_dead, current_location_id) "
				"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, id);
			sqlite3_bind_text(stmt, 2, entity.get_name().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 3, entity.get_background().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 4, entity.get_class_type().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 5, entity.get_stats().str_);
			sqlite3_bind_int(stmt, 6, entity.get_stats().dex_);
			sqlite3_bind_int(stmt, 7, entity.get_stats().cons_);
			sqlite3_bind_int(stmt, 8, entity.get_stats().int_);
			sqlite3_bind_int(stmt, 9, entity.get_stats().wis_);
			sqlite3_bind_int(stmt, 10, entity.get_stats().cha_);
			sqlite3_bind_int(stmt, 11, entity.get_level());
			sqlite3_bind_int(stmt, 12, entity.get_xp());
			sqlite3_bind_int(stmt, 13, entity.get_max_hp());
			sqlite3_bind_int(stmt, 14, entity.get_current_hp());
			sqlite3_bind_int(stmt, 15, entity.get_max_mana());
			sqlite3_bind_int(stmt, 16, entity.get_current_mana());
			sqlite3_bind_int(stmt, 17, entity.get_armorclass());
			sqlite3_bind_int(stmt, 18, entity.get_gold());
			sqlite3_bind_int(stmt, 19, entity.get_is_player() ? 1 : 0);
			sqlite3_bind_int(stmt, 20, entity.get_is_dead() ? 1 : 0);
			entity.get_current_locaion_id() == 0 ? sqlite3_bind_null(stmt, 21) : sqlite3_bind_int(stmt, 21, entity.get_current_locaion_id());

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_insert_reward()
{
	lambda->insert_reward = [this](const Rewards& reward, int id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "INSERT INTO rewards (id, gold, xp) VALUES (?, ?, ?);";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, id);
			sqlite3_bind_int(stmt, 2, reward.get_gold());
			sqlite3_bind_int(stmt, 3, reward.get_xp());

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_insert_location()
{
	lambda->insert_location = [this](const Location& location, int id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "INSERT INTO locations(id, name, properties) VALUES (?, ?, ?);";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, id);
			sqlite3_bind_text(stmt, 2, location.get_name().c_str(), -1, SQLITE_TRANSIENT);

			std::string properties{};
			for (std::string s : location.get_properties())
			{
				properties += s + "||";
			}
			sqlite3_bind_text(stmt, 3, properties.c_str(), -1, SQLITE_TRANSIENT);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_insert_quest()
{
	lambda->insert_quest = [this](const Quest& quest, int id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "INSERT INTO quests(id, name, description, quest_giver, quest_level, priority) VALUES (?, ?, ?, ?, ?, ?);";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, id);
			sqlite3_bind_text(stmt, 2, quest.get_name().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 3, quest.get_description().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 4, quest.get_quest_giver().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 5, quest.get_quest_level());
			sqlite3_bind_int(stmt, 6, quest.get_priority());

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_insert_connected_locations()
{
	lambda->insert_connected_locations = [this](int origin_id, int target_id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "INSERT INTO connected_locations(origin_location_id, target_location_id) VALUES (?, ?);";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, origin_id);
			sqlite3_bind_int(stmt, 2, target_id);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_insert_inventories()
{
	lambda->insert_inventories = [this](int entity_id, int item_id, int quantity)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "INSERT INTO inventories(entity_id, item_id, quantity) VALUES (?, ?, ?) "
				"ON CONFLICT(entity_id, item_id) DO UPDATE SET quantity = quantity + excluded.quantity;";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, entity_id);
			sqlite3_bind_int(stmt, 2, item_id);
			sqlite3_bind_int(stmt, 3, quantity);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_insert_completed_and_ongoing_quest()
{
	lambda->insert_completed_and_ongoing_quest = [this](int entity_id, int quest_id, bool completed)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "INSERT INTO completed_quests (entity_id, quest_id, completed) "
				"VALUES (?, ?, ?) "
				"ON CONFLICT(entity_id, quest_id) DO UPDATE SET "
				"completed = excluded.completed;";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, entity_id);
			sqlite3_bind_int(stmt, 2, quest_id);
			sqlite3_bind_int(stmt, 3, completed ? 1 : 0);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_insert_reward_items()
{
	lambda->insert_reward_items = [this](int reward_id, int item_id, int quantity)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "INSERT INTO reward_items(reward_id, item_id, quantity) VALUES (?, ?, ?) "
				"ON CONFLICT(reward_id, item_id) DO UPDATE SET "
				"quantity = quantity + excluded.quantity;";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, reward_id);
			sqlite3_bind_int(stmt, 2, item_id);
			sqlite3_bind_int(stmt, 3, quantity);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_insert_quest_enemies()
{
	lambda->insert_quest_enemies = [this](int quest_id, int enemy_id, int quantity)
		{
			sqlite3_stmt* stmt;
			int rc{ 0 };

			const char* sql = "INSERT INTO quest_enemies(quest_id, enemy_id, quantity) VALUES (?, ?, ?);";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, quest_id);
			sqlite3_bind_int(stmt, 2, enemy_id);
			sqlite3_bind_int(stmt, 3, quantity);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_insert_quest_reward()
{
	lambda->insert_quest_reward = [this](int quest_id, int reward_id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "UPDATE quests SET reward_id = ? WHERE id = ?;";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, reward_id);
			sqlite3_bind_int(stmt, 2, quest_id);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_insert_location_reward()
{
	lambda->insert_location_reward = [this](int location_id, int reward_id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "UPDATE locations SET reward_id = ? WHERE id = ?;";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, reward_id);
			sqlite3_bind_int(stmt, 2, location_id);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_delete_from_main_container()
{
	lambda->delete_from_main_container = [this](int id, Container_Type type)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };
			const char* sql{ nullptr };

			switch (type)
			{
			case Container_Type::Item:
				sql = "DELETE FROM items WHERE ID = ?;";
				break;

			case Container_Type::Entity:
				sql = "DELETE FROM entities WHERE ID = ?;";
				break;

			case Container_Type::Reward:
				sql = "DELETE FROM rewards WHERE ID = ?;";
				break;

			case Container_Type::Location:
				sql = "DELETE FROM locations WHERE ID = ?;";
				break;

			case Container_Type::Quest:
				sql = "DELETE FROM quests WHERE ID = ?;";
				break;

			default:
				throw std::runtime_error("invalid container type");
				break;
			}

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, id);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_delete_from_sub_container()
{
	lambda->delete_from_sub_container = [this](int primary_id, int sub_id, Sub_Container_Type type)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };
			const char* sql{ nullptr };

			switch (type)
			{
			case Sub_Container_Type::Connected_Location:
				sql = "DELETE FROM connected_locations WHERE origin_location_id = ? AND target_location_id = ?;";
				break;

			case Sub_Container_Type::Inventory:
				sql = "DELETE FROM inventories WHERE entity_id = ? AND item_id = ?;";
				break;

			case Sub_Container_Type::Completed_And_Ongoing_quest:
				sql = "DELETE FROM completed_quests WHERE entity_id = ? AND quest_id = ?;";
				break;

			case Sub_Container_Type::Reward_Item:
				sql = "DELETE FROM reward_items WHERE reward_id = ? AND item_id = ?;";
				break;

			case Sub_Container_Type::Quest_Enemy:
				sql = "DELETE FROM quest_enemies WHERE quest_id = ? AND enemy_id = ?;";
				break;

			default:
				throw std::runtime_error("invalid sub container type");
				break;
			}

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, primary_id);
			sqlite3_bind_int(stmt, 2, sub_id);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_update_entity_location()
{
	lambda->update_entity_location = [this](int entity_id, int location_id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "UPDATE entities SET current_location_id = ? WHERE entity_id = ?;";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, location_id);
			sqlite3_bind_int(stmt, 2, entity_id);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_update_entity_equipped_item()
{
	lambda->update_entity_equipped_item = [this](int entity_id, int item_id, Item_Type type)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };
			const char* sql{ nullptr };

			switch (type)
			{
			case Item_Type::Weapon:
				sql = "UPDATE entities SET equipped_weapon_id = ? WHERE id = ?;";
				break;

			case Item_Type::Armor:
				sql = "UPDATE entities SET equipped_armor_id = ? WHERE id = ?;";
				break;

			default:
				throw std::runtime_error("invalid item type");
				break;
			}

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, item_id);
			sqlite3_bind_int(stmt, 2, entity_id);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_update_sub_container_quantity()
{
	lambda->update_sub_container_quantity = [this](int primary_id, int sub_id, int quantity, Sub_Container_Type type)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };
			const char* sql{ nullptr };

			switch (type)
			{
			case Sub_Container_Type::Inventory:
				sql = "UPDATE inventories set quantity = ? WHERE entity_id = ? AND item_id = ?;";
				break;

			case Sub_Container_Type::Quest_Enemy:
				sql = "UPDATE quest_enemies set quantity = ? WHERE quest_id = ? AND enemy_id = ?;";
				break;

			case Sub_Container_Type::Reward_Item:
				sql = "UPDATE reward_items set quantity = ? WHERE reward_id = ? AND item_id = ?;";
				break;

			default:
				throw std::runtime_error("invalid sub container type");
				break;
			}

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, quantity);
			sqlite3_bind_int(stmt, 2, primary_id);
			sqlite3_bind_int(stmt, 3, sub_id);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_update_properties()
{
	lambda->update_properties = [this](std::vector<std::string> new_properties, Container_Type type, int id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };
			const char* sql{ nullptr };

			switch (type)
			{
			case Container_Type::Item:
				sql = "UPDATE items set properties = ? WHERE id = ?;";
				break;

			case Container_Type::Location:
				sql = "UPDATE locations set properties = ? WHERE id = ?;";
				break;;

			default:
				throw std::runtime_error("invalid container type");
				break;
			}

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			std::string properties{};
			for (std::string s : new_properties)
			{
				properties += s + "||";
			}
			sqlite3_bind_text(stmt, 1, properties.c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 2, id);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_update_item()
{
	lambda->update_item = [this](const Item& item, int id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			switch (item.child_type())
			{
				//WEAPON
			case Item_Type::Weapon:
			{
				const char* sql = "UPDATE items SET name = ?, type = ?, properties= ?, value= ?, is_magical = ?, "
					"dmg_dice = ?, dmg_bonus = ?, dmg_dice_count = ?, range = ? "
					"WHERE id = ?;";

				rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
				if (rc != SQLITE_OK)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					throw std::runtime_error("prepare failed");
				}

				//BIND PARAMETERS
				sqlite3_bind_text(stmt, 1, item.get_name().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_text(stmt, 2, item.get_type().c_str(), -1, SQLITE_TRANSIENT);

				std::string properties{};
				for (std::string s : item.get_properties())
				{
					properties += s + "||";
				}
				sqlite3_bind_text(stmt, 3, properties.c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 4, item.get_value());
				sqlite3_bind_int(stmt, 5, item.get_is_magical());
				sqlite3_bind_int(stmt, 6, item.get_dmg_dice());
				sqlite3_bind_int(stmt, 7, item.get_dmg_bonus());
				sqlite3_bind_int(stmt, 8, item.get_dmg_count());
				sqlite3_bind_double(stmt, 9, item.get_range());
				sqlite3_bind_int(stmt, 10, id);

				rc = sqlite3_step(stmt);
				if (rc != SQLITE_DONE)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					sqlite3_finalize(stmt);
					throw std::runtime_error("step failed");
				}

				sqlite3_finalize(stmt);
				break;
			}

			//ARMOR
			case Item_Type::Armor:
			{
				const char* sql = "UPDATE items SET name = ?, type = ?, properties = ?, value = ?, is_magical = ?, armor_class = ?, max_dex_bonus = ?, stealth_friendly = ? "
					"WHERE id = ?;";

				rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
				if (rc != SQLITE_OK)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					throw std::runtime_error("prepare failed");
				}

				sqlite3_bind_text(stmt, 1, item.get_name().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_text(stmt, 2, item.get_type().c_str(), -1, SQLITE_TRANSIENT);

				std::string properties{};
				for (std::string s : item.get_properties())
				{
					properties += s + "||";
				}
				sqlite3_bind_text(stmt, 3, properties.c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 4, item.get_value());
				sqlite3_bind_int(stmt, 5, item.get_is_magical());
				sqlite3_bind_int(stmt, 6, item.get_armor_class());
				sqlite3_bind_int(stmt, 7, item.get_dex_bonus());
				sqlite3_bind_int(stmt, 8, item.get_stealth_friendly() ? 1 : 0);
				sqlite3_bind_int(stmt, 9, id);

				rc = sqlite3_step(stmt);
				if (rc != SQLITE_DONE)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					sqlite3_finalize(stmt);
					throw std::runtime_error("step failed");
				}

				sqlite3_finalize(stmt);
				break;
			}

			//SPELL SCROLL
			case Item_Type::Spell_Scroll:
			{
				const char* sql = "UPDATE items SET name = ?, type = ?, properties = ?, value = ?, is_magical = ?, "
					"dmg_dice = ?, dmg_bonus = ?, dmg_dice_count = ?, range = ?, element = ?, mana_cost = ? "
					"WHERE id = ?;";

				rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
				if (rc != SQLITE_OK)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					throw std::runtime_error("prepare failed");
				}

				sqlite3_bind_text(stmt, 1, item.get_name().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_text(stmt, 2, item.get_type().c_str(), -1, SQLITE_TRANSIENT);

				std::string properties{};
				for (std::string s : item.get_properties())
				{
					properties += s + "||";
				}
				sqlite3_bind_text(stmt, 3, properties.c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 4, item.get_value());
				sqlite3_bind_int(stmt, 5, item.get_is_magical() ? 1 : 0);
				sqlite3_bind_int(stmt, 6, item.get_dmg_dice());
				sqlite3_bind_int(stmt, 7, item.get_dmg_bonus());
				sqlite3_bind_int(stmt, 8, item.get_dmg_count());
				sqlite3_bind_double(stmt, 9, item.get_range());
				sqlite3_bind_text(stmt, 10, item.get_element().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 11, item.get_mana_cost());
				sqlite3_bind_int(stmt, 12, id);

				rc = sqlite3_step(stmt);
				if (rc != SQLITE_DONE)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					sqlite3_finalize(stmt);
					throw std::runtime_error("step failed");
				}

				sqlite3_finalize(stmt);
				break;
			}

			//CONSUMABLE
			case Item_Type::Consumable:
			{
				const char* sql = "UPDATE items SET name = ?, type = ?, properties = ?, value = ?, is_magical = ?, health_restore = ?, mana_restore = ?, level_requirement = ? "
					"WHERE id = ?;";

				rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
				if (rc != SQLITE_OK)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					throw std::runtime_error("prepare failed");
				}

				sqlite3_bind_text(stmt, 1, item.get_name().c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_text(stmt, 2, item.get_type().c_str(), -1, SQLITE_TRANSIENT);

				std::string properties{};
				for (std::string s : item.get_properties())
				{
					properties += s + "||";
				}
				sqlite3_bind_text(stmt, 3, properties.c_str(), -1, SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 4, item.get_value());
				sqlite3_bind_int(stmt, 5, item.get_is_magical() ? 1 : 0);
				sqlite3_bind_int(stmt, 6, item.get_health_restore());
				sqlite3_bind_int(stmt, 7, item.get_mana_restore());
				sqlite3_bind_int(stmt, 8, item.get_level_requirement());
				sqlite3_bind_int(stmt, 9, id);

				rc = sqlite3_step(stmt);
				if (rc != SQLITE_DONE)
				{
					std::cerr << sqlite3_errmsg(this->db) << '\n';
					sqlite3_finalize(stmt);
					throw std::runtime_error("step failed");
				}

				sqlite3_finalize(stmt);
				break;
			}

			default:
				throw std::runtime_error("sqlite insertion faile : item child class is invalid");
				break;
			}
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_update_entity()
{
	lambda->update_entity = [this](const Entity& entity, int id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "UPDATE entities SET name = ?, background = ?, class_type = ?, str = ?, dex = ?, cons = ?, int = ?, wis = ?, cha = ?, "
				"level = ?, xp = ?, max_hp = ?, current_hp = ?, max_mana = ?, current_mana = ?, armorclass = ?, gold = ?, current_location_id = ?, is_player = ?, is_dead = ? "
				"WHERE id = ?";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_text(stmt, 1, entity.get_name().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 2, entity.get_background().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 3, entity.get_class_type().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 4, entity.get_stats().str_);
			sqlite3_bind_int(stmt, 5, entity.get_stats().dex_);
			sqlite3_bind_int(stmt, 6, entity.get_stats().cons_);
			sqlite3_bind_int(stmt, 7, entity.get_stats().int_);
			sqlite3_bind_int(stmt, 8, entity.get_stats().wis_);
			sqlite3_bind_int(stmt, 9, entity.get_stats().cha_);
			sqlite3_bind_int(stmt, 10, entity.get_level());
			sqlite3_bind_int(stmt, 11, entity.get_xp());
			sqlite3_bind_int(stmt, 12, entity.get_max_hp());
			sqlite3_bind_int(stmt, 13, entity.get_current_hp());
			sqlite3_bind_int(stmt, 14, entity.get_max_mana());
			sqlite3_bind_int(stmt, 15, entity.get_current_mana());
			sqlite3_bind_int(stmt, 16, entity.get_armorclass());
			sqlite3_bind_int(stmt, 17, entity.get_gold());
			entity.get_current_locaion_id() == 0 ? sqlite3_bind_null(stmt, 18) : sqlite3_bind_int(stmt, 18, entity.get_current_locaion_id());
			sqlite3_bind_int(stmt, 19, entity.get_is_player() ? 1 : 0);
			sqlite3_bind_int(stmt, 20, entity.get_is_dead() ? 1 : 0);
			sqlite3_bind_int(stmt, 21, id);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_update_reward()
{
	lambda->update_reward = [this](const Rewards& reward, int id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "UPDATE rewards SET gold = ?, xp = ? WHERE id = ?;";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_int(stmt, 1, reward.get_gold());
			sqlite3_bind_int(stmt, 2, reward.get_xp());
			sqlite3_bind_int(stmt, 3, id);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_update_location()
{
	lambda->update_location = [this](const Location& location, int id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "UPDATE locations SET name= ?, properties = ? WHERE id = ?; ";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_text(stmt, 1, location.get_name().c_str(), -1, SQLITE_TRANSIENT);

			std::string properties{};
			for (std::string s : location.get_properties())
			{
				properties += s + "||";
			}
			sqlite3_bind_text(stmt, 2, properties.c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 3, id);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Data_Base::set_update_quest()
{
	lambda->update_quest = [this](const Quest& quest, int id)
		{
			sqlite3_stmt* stmt{ nullptr };
			int rc{ 0 };

			const char* sql = "UPDATE quests SET name = ?, description = ?, quest_giver = ?, quest_level = ?, priority = ? WHERE id = ?;";

			rc = sqlite3_prepare_v2(this->db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				throw std::runtime_error("prepare failed");
			}

			sqlite3_bind_text(stmt, 1, quest.get_name().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 2, quest.get_description().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 3, quest.get_quest_giver().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 4, quest.get_quest_level());
			sqlite3_bind_int(stmt, 5, quest.get_priority());
			sqlite3_bind_int(stmt, 6, id);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
			{
				std::cerr << sqlite3_errmsg(this->db) << '\n';
				sqlite3_finalize(stmt);
				throw std::runtime_error("step failed");
			}

			sqlite3_finalize(stmt);
		};
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

Data_Base::Data_Base() : lambda(std::make_unique<Data_Base_Lambda>())
{
	startup();

	//SET INSERTING INTO TABLES LAMBDA FUNCTIONS
	set_insert_item();
	set_insert_entity();
	set_insert_reward();
	set_insert_location();
	set_insert_quest();
	set_insert_connected_locations();
	set_insert_inventories();
	set_insert_completed_and_ongoing_quest();
	set_insert_reward_items();
	set_insert_quest_enemies();
	set_insert_quest_reward();
	set_insert_location_reward();

	//SET DELETING FROM TABLES LAMBDA FUNCTIONS
	set_delete_from_main_container();
	set_delete_from_sub_container();

	//SET UPDATING TABLES LAMBDA FUNCTIONS
	set_update_entity_location();
	set_update_entity_equipped_item();
	set_update_sub_container_quantity();
	set_update_properties();
	set_update_item();
	set_update_entity();
	set_update_reward();
	set_update_location();
	set_update_quest();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

Data_Base::~Data_Base() 
{
	if (db)
		sqlite3_close(db);
}