#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <type_traits>
#include <array>

#include "Enums.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function to throw an attack roll to see if its a hit or no
bool is_it_a_hit(int target_ac, int str_modifier, int proficiency_bonus);

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function to check for id duplicates in an std::array used primarily in Party class
void check_for_duplicates(std::array<int, 4> ids);

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function to trim trailing whitespaces in a string gotten from input function 
std::string trim_trailing_whitespace(const std::string& str);

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function for checking user input for deletion/removal
char to_upper(char c);

/////////////////////////////////////////////////////////////////////////////////////////////////////

//check for values being in a range
bool in_range(int value, int min, int max);

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function for rolling a dice with certain sides
int dice_roll(int sides);

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function for having a cool dramatic printing effect
void print(const std::string& text, int delay = 0 );

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function for printing a line to seperate menus and functions
void print_line(int delay = 0);

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function to get input and make sure its the intented type 
template <typename T>
T input(const std::string& prompt, int delay = 0)
{
	T variable;
	try
	{
        if constexpr (std::is_same_v<T, std::string>) 
        {
            print(prompt, delay);
            if (std::cin.peek() == '\n')
                std::cin.ignore();

            if (!(std::getline(std::cin, variable)))
            {
                std::cin.clear();
                throw std::runtime_error(std::string("invalid input type entered, expected : ") + typeid(T).name());
            }

            variable = trim_trailing_whitespace(variable);
        }
        else
        {
            print(prompt);
            if (!(std::cin >> variable))
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw std::runtime_error(std::string("invalid input type entered, expected : ") + typeid(T).name());
            }
        }
		return variable;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return input<T>(prompt);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

template<>
Item_Type input<Item_Type>(const std::string& prompt, int delay);

/////////////////////////////////////////////////////////////////////////////////////////////////////

//function to get an id from the user, it has two modes   
// 1.for creating a new instance in the list, so the provided id must not exist    
// 2.for acessing an instance in the list, so the provided id must exist
template <typename T>
int ask_id(const T& container, bool creation, std::function<void()> print_list, std::string prompt = "please enter id (integer, -1 to cancel, -2 to display list) : ", int delay = 0)
{
    if (container.empty() && !creation) throw std::runtime_error("container empty");

    while (true)
    {
        int id = input<int>(prompt, delay);

        if (id == -1) throw std::runtime_error("operation cancelled by user");  // Escape hatch

        if (id == -2)
        {
            print_list();
            print_line();
            continue;
        }

        if (id <= 0)
        {
            print("invalid id entered (positive only)\n");
            continue;
        }

        auto it = container.find(id);

        if (creation)
        {
            if (it != container.end())
            {
                print("invalid id entered (already exists)\n");
                continue;
            }
        }
        else
        {
            if (it == container.end())
            {
                print("invalid id entered (doesnt exist)\n");
                continue;
            }
        }

        return id;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/*
auto it = std::find_if(
		reward_item_pointers.begin(),
		reward_item_pointers.end(),
		[&](const std::weak_ptr<Item>& w)
		{
			return w.lock() == item_ptr.lock();
		});
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////