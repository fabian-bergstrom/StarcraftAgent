#include "UnitType.h"
#include "IDABot.h"
#include "IDAReplayObserver.h"

UnitType::UnitType()
    : m_client(nullptr)
    , m_type(0)
{

}

UnitType::UnitType(const sc2::UnitTypeID & type, sc2::Client & client)
    : m_client(&client)
    , m_type(type)
	, m_bot(nullptr)
	, m_observer(nullptr)

{

}

UnitType::UnitType(const sc2::UnitTypeID & type, sc2::Client & client, IDABot & bot)
	: m_client(&client)
	, m_type(type)
	, m_bot(&bot)
	, m_observer(nullptr)
{

}

UnitType::UnitType(const sc2::UnitTypeID & type, sc2::Client & client, IDAReplayObserver & observer)
	: m_client(&client)
	, m_type(type)
	, m_observer(&observer)
	, m_bot(nullptr)

{

}

sc2::UnitTypeID UnitType::getAPIUnitType() const
{
    return m_type;
}

bool UnitType::is(const sc2::UnitTypeID & type) const
{
    return m_type == type;
}

bool UnitType::operator < (const UnitType & rhs) const
{
    return m_type < rhs.m_type;
}

bool UnitType::operator == (const UnitType & rhs) const
{
    return m_type == rhs.m_type;
}

bool UnitType::isValid() const
{
    return m_type != 0;
}

std::string UnitType::getName() const
{
    return sc2::UnitTypeToName(m_type);
}

CCRace UnitType::getRace() const
{
    return m_client->Observation()->GetUnitTypeData()[m_type].race;
}

bool UnitType::isCombatUnit() const
{
#ifdef SC2API
    if (isWorker()) { return false; }
    if (isSupplyProvider()) { return false; }
    if (isBuilding()) { return false; }

    if (isEgg() || isLarva()) { return false; }

    return true;
#else
    // check for various types of combat units
    if (m_type.canAttack() || 
        m_type == BWAPI::UnitTypes::Terran_Medic ||
        m_type == BWAPI::UnitTypes::Protoss_High_Templar ||
        m_type == BWAPI::UnitTypes::Protoss_Observer ||
        m_type == BWAPI::UnitTypes::Zerg_Overlord ||
        m_type == BWAPI::UnitTypes::Protoss_Observer)
    {
        return true;
    }

    // no workers or buildings allowed
    if (m_type.isWorker() || m_type.isBuilding())
    {
        return false;
    }

    return false;
#endif
}

bool UnitType::isSupplyProvider() const
{
    return (supplyProvided() > 0) && !isResourceDepot();
}

bool UnitType::isResourceDepot() const
{
#ifdef SC2API
    switch (m_type.ToType()) 
    {
        case sc2::UNIT_TYPEID::ZERG_HATCHERY                : return true;
        case sc2::UNIT_TYPEID::ZERG_LAIR                    : return true;
        case sc2::UNIT_TYPEID::ZERG_HIVE                    : return true;
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER         : return true;
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND        : return true;
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING  : return true;
        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS     : return true;
        case sc2::UNIT_TYPEID::PROTOSS_NEXUS                : return true;
        default: return false;
    }
#else
    return m_type.isResourceDepot();
#endif
}

bool UnitType::isRefinery() const
{
#ifdef SC2API
    switch (m_type.ToType()) 
    {
        case sc2::UNIT_TYPEID::TERRAN_REFINERY			: return true;
		case sc2::UNIT_TYPEID::TERRAN_REFINERYRICH		: return true;
        case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR		: return true;
		case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATORRICH  : return true;
        case sc2::UNIT_TYPEID::ZERG_EXTRACTOR			: return true;
		case sc2::UNIT_TYPEID::ZERG_EXTRACTORRICH		: return true;
        default: return false;
    }
#else
    return m_type.isRefinery();
#endif
}

bool UnitType::isDetector() const
{
#ifdef SC2API
    switch (m_type.ToType())
    {
        case sc2::UNIT_TYPEID::PROTOSS_OBSERVER        : return true;
        case sc2::UNIT_TYPEID::ZERG_OVERSEER           : return true;
        case sc2::UNIT_TYPEID::TERRAN_MISSILETURRET    : return true;
        case sc2::UNIT_TYPEID::ZERG_SPORECRAWLER       : return true;
        case sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON    : return true;
        case sc2::UNIT_TYPEID::TERRAN_RAVEN            : return true;
        default: return false;
    }
#else
    return m_type.isDetector();
#endif
}

