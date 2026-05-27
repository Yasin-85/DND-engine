#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <typeinfo>
#include <cctype>
#include <array>
#include <algorithm>

#include "Utils.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

void check_for_duplicates(std::array<int, 4> ids)
{
	std::array<int, 4> copy = ids;
	std::sort(copy.begin(), copy.end());
	auto it = std::adjacent_find(copy.begin(), copy.end());

	if (it != copy.end())
		throw std::invalid_argument("duplicate ids entered");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function to trim trailing whitespaces in string gotten from input function 
std::string trim_trailing_whitespace(const std::string& str) 
{
	auto end = str.find_last_not_of(" \t\n\r\f\v");
	if (end == std::string::npos)
		return "";  // All whitespace
	
	return str.substr(0, end + 1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function for checking user input for deletion/removal
char to_upper(char c) { return std::toupper(static_cast<unsigned char>(c)); }

/////////////////////////////////////////////////////////////////////////////////////////////////////

//check for values being in a range
bool in_range(int value, int min, int max)
{
	return value >= min && value <= max;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function for rolling a dice with certain sides
int dice_roll(int sides)
{
	static std::random_device rd;  // Non-deterministic seed
	static std::mt19937 gen(rd()); // Mersenne Twister
	std::uniform_int_distribution<> dist(1, sides);
	return dist(gen);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function for having a cool dramatic printing effect
void print(const std::string& text, int delay)
{
	for (char c : text)
	{
		std::cout << c << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function for printing a line to seperate menus and functions
void print_line(int delay)
{
	print("########################################\n", delay);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function to get input and make sure its the intented type 
template<>
Item_Type input<Item_Type>(const std::string& prompt, int delay)
{
	std::string s = input<std::string>(prompt);

	// lowercase for easy comparison
	for (char& c : s) c = std::tolower(c);

	if (s == "weapon")        return Item_Type::Weapon;
	if (s == "armor")         return Item_Type::Armor;
	if (s == "spell_scroll")  return Item_Type::Spell_Scroll;
	if (s == "consumable")    return Item_Type::Consumable;

	std::cerr << "Invalid item type. Valid types: weapon, armor, spell_scroll, consumable.\n";
	return input<Item_Type>(prompt);  // retry
}