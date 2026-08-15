#pragma once
#include "rok.h"

struct Navigation {
	ROK::Account* activeAccount = nullptr;
	ROK::Character* activeCharacter = nullptr;
};