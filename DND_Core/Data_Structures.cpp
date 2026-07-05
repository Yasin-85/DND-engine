#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <unordered_map>

#include "Utils.h"
#include "Data_Structures.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

//ITEM

//ITEM GETTERS AND SETTERS
const std::string& Item::get_name() const { return name; }
const std::string& Item::get_type() const { return type; }
const std::vector<std::string>& Item::get_properties() const { return properties; }
const int& Item::get_value() const { return value; }
const bool& Item::get_is_magical() const { return is_magical; }

void Item::set_properties(std::vector<std::string> new_properties) { properties = new_properties; }
void Item::add_property(std::string new_property) { properties.push_back(new_property); }
void Item::remove_property(std::string new_property)
{
	auto it = std::find(properties.begin(), properties.end(), new_property);

	if (it != properties.end())
	{
		properties.erase(it);
		print("property deleted\n");
	}
	else
		print("property not found\n");
}

void Item::set_name(std::string new_name) { name = new_name; }
void Item::set_type(std::string new_type) { type = new_type; }
void Item::set_value(int new_value)
{
	if (new_value > -1)
		value = new_value;
	else
		throw std::invalid_argument("invalid value entered (positive only)");
}
void Item::set_magical(bool state) { is_magical = state; }

//WEAPON GETTERS AND SETTERS
const int& Item::get_dmg_dice() const { return 0; }
const int& Item::get_dmg_bonus() const { return 0; }
const int& Item::get_dmg_count() const { return 0; }
const float& Item::get_range() const { return 0.0f; }

void Item::set_dmg_dice(int D) {}
void Item::set_dmg_bonus(int D) {}
void Item::set_dmg_count(int D) {}
void Item::set_range(float R) {}

//ARMOR GETTERS AND SETTERS
const int& Item::get_armor_class() const { return 0; }
const int& Item::get_dex_bonus() const { return 0; }
const bool& Item::get_stealth_friendly() const { return false; }

void Item::set_armor_class(int new_ac) {}
void Item::set_dex_bonus(int new_dex_bonus) {}
void Item::set_stealth_friendly(bool new_stealth) {}

//SPELL_SCROLL GETTERS AND SETTERS
const std::string& Item::get_element() const { return ""; }
const int& Item::get_mana_cost() const { return 0; }

void Item::set_element(std::string new_element) {}
void Item::set_mana_cost(int new_mana_cost) {}

//CONSUMABLE GETTERS AND SETTERS
const int& Item::get_health_restore() const { return 0; }
const int& Item::get_mana_restore() const { return 0; }
const int& Item::get_level_requirement() const { return 0; }

void Item::set_health_restore(int new_health_restore) {}
void Item::set_mana_restore(int new_mana_restore) {}
void Item::set_level_requirement(int new_level_requirement) {}

//ITEM FUNCTIONS
void Item::display_details() const
{
	print(get_name() + ", " + get_type() + ", value : " + std::to_string(get_value()) + " gold");
	(get_is_magical()) ? print(", is magical") : print(", isn't magical");
}

void Item::display_properties() const
{
	if (!get_properties().empty())
	{
		print("properties :\n");
		for (const auto& v : get_properties())
		{
			print(v + " ");
		}
		std::cout << "\n";
	}
	else
	{
		print("no propery\n");
	}
}

//CONSTRUCTOR
Item::Item(std::string new_name, std::string new_type, int new_value, bool new_magical, std::vector<std::string> new_properties)
	: name(new_name), type(new_type), properties(new_properties)
{
	set_value(new_value);
	set_magical(new_magical);
}

//WEAPON

//GETTERS AND SETTERS
const int& Weapon::get_dmg_dice() const { return dmg_dice; }
const int& Weapon::get_dmg_bonus() const { return dmg_bonus; }
const int& Weapon::get_dmg_count() const { return dmg_dice_count; }
const float& Weapon::get_range() const { return range; }

void Weapon::set_dmg_dice(int D)
{
	if (D == 4 || D == 6 || D == 8 || D == 10 || D == 12 || D == 20 || D == 100)
		dmg_dice = D;
	else
		throw std::invalid_argument("invalid dice type entered (4, 6, 8, 10, 12 ,20 ,100)");
}
void Weapon::set_dmg_bonus(int D)
{
	if (D > -1)
		dmg_bonus = D;
	else
		throw std::invalid_argument("invalid damage bonus entered (no negative)");
}
void Weapon::set_dmg_count(int D)
{
	if (D > 0)
		dmg_dice_count = D;
	else
		throw std::invalid_argument("invalid dice count entered (positive only)");
}
void Weapon::set_range(float R)
{
	if (R > 0)
		range = R;
	else
		throw std::invalid_argument("invalid range entered (positive only)");
}

//CLASS CHILD TYPE
Item_Type Weapon::child_type() const { return Item_Type::Weapon; }

//FUNCTIONS
void Weapon::display_details() const
{
	Item::display_details();
	print(", " + std::to_string(get_dmg_count()) + " D" + std::to_string(get_dmg_dice()) +
		", dmg bonus of " + std::to_string(get_dmg_bonus()) +
		", range : " + std::to_string(get_range()) + " meters\n");
}

//CONSTRUCTOR
Weapon::Weapon(std::string new_name, std::string new_type, int new_value, bool new_magical,
	std::vector<std::string> new_properties, int new_dice, int new_bonus,
	int new_dice_count, float new_range)
	: Item(new_name, new_type, new_value, new_magical, new_properties)
{
	set_dmg_dice(new_dice);
	set_dmg_bonus(new_bonus);
	set_dmg_count(new_dice_count);
	set_range(new_range);
}

