#include "IDABot.h"
#include "Util.h"


IDABot::IDABot()
	: m_map(*this)
	, m_bases(*this)
	, m_unitInfo(*this)
	, m_techTree(*this)
    , m_buildingPlacer(*this)
{
}

void IDABot::OnGameStart()
{
	// ----------------------------------------------------------------- 
	// Initialize all start (base) locations.
	// -----------------------------------------------------------------
	for (auto & loc : Observation()->GetGameInfo().enemy_start_locations)
	{
		m_baseLocations.push_back(loc);
	}
	m_baseLocations.push_back(Observation()->GetStartLocation());

	// -----------------------------------------------------------------
	// Initialize all info, units, etc.
	// -----------------------------------------------------------------
	setUnits();
	m_techTree.onStart();
	m_map.onStart();
	m_unitInfo.onStart();
	m_bases.onStart();
    m_buildingPlacer.onStart();

    /*
    UnitType target { sc2::UNIT_TYPEID::TERRAN_FUSIONCORE, *this };
    std::vector<UnitType> needed = { target };
    build_order.clear();

    std::cout << "Creating plan: ";

    while (!needed.empty())
    {
        UnitType target = needed.back();
	    TypeData target_data = m_techTree.getData(target);
        build_order.push_back(target);
        std::cout << target.getName();
        needed.pop_back();
        
        if (!target_data.requiredUnits.empty())
        {
            // We only need one of the required units, so we pick the first.
            // TODO: Pick in a smarter way.
            UnitType subtarget = target_data.requiredUnits.front();
            needed.push_back(subtarget);

            std::cout << ", ";
        }
    }

    std::cout << "." << std::endl;
    std::cout << "Created build plan of " << build_order.size() << " steps." << std::endl;
    */
}

void IDABot::OnStep()
{
	// -----------------------------------------------------------------
	// Update units, map info, unit info, and base info.
	// -----------------------------------------------------------------
	setUnits();
	m_map.onFrame();
	m_unitInfo.onFrame();
	m_bases.onFrame();

	// suppress warnings while we update the tiles occupied by units
	bool old_suppress = m_techTree.getSuppressWarnings();
	m_techTree.setSuppressWarnings(true);
	m_buildingPlacer.updateReserved(GetAllUnits());
	m_techTree.setSuppressWarnings(old_suppress);

	// -----------------------------------------------------------------
	// Draw debug interface, and send debug interface to the Sc2 client.
	// -----------------------------------------------------------------
	Debug()->SendDebug();
	m_buildingPlacer.drawReservedTiles();
}

void IDABot::setUnits()
{
	m_allUnits.clear();
	Control()->GetObservation();
	for (auto & unit : Observation()->GetUnits())
	{
		m_allUnits.push_back(Unit(unit, *this));
	}
}

CCRace IDABot::GetPlayerRace(int player) const
{
	auto playerID = Observation()->GetPlayerID();
	for (auto & playerInfo : Observation()->GetGameInfo().player_info)
	{
		if (playerInfo.player_id == playerID)
		{
			return playerInfo.race_actual;
		}
	}

	BOT_ASSERT(false, "Failed to find the player's race!");
	return sc2::Race::Random;
}

const MapTools & IDABot::Map() const
{
	return m_map;
}

const BaseLocationManager & IDABot::Bases() const
{
	return m_bases;
}

const UnitInfoManager & IDABot::UnitInfo() const
{
	return m_unitInfo;
}

int IDABot::GetCurrentFrame() const
{
	return (int)Observation()->GetGameLoop();
}

const TechTree & IDABot::GetTechTree() const
{
    return m_techTree;
}

int IDABot::GetCurrentSupply() const
{
	return Observation()->GetFoodUsed();
}

int IDABot::GetMaxSupply() const
{
	return Observation()->GetFoodCap();
}

int IDABot::GetMinerals() const
{
	return Observation()->GetMinerals();
}

int IDABot::GetGas() const
{
	return Observation()->GetVespene();
}

Unit IDABot::GetUnit(const CCUnitID & tag) const
{
	return Unit(Observation()->GetUnit(tag), *(IDABot *)this);
}

const std::vector<Unit> & IDABot::GetAllUnits() const
{
	return m_allUnits;
}

const std::vector<Unit> & IDABot::GetMyUnits() const
{
	return UnitInfo().getUnits(Players::Self);
}

const std::vector<Unit> IDABot::GetUnits(const UnitType & type, int player) const
{
    std::vector<Unit> units;
    for (const Unit & unit : GetAllUnits()) {
        if (unit.getPlayer() == player)
        {
            if (!type.isValid() || (type.isValid() && unit.getType() == type))
            {
                units.push_back(unit);
            }
        }
    }
    return units;
}

CCPosition IDABot::GetStartLocation() const
{
	return Observation()->GetStartLocation();
}

const std::vector<CCPosition> & IDABot::GetStartLocations() const
{
	return m_baseLocations;
}