bool UnitType::isGeyser() const
{
#ifdef SC2API
    switch (m_type.ToType()) 
    {
        case sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER        : return true;
        case sc2::UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER : return true;
        case sc2::UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER  : return true;
		case sc2::UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER: return true;
		case sc2::UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER	: return true;
		case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER: return true;

        default: return false;
    }
#else
    return m_type == BWAPI::UnitTypes::Resource_Vespene_Geyser;
#endif
}

bool UnitType::isMineral() const
{
#ifdef SC2API
    switch (m_type.ToType()) 
    {
        case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD				: return true;
        case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750			: return true;
        case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD			: return true;
        case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750		: return true;
		case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD			: return true;
		case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750		: return true;
		case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD		: return true;
		case sc2::UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD750	: return true;

        default: return false;
    }
#else
    return m_type.isMineralField();
#endif
}

bool UnitType::isWorker() const
{
#ifdef SC2API
    switch (m_type.ToType()) 
    {
        case sc2::UNIT_TYPEID::TERRAN_SCV           : return true;
        case sc2::UNIT_TYPEID::PROTOSS_PROBE        : return true;
        case sc2::UNIT_TYPEID::ZERG_DRONE           : return true;
        case sc2::UNIT_TYPEID::ZERG_DRONEBURROWED   : return true;
        default: return false;
    }
#else
    return m_type.isWorker();
#endif
}

bool UnitType::canAttackGound() const 
{
#ifdef SC2API
	auto & weapons = m_client->Observation()->GetUnitTypeData()[m_type].weapons;

	if (weapons.empty())
	{
		return false;
	}
	for (auto & weapon : weapons)
	{
		if (weapon.type == sc2::Weapon::TargetType::Ground || weapon.type == sc2::Weapon::TargetType::Any) 
		{
			return true;
		}
	}
	return false;
#else
	// TODO: this is nothing right now...
	// just like with the attackRange we should never get in here.
	return 0.0f;
#endif
}

bool UnitType::canAttackAir() const 
{
#ifdef SC2API
	auto & weapons = m_client->Observation()->GetUnitTypeData()[m_type].weapons;

	if (weapons.empty())
	{
		return false;
	}
	for (auto & weapon : weapons)
	{
		if (weapon.type == sc2::Weapon::TargetType::Air || weapon.type == sc2::Weapon::TargetType::Any)
		{
			return true;
		}
	}
	return false;
#else
	// TODO: this is nothing right now...
	// just like with the attackRange we should never get in here.
	return 0.0f;
#endif
}

CCPositionType UnitType::getAttackRange() const
{
#ifdef SC2API
    auto & weapons = m_client->Observation()->GetUnitTypeData()[m_type].weapons;
    
    if (weapons.empty())
    {
        return 0.0f;
    }

    float maxRange = 0.0f;
    for (auto & weapon : weapons)
    {
        if (weapon.range > maxRange)
        {
            maxRange = weapon.range;
        }
    }

    return maxRange;
#else
    // TODO: this is ground weapon range right now
    return m_type.groundWeapon().maxRange();
#endif
}

float UnitType::getAttackDamage() const
{
#ifdef SC2API
	auto & weapons = m_client->Observation()->GetUnitTypeData()[m_type].weapons;

	if (weapons.empty())
	{
		return 0.0f;
	}

	float maxDamage = 0.0f;
	for (auto & weapon : weapons)
	{
		if (weapon.range > maxDamage)
		{
			maxDamage = weapon.damage_;
		}
	}

	return maxDamage;
#else
	// TODO: this is nothing right now...
	// just like with the attackRange we should never get in here.
	return 0.0f;
#endif
}

int UnitType::tileWidth() const
{
#ifdef SC2API
    if (isMineral()) { return 2; }
    if (isGeyser()) { return 3; }
	else {
		if (m_bot != nullptr)
		{
			return (int)(2 * m_client->Observation()->GetAbilityData()[m_bot->Data(*this).buildAbility].footprint_radius);
		}
		else if (m_observer != nullptr)
		{
			return (int)(2 * m_client->Observation()->GetAbilityData()[m_observer->Data(*this).buildAbility].footprint_radius);
		}
		else
		{
			return -1;
		}
	}
#else
    return m_type.tileWidth();
#endif
}

int UnitType::tileHeight() const
{
#ifdef SC2API
    if (isMineral()) { return 1; }
    if (isGeyser()) { return 3; }
    else {
		if (m_bot != nullptr)
		{
			return (int)(2 * m_client->Observation()->GetAbilityData()[m_bot->Data(*this).buildAbility].footprint_radius);
		}
		else if (m_observer != nullptr)
		{
			return (int)(2 * m_client->Observation()->GetAbilityData()[m_observer->Data(*this).buildAbility].footprint_radius);
		}
		else
		{
			return -1;
		}
	}
#else
    return m_type.tileHeight();
#endif

}