//SPELL_SCROLL

//SPELL_SCROLL GETTERS AND SETTERS
const std::string& Spell_Scroll::get_element() const { return element; }
const int& Spell_Scroll::get_mana_cost() const { return mana_cost; }

void Spell_Scroll::set_element(std::string new_element) { element = new_element; }
void Spell_Scroll::set_mana_cost(int new_mana_cost)
{
	if (new_mana_cost > -1)
		mana_cost = new_mana_cost;

	else
		throw std::invalid_argument("invalid mana cost entered (no negatives)");
}

//CLASS CHILD TYPE
Item_Type Spell_Scroll::child_type() const { return Item_Type::Spell_Scroll; }

//FUNCTIONS
void Spell_Scroll::display_details() const
{
	Item::display_details();
	print(", " + std::to_string(get_dmg_count()) + " D" + std::to_string(get_dmg_dice()) +
		", dmg bonus of " + std::to_string(get_dmg_bonus()) +
		", range : " + std::to_string(get_range()) + " meters " +
		", element : " + get_element() +
		", mana cost : " + std::to_string(get_mana_cost()) + "\n");
}

//CONSTRUCTOR
Spell_Scroll::Spell_Scroll(std::string new_name, std::string new_type, int new_value, bool new_magical,
	std::vector<std::string> new_properties, int new_dice, int new_bonus,
	int new_dice_count, float new_range, std::string new_element, int new_mana_cost)
	: Weapon(new_name, new_type, new_value, new_magical, new_properties,
		new_dice, new_bonus, new_dice_count, new_range), element(new_element)
{
	set_mana_cost(new_mana_cost);
}

//ARMOR

//ARMOR GETTERS AND SETTERS
const int& Armor::get_armor_class() const { return armor_class; }
const int& Armor::get_dex_bonus() const { return max_dex_bonus; }
const bool& Armor::get_stealth_friendly() const { return stealth_friendly; }

void Armor::set_armor_class(int new_ac)
{
	if (new_ac > 0)
		armor_class = new_ac;
	else
		throw std::invalid_argument("invalid armor class entered (positive only)");
}
void Armor::set_dex_bonus(int new_dex_bonus)
{
	if (new_dex_bonus > -1)
		max_dex_bonus = new_dex_bonus;
	else
		throw std::invalid_argument("invalid dex bonus entered (no negative)");
}

void Armor::set_stealth_friendly(bool new_stealth) { stealth_friendly = new_stealth; }

//CLASS CHILD TYPE 
Item_Type Armor::child_type() const { return Item_Type::Armor; }

//FUNCTIONS
void Armor::display_details() const
{
	Item::display_details();
	print(", AC : " + std::to_string(get_armor_class()) +
		", max dex bonus : " + std::to_string(get_dex_bonus()));
	(get_stealth_friendly()) ? print(", stealth friendly\n") : print(", not stealth friendly\n");
}

//CONSTRUCTOR
Armor::Armor(std::string new_name, std::string new_type, int new_value, bool new_magical,
	std::vector<std::string> new_properties, int new_ac, int new_dex_bonus, bool new_stealth)
	: Item(new_name, new_type, new_value, new_magical, new_properties)
{
	set_armor_class(new_ac);
	set_dex_bonus(new_dex_bonus);
	set_stealth_friendly(new_stealth);
}

//CONSUMABLE

//CONSUMABLE GETTERS AND SETTERS
const int& Consumable::get_health_restore() const { return health_restore; }
const int& Consumable::get_mana_restore() const { return mana_restore; }
const int& Consumable::get_level_requirement() const { return level_requirement; }

void Consumable::set_health_restore(int new_health_restore) { health_restore = new_health_restore; }
void Consumable::set_mana_restore(int new_mana_restore) { mana_restore = new_mana_restore; }
void Consumable::set_level_requirement(int new_level_requirement)
{
	if (new_level_requirement > 0)
		level_requirement = new_level_requirement;

	else
		throw std::invalid_argument("invalid level requirement entered (positive only)");
}

//CLASS CHILD TYPE
Item_Type Consumable::child_type() const { return Item_Type::Consumable; }

//FUNCTIONS
void Consumable::display_details() const
{
	Item::display_details();
	print(", health restore amount : " + std::to_string(get_health_restore()) +
		", mana restore amount : " + std::to_string(get_mana_restore()) +
		", level requirement : " + std::to_string(get_level_requirement()) + "\n");
}

