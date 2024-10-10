#pragma once

#include <deque>
#include <limits>

#include "Common.h"

#include "MapTools.h"
#include "BaseLocationManager.h"
#include "UnitInfoManager.h"
#include "BuildingPlacer.h"
#include "TechTree.h"
#include "TechTreeImproved.h"
#include "MetaType.h"
#include "Unit.h"

using sc2::UnitTypeID;
using sc2::Point2D;
using sc2::Point2DI;

class IDABot : public sc2::Agent 
{
    MapTools                m_map;
    BaseLocationManager     m_bases;
    UnitInfoManager         m_unitInfo;
    TechTree                m_techTree;
    BuildingPlacer          m_buildingPlacer;

    std::vector<Unit>       m_allUnits;
    std::vector<CCPosition> m_baseLocations;

    void setUnits();
    void OnError(const std::vector<sc2::ClientError> & client_errors, 
                 const std::vector<std::string> & protocol_errors = {}) override;
public:

    IDABot();

    void OnGameStart() override;
    void OnStep() override;

    /*
	    API for students
    */
    const TechTree & GetTechTree() const;
    const BaseLocationManager & Bases() const;
    const MapTools & Map() const;
    const UnitInfoManager & UnitInfo() const;
    CCRace GetPlayerRace(int player) const;
    CCPosition GetStartLocation() const;
    BuildingPlacer & GetBuildingPlacer();

    void SendChat(const std::string & message);

    int GetCurrentFrame() const;
    int GetMinerals() const;
    int GetCurrentSupply() const;
    int GetMaxSupply() const;
    int GetGas() const;
    Unit GetUnit(const CCUnitID & tag) const;
    const std::vector<Unit> & GetAllUnits() const;
	const std::vector<Unit> & GetMyUnits() const;
    const std::vector<Unit> GetUnits(const UnitType & type, int player = Players::Self) const;
    const std::vector<CCPosition> & GetStartLocations() const;

	/*
		API extended summer 2020
	*/
	void DebugCreateUnit(UnitTypeID unit_type, const CCPosition& p, uint32_t player_id = 1, uint32_t count = 1);
	void DebugKillUnit(const Unit unit);
	void DebugShowMap();
	void DebugFastBuild();
	void DebugEnemyControl();
	void DebugIgnoreFood();
	void DebugIgnoreResourceCost();
	void DebugGiveAllResources();
	void DebugGodMode();
	void DebugIgnoreMineral();
	void DebugNoCooldowns();
	void DebugGiveAllTech();
	void DebugGiveAllUpgrades();
	void DebugSetScore(float score);
	void DebugEndGame(bool victory);
	void DebugSetEnergy(float value, const Unit unit);
	void DebugSetLife(float value, const Unit unit);
	void DebugSetShields(float value, const Unit unit);
	const std::vector<Point2D> GetEnemyBaseLocations();
	bool HasCreep(Point2D p) const;
	void CameraMove(Point2DI p);
	sc2::ABILITY_ID abilityForUpgrade(sc2::UpgradeID upgrade_id) const;
	uint32_t UpgradeMineralCost(sc2::UpgradeID upgrade_id) const;
	uint32_t UpgradeGasCost(sc2::UpgradeID upgrade_id) const;
	float UpgradeResearchTime(sc2::UpgradeID upgrade_id) const;
	float RadiusEffect(sc2::EffectID effect_id) const;



    // Not needed, just convenience functions
    const TypeData & Data(const UnitType & type) const;
    const TypeData & Data(const CCUpgrade & type) const;
    const TypeData & Data(const MetaType & type) const;
    const TypeData & Data(const Unit & unit) const;
};
