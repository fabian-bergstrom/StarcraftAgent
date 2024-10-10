#pragma once

#include <deque>
#include <limits>

#include "Common.h"
#include "ReplayUnit.h"
#include "TechTree.h"


class ReplayUnit;

class IDAReplayObserver : public sc2::ReplayObserver
{
	void setUnits();
	std::vector<ReplayUnit>       m_allUnits;
	std::set<CCUnitID>         m_allUnitsID;
	TechTree                m_techTree;



public:
	IDAReplayObserver();
	
	void OnGameStart() override;
	void OnStep() override;
	void OnGameEnd() override;
	void OnUnitDestroyed(const sc2::Unit*) override;
	virtual void OnReplayUnitDestroyed(const ReplayUnit*);
	void OnUnitCreated(const sc2::Unit*);
	virtual void OnReplayUnitCreated(const ReplayUnit*);
	void OnBuildingConstructionComplete(const sc2::Unit*);

	ReplayUnit GetUnit(const CCUnitID tag) const;
	bool UnitExists(const CCUnitID tag) const;


	const std::vector<ReplayUnit> & GetAllUnits() const;
	CCRace GetPlayerRace(int player);
	std::string GetReplayPath();
	sc2::GameResult GetResultForPlayer(int player);

	const TechTree & GetTechTree() const;

	const TypeData & Data(const UnitType & type) const;
	const TypeData & Data(const CCUpgrade & type) const;
	const TypeData & Data(const MetaType & type) const;
	const TypeData & Data(const Unit & unit) const;

};

