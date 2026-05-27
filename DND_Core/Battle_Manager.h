#pragma once

#include <memory>

#include "Enums.h"

class Party;
class Quest;

struct Entity_Manager_Lambda;

class Battle_Manager
{
private:
	std::weak_ptr<Party> party;
	std::weak_ptr<Quest> quest;
	Battle_Type battle_type;

public:

};
