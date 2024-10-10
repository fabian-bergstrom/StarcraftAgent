#pragma once

#include "Common.h"


class IDABot;
class IDAReplayObserver;

class UnitType
{
    mutable sc2::Client * m_client;
	mutable IDABot * m_bot;
	mutable IDAReplayObserver * m_observer;

    sc2::UnitTypeID m_type;

public:

    UnitType();

    UnitType(const sc2::UnitTypeID & type, sc2::Client & client);
	UnitType(const sc2::UnitTypeID & type, sc2::Client & client, IDABot & m_bot);
	UnitType(const sc2::UnitTypeID & type, sc2::Client & client, IDAReplayObserver & observer);

    sc2::UnitTypeID getAPIUnitType() const;
    bool is(const sc2::UnitTypeID & type) const;

    bool operator < (const UnitType & rhs) const;
    bool operator == (const UnitType & rhs) const;

    std::string getName() const;
    CCRace getRace() const;
	
    int getMovementSpeed() const;
    int getSightRange() const;
	sc2::UnitTypeID getRequiredStructure() const;
    
    bool isValid() const;
    bool isBuilding() const;
    bool isCombatUnit() const;
    bool isSupplyProvider() const;
    bool isResourceDepot() const;
    bool isRefinery() const;
    bool isDetector() const;
    bool isGeyser() const;
    bool isMineral() const;
    bool isWorker() const;
    bool isMorphedBuilding() const;
    bool canAttack() const;
    bool canMove() const;
    bool isAddon() const;
	bool canAttackGound() const;
	bool canAttackAir() const;
    CCPositionType getAttackRange() const;
	float getAttackDamage() const;
    int tileWidth() const;
    int tileHeight() const;
    int supplyProvided() const;
    int supplyRequired() const;
    int mineralPrice() const;
    int gasPrice() const;

    const std::vector<UnitType> & whatBuilds() const;
	std::vector<sc2::UnitTypeID> getEquivalentUnits() const;
	bool requiredAttached() const;
	float getBuildTime() const;

    static UnitType GetUnitTypeFromName(const std::string & name, IDABot & bot);

    // lazy functions to let me know if this type is a special type
    bool isOverlord() const;
    bool isLarva() const;
    bool isEgg() const;
    bool isQueen() const;
    bool isTank() const;
};