bool UnitType::isAddon() const
{
#ifdef SC2API
	if (m_bot != nullptr)
	{
		return m_bot->Data(*this).isAddon;
	}
	else if (m_observer != nullptr)
	{
		return m_observer->Data(*this).isAddon;
	}
	else
	{
		return false;
	}
    
#else
    return m_type.isAddon();
#endif
}

bool UnitType::isBuilding() const
{
#ifdef SC2API
	if (m_bot != nullptr)
	{
		return m_bot->Data(*this).isBuilding;
	}
	else if (m_observer != nullptr)
	{
		return m_observer->Data(*this).isBuilding;
	}
	else 
	{
		return false;
	}
    
#else
    return m_type.isBuilding();
#endif
}

int UnitType::supplyProvided() const
{
#ifdef SC2API
    return (int)m_client->Observation()->GetUnitTypeData()[m_type].food_provided;
#else
    return m_type.supplyProvided();
#endif
}

int UnitType::supplyRequired() const
{
#ifdef SC2API
    return (int)m_client->Observation()->GetUnitTypeData()[m_type].food_required;
#else
    return m_type.supplyRequired();
#endif
}

int UnitType::mineralPrice() const
{
#ifdef SC2API
    return (int)m_client->Observation()->GetUnitTypeData()[m_type].mineral_cost;
#else
    return m_type.mineralPrice();
#endif
}

int UnitType::gasPrice() const
{
#ifdef SC2API
    return (int)m_client->Observation()->GetUnitTypeData()[m_type].vespene_cost;
#else
    return m_type.gasPrice();
#endif
}

UnitType UnitType::GetUnitTypeFromName(const std::string & name, IDABot & bot)
{
#ifdef SC2API
    for (const sc2::UnitTypeData & data : bot.Observation()->GetUnitTypeData())
    {
        if (name == data.name)
        {
            return UnitType(data.unit_type_id, bot);
        }
    }
#else

#endif

    return UnitType();
}

bool UnitType::isOverlord() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::ZERG_OVERLORD;
#else
    return m_type == BWAPI::UnitTypes::Zerg_Overlord;
#endif
}

bool UnitType::isLarva() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::ZERG_LARVA;
#else
    return m_type == BWAPI::UnitTypes::Zerg_Larva;
#endif
}

bool UnitType::isEgg() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::ZERG_EGG;
#else
    return m_type == BWAPI::UnitTypes::Zerg_Egg;
#endif
}

bool UnitType::isQueen() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::ZERG_QUEEN;
#else
    return m_type == BWAPI::UnitTypes::Zerg_Queen;
#endif
}

bool UnitType::isTank() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::TERRAN_SIEGETANK || m_type == sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED;
#else
    return m_type == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode || m_type == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode;
#endif
}

bool UnitType::isMorphedBuilding() const
{
#ifdef SC2API
    
    switch (m_type.ToType())
    {
        case sc2::UNIT_TYPEID::ZERG_LAIR:                   { return true;  }
        case sc2::UNIT_TYPEID::ZERG_HIVE:                   { return true;  }
        case sc2::UNIT_TYPEID::ZERG_GREATERSPIRE:           { return true;  }
        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:    { return true;  }
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:       { return true;  }
        default:                                            { return false; }                                                            
    }
#else
    return  m_type == BWAPI::UnitTypes::Zerg_Sunken_Colony ||
            m_type == BWAPI::UnitTypes::Zerg_Spore_Colony ||
            m_type == BWAPI::UnitTypes::Zerg_Lair ||
            m_type == BWAPI::UnitTypes::Zerg_Hive ||
            m_type == BWAPI::UnitTypes::Zerg_Greater_Spire;
#endif
}

int UnitType::getMovementSpeed() const
{
    return m_client->Observation()->GetUnitTypeData()[m_type].movement_speed;
}

int UnitType::getSightRange() const
{
    return m_client->Observation()->GetUnitTypeData()[m_type].sight_range;
}

UnitTypeID UnitType::getRequiredStructure() const
{
    return m_client->Observation()->GetUnitTypeData()[m_type].tech_requirement;
}

std::vector<sc2::UnitTypeID> UnitType::getEquivalentUnits() const
{
	return m_client->Observation()->GetUnitTypeData()[m_type].tech_alias;
}

bool UnitType::requiredAttached() const
{
	return m_client->Observation()->GetUnitTypeData()[m_type].require_attached;
}

float UnitType::getBuildTime() const
{
	return m_client->Observation()->GetUnitTypeData()[m_type].build_time;
}