#pragma once

#include "Common.h"
#include "UnitType.h"

class IDABot;
class Unit;

namespace Util
{
    CCRace          GetRaceFromString(const std::string & str);
    CCTilePosition  GetTilePosition(const CCPosition & pos);
    CCPosition      GetPosition(const CCTilePosition & tile);
    std::string     GetStringFromRace(const CCRace & race);
    bool            UnitCanMetaTypeNow(const Unit & unit, const UnitType & type, IDABot & m_bot);
    UnitType        GetTownHall(const CCRace & race, IDABot & bot);
    UnitType        GetRefinery(const CCRace & race, IDABot & bot);
    UnitType        GetSupplyProvider(const CCRace & race, IDABot & bot);
    CCPosition      CalcCenter(const std::vector<Unit> & units);
    bool            IsZerg(const CCRace & race);
    bool            IsProtoss(const CCRace & race);
    bool            IsTerran(const CCRace & race);
    CCPositionType  TileToPosition(float tile);

#ifdef SC2API
    sc2::BuffID     GetBuffFromName(const std::string & name, IDABot & bot);
    sc2::AbilityID  GetAbilityFromName(const std::string & name, IDABot & bot);
#endif

    float Dist(const Unit & unit, const CCPosition & p2);
    float Dist(const Unit & unit1, const Unit & unit2);
    float Dist(const CCPosition & p1, const CCPosition & p2);
    CCPositionType DistSq(const CCPosition & p1, const CCPosition & p2);
};
