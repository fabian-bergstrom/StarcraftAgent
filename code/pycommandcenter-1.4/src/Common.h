#pragma once

#include "BotAssert.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <set>
#include <fstream>
#include <streambuf>
#include <string>
#include <array>

// Somewhere in the dependency graph someone defines a macro called max,
// which breaks all calls to std::limits::max
#define NOMINMAX

#include <sc2api/sc2_api.h>

typedef sc2::Point2D        CCPosition;
typedef sc2::Point2DI       CCTilePosition;
typedef sc2::Color          CCColor;
typedef sc2::UpgradeID      CCUpgrade;
typedef sc2::Tag            CCUnitID;
typedef sc2::Race           CCRace;
typedef float               CCHealth;
typedef float               CCPositionType;
typedef sc2::BuffID         CCBuff;
    
typedef size_t CCPlayer;

namespace Players
{
    enum {Self = 0u, Enemy = 1u, Neutral = 2u, Ally = 3u, Size = 4u, None = 5u};
}