void IDABot::OnError(const std::vector<sc2::ClientError> & client_errors, const std::vector<std::string> & protocol_errors)
{
	// This is called when the sc2api (Google's API) has an error.
}

BuildingPlacer & IDABot::GetBuildingPlacer()
{
    return m_buildingPlacer;
}


void IDABot::SendChat(const std::string & message)
{
    Actions()->SendChat(message);
}

const TypeData & IDABot::Data(const UnitType & type) const
{
	return m_techTree.getData(type);
}

const TypeData & IDABot::Data(const Unit & unit) const
{
	return m_techTree.getData(unit.getType());
}

const TypeData & IDABot::Data(const CCUpgrade & type) const
{
	return m_techTree.getData(type);
}

const TypeData & IDABot::Data(const MetaType & type) const
{
	return m_techTree.getData(type);
}

/*
	API extended summer 2020
*/

void IDABot::DebugCreateUnit(UnitTypeID unit_type, const CCPosition& p, uint32_t player_id, uint32_t count) 
{
	switch (player_id) // playerconstants for the IDAbot is not the same as for the sc2 API
	{
	case 0:
		Debug()->DebugCreateUnit(unit_type, p, 1, count);
		break;
	case 1:
		Debug()->DebugCreateUnit(unit_type, p, 2, count);
		break;
	case 2:
		Debug()->DebugCreateUnit(unit_type, p, 0, count);
		break;
	case 3:
		Debug()->DebugCreateUnit(unit_type, p, 3, count);
	default:
		break;
	}
}

void IDABot::DebugKillUnit(const Unit unit) 
{
	Debug()->DebugKillUnit(unit.getUnitPtr());
}

void IDABot::DebugShowMap() 
{
	Debug()->DebugShowMap();
}

void IDABot::DebugFastBuild() 
{
	Debug()->DebugFastBuild();
}

void IDABot::DebugEnemyControl() 
{
	Debug()->DebugEnemyControl();
}

void IDABot::DebugIgnoreFood() 
{
	Debug()->DebugIgnoreFood();
}

void IDABot::DebugIgnoreResourceCost() 
{
	Debug()->DebugIgnoreResourceCost();
}

void IDABot::DebugGiveAllResources() 
{
	Debug()->DebugGiveAllResources();
}

void IDABot::DebugGodMode() 
{
	Debug()->DebugGodMode();
}

void IDABot::DebugIgnoreMineral() 
{
	Debug()->DebugIgnoreMineral();
}

void IDABot::DebugNoCooldowns() 
{
	Debug()->DebugIgnoreMineral();
}

void IDABot::DebugGiveAllTech()
{
	Debug()->DebugGiveAllTech();
}

void IDABot::DebugGiveAllUpgrades() 
{
	Debug()->DebugGiveAllUpgrades();
}

void IDABot::DebugSetScore(float score) 
{
	Debug()->DebugSetScore(score);
}

void IDABot::DebugEndGame(bool victory)
{
	Debug()->DebugEndGame(victory);
}

void IDABot::DebugSetEnergy(float value, const Unit unit)
{
	Debug()->DebugSetEnergy(value, unit.getUnitPtr());
}

void IDABot::DebugSetLife(float value, const Unit unit)
{
	Debug()->DebugSetLife(value, unit.getUnitPtr());
}

void IDABot::DebugSetShields(float value, const Unit unit)
{
	Debug()->DebugSetShields(value, unit.getUnitPtr());
}

// There is a bug in the latest SC2 (if using Blizzard API with game >=4.10)
// This a function to get the enemy base instead of using build location manager
// Switched over to other API where this is solved
// Leaving function incase of it breaking
const std::vector<Point2D> IDABot::GetEnemyBaseLocations()
{
	return Observation()->GetGameInfo().enemy_start_locations;
}

bool IDABot::HasCreep(Point2D p) const 
{
	return Observation()->HasCreep(p);
}

void IDABot::CameraMove(Point2DI p) 
{
	ActionsFeatureLayer()->CameraMove(p);
}

sc2::ABILITY_ID IDABot::abilityForUpgrade(sc2::UpgradeID upgrade_id) const
{
	return (sc2::ABILITY_ID)Observation()->GetUpgradeData()[upgrade_id].ability_id;
}

uint32_t IDABot::UpgradeMineralCost(sc2::UpgradeID upgrade_id) const
{
	return Observation()->GetUpgradeData()[upgrade_id].mineral_cost;
}

uint32_t IDABot::UpgradeGasCost(sc2::UpgradeID upgrade_id) const
{
	return Observation()->GetUpgradeData()[upgrade_id].vespene_cost;
}

float IDABot::UpgradeResearchTime(sc2::UpgradeID upgrade_id) const
{
	return Observation()->GetUpgradeData()[upgrade_id].research_time;
}

float IDABot::RadiusEffect(sc2::EffectID effect_id) const
{
	return Observation()->GetEffectData()[effect_id].radius;
}