//CONSTRUCTOR
Consumable::Consumable(std::string new_name, std::string new_type, int new_value, bool new_magical, std::vector<std::string> new_properties,
	int new_health_restore, int new_mana_restore, int new_level_requirement) : Item(new_name, new_type, new_value, new_magical, new_properties),
	health_restore(new_health_restore), mana_restore(new_mana_restore)
{
	set_level_requirement(new_level_requirement);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//ENTITY

//ENTITY GETTERS AND SETTERS
const std::string& Entity::get_name() const { return name; }
const std::string& Entity::get_background() const { return background; }
const std::string& Entity::get_class_type() const { return class_type; }
const Attributes& Entity::get_stats() const { return stats; }
const int& Entity::get_level() const { return level; }
const int& Entity::get_xp() const { return xp; }
const int& Entity::get_max_hp() const { return max_hp; }
const int& Entity::get_current_hp() const { return current_hp; }
const int& Entity::get_max_mana() const { return max_mana; }
const int& Entity::get_current_mana() const { return current_mana; }
const int& Entity::get_armorclass() const { return armorclass; }
const int& Entity::get_gold() const { return gold; }
const int& Entity::get_current_locaion_id() const { return current_location_id; }
const bool& Entity::get_is_player() const { return is_player; }
const bool& Entity::get_is_dead() const { return is_dead; }
const bool& Entity::get_dirty() const { return dirty; }
const std::vector<int>& Entity::get_completed_quests() const { return completed_quests; }
const std::vector<int>& Entity::get_ongoing_quests() const { return ongoing_quests; }
const std::unordered_map<int, Owned_Items>& Entity::get_inventory() const { return character_inventory; }
const std::pair<int, std::weak_ptr<Item>>& Entity::get_equipped_weapon() const { return equipped_weapon; }
const std::pair<int, std::weak_ptr<Item>>& Entity::get_equipped_armor() const { return equipped_armor; }

void Entity::set_name(std::string new_name) { name = new_name; dirty = true; }
void Entity::set_background(std::string new_background) { background = new_background; dirty = true; }
void Entity::set_class_type(std::string new_class_type) { class_type = new_class_type; dirty = true; }
void Entity::set_stats(Attributes new_stats)
{
	if (in_range(new_stats.cha_, 1, 20) &&
		in_range(new_stats.cons_, 1, 20) &&
		in_range(new_stats.dex_, 1, 20) &&
		in_range(new_stats.int_, 1, 20) &&
		in_range(new_stats.str_, 1, 20) &&
		in_range(new_stats.wis_, 1, 20))
	{
		stats = new_stats;
		dirty = true;
	}
	else
		throw std::invalid_argument("invalid stat (0–20)");
}
void Entity::set_level(int new_level)
{
	if (new_level > 0)
		level = new_level;
	else
		throw std::invalid_argument("invalid level entered (positive only)");

	dirty = true;
}
void Entity::set_xp(int new_xp)
{
	if (new_xp > -1)
		xp = new_xp;
	else
		throw std::invalid_argument("invalid xp entered (no negatives)");

	dirty = true;
}
void Entity::set_max_hp(int new_max_hp)
{
	if (new_max_hp > 0)
		max_hp = new_max_hp;
	else
		throw std::invalid_argument("invalid max hp entered (positive only)");

	dirty = true;
}
void Entity::set_current_hp(int new_current_hp)
{
	if (new_current_hp <= get_max_hp())
		current_hp = new_current_hp;
	else
		throw std::invalid_argument("invalid current hp: must be less than or equal to max hp");

	dirty = true;
}
void Entity::set_max_mana(int new_max_mana)
{
	if (new_max_mana > -1)
		max_mana = new_max_mana;
	else
		throw std::invalid_argument("invalid max mana entered (no negatives)");

	dirty = true;
}
void Entity::set_current_mana(int new_current_mana)
{
	if (new_current_mana <= get_max_mana() && new_current_mana > -1)
		current_mana = new_current_mana;
	else
		throw std::invalid_argument("invalid current mana : must be less than or equal to max mana and positive only");

	dirty = true;
}
void Entity::set_armorclass(int new_armorclass)
{
	if (new_armorclass > -1)
		armorclass = new_armorclass;
	else
		throw std::invalid_argument("invalid armor class entered (no negatives)");

	dirty = true;
}
void Entity::set_gold(int new_gold)
{
	if (new_gold > -1)
		gold = new_gold;
	else
		throw std::invalid_argument("invalid gold enterted (no negatives)");

	dirty = true;
}
void Entity::set_current_location_id(int new_location_id) { current_location_id = new_location_id; dirty = true; }
void Entity::set_is_player(bool new_is_player) { is_player = new_is_player; dirty = true; }
void Entity::set_is_dead(bool new_is_dead) { is_dead = new_is_dead; dirty = true; }
void Entity::set_dirty(bool new_dirty) { dirty = new_dirty; }
void Entity::add_completed_quest(int new_completed_quest) { completed_quests.push_back(new_completed_quest); dirty = true; }
void Entity::remove_completed_quest(int new_id)
{
	auto it = std::find(completed_quests.begin(), completed_quests.end(), new_id);

	if (it != completed_quests.end())
	{
		completed_quests.erase(it);
		print("completed quest removed\n");
		dirty = true;
	}
	else
		print("completed quest id not found\n");
}
void Entity::add_ongoing_quest(int new_ongoing_quest) { ongoing_quests.push_back(new_ongoing_quest); dirty = true; }
void Entity::remove_ongoing_quest(int new_id)
{
	auto it = std::find(ongoing_quests.begin(), ongoing_quests.end(), new_id);

	if (it != ongoing_quests.end())
	{
		ongoing_quests.erase(it);
		print("ongoing quest removed\n");
		dirty = true;
	}
	else
		print("ongoing quest id not found\n");
}
void Entity::set_equipped_weapon(std::pair<int, std::weak_ptr<Item>> new_equipped_weapon) { equipped_weapon = new_equipped_weapon; dirty = true; }
void Entity::set_equipped_armor(std::pair<int, std::weak_ptr<Item>> new_equipped_armor) { equipped_armor = new_equipped_armor; dirty = true; }

//FUNCTIONS
int Entity::get_proficiency_bonus(int new_level) const
{
	if (in_range(new_level, 1, 4))
		return 2;

	if (in_range(new_level, 5, 8))
		return 3;

	if (in_range(new_level, 9, 12))
		return 4;

	if (in_range(new_level, 13, 16))
		return 5;

	if (in_range(new_level, 17, 20))
		return 6;

	return 0;
}

//DISPLAYS
void Entity::display_stats() const
{
	print("strength : " + std::to_string(get_stats().str_) +
		", dexterity : " + std::to_string(get_stats().dex_) +
		", constitution : " + std::to_string(get_stats().cons_) +
		", intelligence : " + std::to_string(get_stats().int_) +
		", wisdom : " + std::to_string(get_stats().wis_) +
		", charisma : " + std::to_string(get_stats().cha_) + "\n");
}

void Entity::display_info() const
{
	print(get_name() + ", " + get_class_type() + ", " + get_background() + ", xp : " + std::to_string(get_xp()) +
		", level : " + std::to_string(get_level()) + ", hp : " + std::to_string(get_current_hp()) + " / " + std::to_string(get_max_hp()) +
		", mana : " + std::to_string(get_current_mana()) + " / " + std::to_string(get_max_mana()) +
		", AC : " + std::to_string(get_armorclass()) + ", gold : " + std::to_string(get_gold()));

	get_is_player() ? print(", is a player") : print(", is not a player");
	get_is_dead() ? print(", is dead") : print(", is not dead");

	std::cout << '\n';
}

void Entity::display_inventory()
{
	if (!get_inventory().empty())
	{
		print(std::to_string(character_inventory.size()) + " items in " + get_name() + "'s inventory\n");
		for (auto it : character_inventory)
		{
			if (auto p = it.second.item.lock())
			{
				print(std::to_string(it.first) + " ");
				p->display_details();
				p->display_properties();
			}
			else
				throw std::out_of_range("weak_ptr failed");
		}
	}
	else
		print("character inventory is empty\n");
}

//EQUIP AND DE EQUIP
void Entity::equip_item()
{
	std::vector<int> items;
	std::string choice = input<std::string>("please enter the name of the item of choice to equip : ");
	int item_choice{ -1 }, i{ 1 };
	for (const auto& it : get_inventory()) //checks for all items in inventory with the name given
	{
		if (std::shared_ptr<Item> p = it.second.item.lock())
		{
			if (p->get_name() == choice && (p->child_type() == Item_Type::Armor || p->child_type() == Item_Type::Weapon))
				items.push_back(it.first);
		}
		else
			throw std::out_of_range("weak_ptr failed");
	}

	if (items.size() == 0) //no item
		print("no item with this name exists\n");

	else if (items.size() == 1) //1 item
	{
		if (auto o = character_inventory[items[0]].item.lock())
		{
			if (o->child_type() == Item_Type::Weapon) //equiping weapons
			{
				set_equipped_weapon(std::make_pair(items[0], character_inventory[items[0]].item));
				if (auto p = get_equipped_weapon().second.lock())
				{
					print("equipped weapon : \n");
					print(std::to_string(items[0]) + " ");
					p->display_details();
					p->display_properties();
				}
				else
					throw std::out_of_range("weak_ptr failed");
			}
			else if (o->child_type() == Item_Type::Armor)//equiping armor
			{
				set_equipped_armor(std::make_pair(items[0], character_inventory[items[0]].item));
				if (auto p = get_equipped_armor().second.lock())
				{
					print("equipped armor : \n");
					print(std::to_string(items[0]) + " ");
					p->display_details();
					p->display_properties();
					update_armor_class();
					print("current AC : " + std::to_string(get_armorclass()) + "\n");
				}
				else
					throw std::out_of_range("weak_ptr failed");
			}
			else
				throw std::invalid_argument("not a weapon or an armor");
		}
	}
	else if (items.size() > 1) //more than 1 item
	{
		print("found " + std::to_string(items.size()) + " items with name " + choice + "\n");

		for (int v : items) //cycle through the found items
		{
			if (auto p = character_inventory[v].item.lock())
			{
				print(std::to_string(i) + ". " + std::to_string(v) + " ");
				p->display_details();
			}
			else
				throw std::out_of_range("weak_ptr failed");

			i++;
		}

		do
		{
			item_choice = input<int>("choose which item : ");

			if (item_choice > (int)items.size() || item_choice < 1)
				print("invalid choice entered\n");

		} while (item_choice > (int)items.size() || item_choice < 1);

		if (auto o = character_inventory[items[item_choice - 1]].item.lock())
		{
			if (o->child_type() == Item_Type::Weapon)
			{
				set_equipped_weapon(std::make_pair(items[item_choice - 1], character_inventory[items[item_choice - 1]].item)); //use item_choice
				if (auto p = get_equipped_weapon().second.lock())
				{
					print("equipped weapon : \n");
					print(std::to_string(items[item_choice - 1]) + " ");
					p->display_details();
					p->display_properties();
				}
				else
					throw std::out_of_range("weak_ptr failed");
			}
			else if (o->child_type() == Item_Type::Armor)
			{
				set_equipped_armor(std::make_pair(items[item_choice - 1], character_inventory[items[item_choice - 1]].item)); //use item_choice
				if (auto p = get_equipped_armor().second.lock())
				{
					print("equipped armor : \n");
					print(std::to_string(items[item_choice - 1]) + " ");
					p->display_details();
					p->display_properties();
					update_armor_class();
					print("current AC : " + std::to_string(get_armorclass()) + "\n");
				}
				else
					throw std::out_of_range("weak_ptr failed");
			}
			else
				throw std::invalid_argument("not a weapon or an armor");
		}
	}
}

void Entity::de_equip_item(std::string weapon_or_armor)
{
	if (weapon_or_armor == "weapon")
		set_equipped_weapon(std::make_pair(-1, std::weak_ptr<Item> {}));

	else if (weapon_or_armor == "armor")
	{
		set_equipped_armor(std::make_pair(-1, std::weak_ptr<Item> {}));
		update_armor_class();
	}

	else
		throw std::invalid_argument("invalid chioce for weapon or armor entered");

	print("item de equipped\n");
}

void Entity::set_equipped_weapon_internal(std::pair<int, std::weak_ptr<Item>> new_weapon) { equipped_weapon = new_weapon; dirty = true; }

void Entity::set_equipped_armor_internal(std::pair<int, std::weak_ptr<Item>> new_armor) { equipped_armor = new_armor; dirty = true; }

//ADD AND REMOVE INTO AND FROM INVENTORY
void Entity::add_item_to_inventory(int item_id, const Owned_Items& new_item)
{
	if (new_item.quantity <= 0)
		throw std::invalid_argument("invalid quantity entered (positive only)");

	auto it = character_inventory.find(item_id);

	if (it != character_inventory.end())
	{
		// Item already exists → increase quantity
		it->second.quantity += new_item.quantity;
	}
	else
	{
		// New entry
		character_inventory.emplace(item_id, new_item);
	}

	dirty = true;
}

void Entity::remove_item_from_inventory(int id)
{
	if (character_inventory.erase(id))
	{
		print("entire item removed from inventory\n");

		if (get_equipped_armor().first == id)
			de_equip_item("armor");

		else if (get_equipped_weapon().first == id)
			de_equip_item("weapon");

		dirty = true;
	}
	else
		print("item not found in inventory\n");
}

void Entity::remove_item_from_inventory(int id, int count)
{
	if (count > character_inventory.at(id).quantity)
		throw std::invalid_argument("invalid count entered must be less than or equal of current quantity");

	if (auto p = character_inventory.at(id).item.lock())
	{
		if (count != character_inventory.at(id).quantity)
		{
			character_inventory.at(id).quantity -= count;
			print("removed " + std::to_string(count) + " instances of item(" + p->get_name() + ") from " + get_name() + "'s inventory\n");
			p->display_details();
		}
		else
		{
			character_inventory.erase(id);
			print("item(" + p->get_name() + ") fully removed from " + get_name() + "'s inventory\n");

			if (get_equipped_armor().first == id)
				de_equip_item("armor");

			else if (get_equipped_weapon().first == id)
				de_equip_item("weapon");
		}

		dirty = true;
	}
	else
		throw std::out_of_range("weak_ptr failed");
}

//XP AND LEVEL UP + STATS
void Entity::stats_upgrade(int points)
{
	print_line();

	std::string stat_choice;
	int point_choice, new_points;
	Attributes new_stats;

	while (points != 0)
	{
		if (get_stats().cha_ == 20 &&
			get_stats().cons_ == 20 &&
			get_stats().dex_ == 20 &&
			get_stats().int_ == 20 &&
			get_stats().str_ == 20 &&
			get_stats().wis_ == 20)
		{
			print("stats are at maximum\n");
			break;
		}

		print("you have " + std::to_string(points) + " points \ncurrent stats : \n");
		display_stats();
		print("which stat do u wish to upgrade and by how much\n");
		stat_choice = input<std::string>("stat : ");
		point_choice = input<int>("point(s) : ");

		new_stats = get_stats();
		new_points = points;

		if (point_choice <= points)
		{
			if (stat_choice == "strength")
			{
				new_stats.str_ += point_choice;
				new_points -= point_choice;
			}
			else if (stat_choice == "dexterity")
			{
				new_stats.dex_ += point_choice;
				new_points -= point_choice;
			}
			else if (stat_choice == "constitution")
			{
				new_stats.cons_ += point_choice;
				new_points -= point_choice;
			}
			else if (stat_choice == "intelligence")
			{
				new_stats.int_ += point_choice;
				new_points -= point_choice;
			}
			else if (stat_choice == "wisdom")
			{
				new_stats.wis_ += point_choice;
				new_points -= point_choice;
			}
			else if (stat_choice == "charisma")
			{
				new_stats.cha_ += point_choice;
				new_points -= point_choice;
			}
			else
			{
				print("invalid stat type entered\n");
				continue;
			}

			try
			{
				set_stats(new_stats);
				points -= point_choice;
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		else
			print("invalid points entered must be less or equal than available points\n");
	}
	print("current stats : \n");
	display_stats();
}

void Entity::gain_xp(int new_xp)
{
	if (new_xp > -1)
		set_xp(get_xp() + new_xp);
	else
		throw std::invalid_argument("invalid xp reward entered (no negatives)");
}

void Entity::gain_gold(int new_gold)
{
	if (new_gold > -1)
		set_gold(get_gold() + new_gold);
	else
		throw std::invalid_argument("invalid gold reward entered (no negatives)");
}

void Entity::level_up()
{
	print_line();

	int levels_gained = 0;

	while (get_xp() >= 100 * pow(1.4, get_level() - 1))
	{
		set_level(get_level() + 1);

		int con_mod = get_stat_modifier(stats.cons_);
		int hp_gain = 8 + con_mod;  // base 8 + con mod
		if (hp_gain < 1) hp_gain = 1;
		set_max_hp(get_max_hp() + hp_gain);

		int int_mod = get_stat_modifier(stats.int_);
		int mana_gain = 6 + int_mod;  // base 6 + int mod
		if (mana_gain < 1) mana_gain = 1;
		set_max_mana(get_max_mana() + mana_gain);

		levels_gained++;
	}

	if (levels_gained > 0)
	{
		print(get_name() + "LEVELED UP " + std::to_string(levels_gained) + " time(s), current level : " + std::to_string(get_level()) + "\n", 5);
		stats_upgrade(levels_gained);
		set_current_hp(get_max_hp());
		set_current_mana(get_max_mana());
	}
	else
	{
		print(get_name() + "did not level up, not enough xp to level up, current xp : " + std::to_string(get_xp()) +
			" xp needed : " + std::to_string((int)(pow(get_level(), 1.5) * 100) - get_xp()) + "\n", 5);
	}

	update_armor_class();
}

//ITEM USAGE FROM INVENTORY
int Entity::attack() const
{
	int damage{ 0 };
	if (auto weapon = get_equipped_weapon().second.lock())
	{
		for (int i = 0; i < weapon->get_dmg_count(); i++)
		{
			damage += dice_roll(weapon->get_dmg_dice());
		}
		damage += weapon->get_dmg_bonus();
	}
	else
		throw std::out_of_range("no weapon equipped");

	return damage += get_stat_modifier(get_stats().str_);
}

void Entity::update_armor_class() {
	int new_ac;
	int dex = get_stats().dex_;
	int dex_mod = (dex - 10) / 2;  // D&D standard

	if (auto p = get_equipped_armor().second.lock()) 
	{
		// Armor equipped
		new_ac = p->get_armor_class();
		int dex_bonus = p->get_dex_bonus();

		if (dex_bonus > 0)
			new_ac += std::min(dex_mod, dex_bonus);  // Cap at max dex bonus
	}
	else 
	{
		// No armor equipped
		new_ac = 10 + dex_mod;
	}
	set_armorclass(new_ac);
}

int Entity::cast_spell(int id)
{
	int damage{ 0 };
	if (auto p = character_inventory.at(id).item.lock())
	{
		if (p->get_mana_cost() <= get_current_mana())
		{
			for (int i = 0; i < p->get_dmg_count(); i++)
			{
				damage += dice_roll(p->get_dmg_dice());
			}
			damage += p->get_dmg_bonus();

			set_current_mana(get_current_mana() - p->get_mana_cost());

			if (--character_inventory.at(id).quantity == 0)
			{
				character_inventory.erase(id);
				dirty = true;
				print("no more spell scroll of " + p->get_name() + " exists in inventory\n");
			}
			return damage += get_stat_modifier(get_stats().int_) + get_stat_modifier(get_stats().wis_);
		}
		else
			throw std::exception("not enough mana to cast this spell");
	}
	else
		throw std::out_of_range("weak_ptr failed");
}

void Entity::use_consumable(int id)
{
	if (auto p = character_inventory.at(id).item.lock())
	{
		if (p->get_level_requirement() <= get_level())
		{
			if (get_current_mana() == 0 && p->get_mana_restore() < 0)
				throw std::exception("current mana is 0, cant use this consumable");

			if ((get_current_hp() + p->get_health_restore()) >= get_max_hp())
				set_current_hp(get_max_hp());
			else if (get_current_hp() + p->get_health_restore() <= 0)
			{
				set_current_hp(0);
				update_death();
			}
			else
				set_current_hp(get_current_hp() + p->get_health_restore());

			if ((get_current_mana() + p->get_mana_restore()) >= get_max_mana())
				set_current_mana(get_max_mana());
			else if (get_current_mana() + p->get_mana_restore() >= 0)
				set_current_mana(0);
			else
				set_current_mana(get_current_mana() + p->get_mana_restore());

			if (--character_inventory.at(id).quantity == 0)
			{
				character_inventory.erase(id);
				dirty = true;
				print("no more consumable of " + p->get_name() + " exists in inventory\n");
			}
		}
		else
			throw std::exception("your level it too low to use this item");
	}
	else
		throw std::out_of_range("weak_ptr failed");
}

void Entity::update_death()
{
	if (get_current_hp() <= 0)
	{
		set_is_dead(true);
		print(get_name() + " died\n");
	}
}

//CONSTRUCTOR
Entity::Entity(std::string new_name, std::string new_background, std::string new_class_type, Attributes new_stats, bool new_is_player)
	: name(new_name), background(new_background), class_type(new_class_type), stats(new_stats), is_player(new_is_player) {}

Entity::Entity(std::string new_name, std::string new_background, std::string new_class_type, Attributes new_stats, int new_level, int new_xp, int new_max_hp, int new_current_hp, 
	int new_max_mana, int new_current_mana, int new_armorclass, int new_gold, int new_current_location_id, bool new_is_player, bool new_is_dead)
	: name(new_name), background(new_background), class_type(new_class_type), stats(new_stats), is_player(new_is_player)
{
	set_level(new_level);
	set_xp(new_xp);
	set_max_hp(new_max_hp);
	set_current_hp(new_current_hp);
	set_max_mana(new_max_mana);
	set_current_mana(new_current_mana);
	set_armorclass(new_armorclass);
	set_gold(new_gold);
	set_current_location_id(new_current_location_id);
	set_is_dead(new_is_dead);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//REWARDS

//REWARDS GETTERS AND SETTERS
const std::unordered_map<int, Owned_Items>& Rewards::get_reward_item_pointers() const { return reward_item_pointers; }
const int& Rewards::get_gold() const { return gold; }
const int& Rewards::get_xp() const { return xp; }

void Rewards::set_reward_item_pointers(std::unordered_map<int, Owned_Items> new_reward_item_pointers) { reward_item_pointers = new_reward_item_pointers; }
void Rewards::set_gold(int new_gold)
{
	if (new_gold > -1)
		gold = new_gold;
	else
		throw std::invalid_argument("invalid gold entered (no negatives)");
}
void Rewards::set_xp(int new_xp)
{
	if (new_xp > -1)
		xp = new_xp;
	else
		throw std::invalid_argument("invalid xp entered (no negatives)");
}

void Rewards::add_item_pointers(int item_id, const Owned_Items& new_item)
{
	// If the item already exists, increase quantity
	auto it = reward_item_pointers.find(item_id);

	if (it != reward_item_pointers.end())
	{
		it->second.quantity += new_item.quantity;
	}
	else
	{
		reward_item_pointers.emplace(item_id, new_item);
	}
}

void Rewards::remove_item_pointers(int item_id)
{
	auto it = reward_item_pointers.find(item_id);

	if (it == reward_item_pointers.end())
		throw std::invalid_argument("this item doesn't exist in the rewards list");

	reward_item_pointers.erase(it);
	print("item removed from reward");
}

void Rewards::remove_item_pointers(int item_id, int count)
{
	auto it = reward_item_pointers.find(item_id);
	if (it == reward_item_pointers.end())
		throw std::out_of_range("item id not found in reward pool");

	if (count > it->second.quantity)
		throw std::invalid_argument("invalid count: more than current quantity");

	if (count < it->second.quantity)
	{
		it->second.quantity -= count;
		print("removed " + std::to_string(count) + " instances of reward item\n");

		if (auto p = it->second.item.lock())
			p->display_details();
		else
			throw std::out_of_range("weak_ptr failed");

		return;
	}

	// count == full quantity → remove entire entry
	reward_item_pointers.erase(it);
	print("item fully removed from reward pool\n");
}

//FUNCTIONS
void Rewards::display_details() const
{
	print(std::to_string(gold) + " gold, " + std::to_string(xp) + " xp\n");

	for (const auto& owned : reward_item_pointers)
	{
		if (auto ptr = owned.second.item.lock())
		{
			ptr->display_details();
			print("Quantity: " + std::to_string(owned.second.quantity) + "\n");
		}
		else
			throw std::out_of_range("weak_ptr failed (expired item in rewards)");
	}
}


//CONSTRUCTOR
Rewards::Rewards(int new_gold, int new_xp)
{
	set_gold(new_gold);
	set_xp(new_xp);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

// LOCATION

//LOCATION GETTERS AND SETTERS
const std::string& Location::get_name() const { return name; }
const std::vector<std::string>& Location::get_properties() const { return properties; }
const std::vector<int>& Location::get_connected_location_ids() const { return connected_location_ids; }
const std::weak_ptr<Rewards>& Location::get_chest() const { return chest; }

void Location::set_name(std::string new_name) { name = new_name; }
void Location::set_properties(std::vector<std::string> new_properties) { properties = new_properties; }
void Location::add_property(std::string new_property) { properties.push_back(new_property); }
void Location::remove_property(std::string new_property)
{
	auto it = std::find(properties.begin(), properties.end(), new_property);

	if (it != properties.end())
	{
		properties.erase(it);
		print("property deleted\n");
	}
	else
		print("property not found\n");
}
void Location::set_connected_location_ids(std::vector<int> new_connected_location_ids) { connected_location_ids = new_connected_location_ids; }
void Location::add_connected_location_id(int new_location_id) { connected_location_ids.push_back(new_location_id); }
void Location::remove_connected_location_id(int new_location_id)
{
	auto it = find(connected_location_ids.begin(), connected_location_ids.end(), new_location_id);

	if (it != get_connected_location_ids().end())
		connected_location_ids.erase(it);
	else
		throw std::out_of_range("location id doesnt exist in connected location ids list");
}
void Location::set_chest(std::weak_ptr<Rewards> new_chest) { chest = new_chest; }
void Location::remove_chest()
{
	chest = std::weak_ptr<Rewards>{};
	print("reward removed\n");
}

//FUNCTIONS
void Location::display_info() const
{
	print(get_name() + "\n");

	if (!get_properties().empty())
	{
		print("properties : ");

		for (const auto& v : get_properties())
		{
			print(v + ", ");
		}
		std::cout << "\n";
	}

	if (get_connected_location_ids().empty())
		print("no location connected\n");

	else
	{
		print("connected location ids: ");

		for (const auto& v : get_connected_location_ids())
		{
			print(std::to_string(v) + ", ");
		}
		std::cout << "\n";
	}

	if (auto p = chest.lock())
	{
		p->display_details();
	}
	else
		print("no chest\n");
}

//CONSTRUCTOR
Location::Location(std::string new_name, std::vector<std::string> new_properties) : name(new_name), properties(new_properties) {}

/////////////////////////////////////////////////////////////////////////////////////////////////////

// QUEST

//QUEST GETTERS AND SETTERS
const std::string& Quest::get_name() const { return name; }
const std::string& Quest::get_description() const { return description; }
const std::string& Quest::get_quest_giver() const { return quest_giver; }
const int& Quest::get_quest_level() const { return quest_level; }
const int& Quest::get_priority() const { return priority; }
const std::weak_ptr<Rewards>& Quest::get_reward() const { return reward; }
const std::unordered_map<int, Enemies>& Quest::get_enemies() const { return enemies; }

void Quest::set_name(std::string new_name) { name = new_name; }
void Quest::set_description(std::string new_description) { description = new_description; }
void Quest::set_quest_giver(std::string new_quest_giver) { quest_giver = new_quest_giver; }
void Quest::set_quest_level(int new_quest_level)
{
	if (new_quest_level > 0)
		quest_level = new_quest_level;
	else
		throw std::invalid_argument("invalid quest level entered (positive only)");
}
void Quest::set_priority(int new_priority)
{
	if (new_priority > 0)
		priority = new_priority;
	else
		throw std::invalid_argument("invalid quest priority entered (positive only)");
}
void Quest::set_reward(std::weak_ptr<Rewards> new_reward) { reward = new_reward; }
void Quest::remove_reward() 
{
	reward = std::weak_ptr<Rewards>{};
	print("reward deleted\n");
}
void Quest::set_enemies(std::unordered_map<int, Enemies> new_enemies) { enemies = new_enemies; }
bool Quest::add_enemy(int new_enemy_id, const Enemies& new_enemy)
{
	if (new_enemy.quantity > 0)
	{
		if (enemies.find(new_enemy_id) == enemies.end())
		{
			enemies[new_enemy_id] = new_enemy;
			return true;
		}

		else
		{
			enemies.at(new_enemy_id).quantity += new_enemy.quantity;
			return false;
		}
	}

	else
		throw std::invalid_argument("invalid quantity enterd");
}
void Quest::remove_enemy(int id)
{
	if (enemies.erase(id))
		print("enemy removed from quest\n");

	else
		print("enemy not found in quest\n");
}
void Quest::remove_enemy(int id, int quantity)
{
	if (quantity > enemies.at(id).quantity)
		throw std::invalid_argument("invalid count entered must be less than or equal of current quantity");

	if (auto p = enemies.at(id).enemy_ptr.lock())
	{
		if (quantity != enemies.at(id).quantity)
		{
			enemies.at(id).quantity -= quantity;
			print("removed " + std::to_string(quantity) + " instances of enemy\n");
			p->display_info();
		}
		else
		{
			enemies.erase(id);
			print("enemy removed from quest\n");
		}
	}
	else
		throw std::out_of_range("weak_ptr failed");
}

//FUNCTIONS
void Quest::display_info_without_enemies() const
{
	print(get_name() + ", description : " + get_description() + ", quest giver : " + get_quest_giver() +
		", quest level : " + std::to_string(get_quest_level()) + ", priority : " + std::to_string(get_priority()) + '\n');
}

void Quest::display_info() const
{
	print(get_name() + ", description : " + get_description() + ", quest giver : " + get_quest_giver() +
		", quest level : " + std::to_string(get_quest_level()) + ", priority : " + std::to_string(get_priority()) + '\n');

	if (auto p = reward.lock())
	{
		print("reward : ");
		p->display_details();
	}
	else
		print("no reward\n");

	if (enemies.empty())
		print("no enemies\n");
	else
	{
		for (const auto& v : enemies)
		{
			if (auto p = v.second.enemy_ptr.lock())
			{
				p->display_info();
				print("quantity : " + std::to_string(v.second.quantity) + "\n");
			}
			else
				throw std::runtime_error("weak_ptr failed");
		}
	}
}

//CONSTRUCTOR
Quest::Quest(std::string new_name, std::string new_description, std::string new_quest_giver, int new_quest_level, int new_priority) :
	name(new_name), description(new_description), quest_giver(new_quest_giver)
{
	set_quest_level(new_quest_level);
	set_priority(new_priority);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

// BATTLE ENTITY

//BATTLE ENTITY GETTERS AND SETTERS
const int Battle_Entity::get_original_id() const { return original_id; }
const int Battle_Entity::get_x_axis() const { return x_axis; }
const int Battle_Entity::get_y_axis() const { return y_axis; }
const int Battle_Entity::get_x_max() const { return x_max; }
const int Battle_Entity::get_y_max() const { return y_max; }
const std::shared_ptr<Entity>& Battle_Entity::get_entity() const { return entity; }
const bool Battle_Entity::get_is_party_member() const { return is_party_member; }
const Battle_Entity_Status Battle_Entity::get_status() const { return status; }

void Battle_Entity::set_original_id(int new_original_id) { original_id = new_original_id; }
void Battle_Entity::set_x_axis(int new_x_axis)
{
	if (new_x_axis >= x_max || new_x_axis < 0)
		throw std::invalid_argument("invalid x_axis entered");

	x_axis = new_x_axis;
}
void Battle_Entity::set_y_axis(int new_y_axis)
{
	if (new_y_axis >= y_max || new_y_axis < 0)
		throw std::invalid_argument("invalid y_axis entered");

	y_axis = new_y_axis;
}
void Battle_Entity::set_x_max(int new_x_max) 
{
	if (new_x_max <= 0)
		throw std::invalid_argument("invalid x_max enetered (positive only)");

	x_max = new_x_max;
}
void Battle_Entity::set_y_max(int new_y_max)
{
	if (new_y_max <= 0)
		throw std::invalid_argument("invalid y_max enetered (positive only)");

	y_max = new_y_max;
}
void Battle_Entity::set_entity(std::weak_ptr<Entity> new_entity)
{
	if (auto p = new_entity.lock())
	{
		if (p->get_is_player())
			entity = p;
		
		else
			entity = std::make_shared<Entity>(*p);
		
	}
	else
		throw std::runtime_error("original entity no longer exists");
	
}
void Battle_Entity::set_is_party_member(bool new_is_party_member) { is_party_member = new_is_party_member; }
void Battle_Entity::set_status(Battle_Entity_Status new_status) { status = new_status; }

//CONSTRUCTOR
Battle_Entity::Battle_Entity(int new_original_id, std::weak_ptr<Entity> new_entity) : original_id(new_original_id)
{
	set_entity(new_entity);
	set_is_party_member(get_entity()->get_is_player());
}