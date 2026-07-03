#pragma once

enum class Item_Type
{
	Weapon, Armor, Spell_Scroll, Consumable
};

enum class Container_Type
{
	Item, Entity, Reward, Location, Quest
};

enum class Sub_Container_Type
{
	Connected_Location, Inventory, Completed_And_Ongoing_quest, Reward_Item, Quest_Enemy
};

enum class Party_State
{
	Resting, Party_Dead
};

enum class Battle_Type
{
	Quest, Random_Encounter
};

enum class Battle_Entity_Status
{
	Alive, Dead, Deserted, Won
};