#include "IDAReplayObserver.h"
#include "Util.h"

void IDAReplayObserver::setUnits()
{
	m_allUnits.clear();
	m_allUnitsID.clear();
	for (auto & unit : Observation()->GetUnits())
	{
		ReplayUnit replayUnit = ReplayUnit(unit, *this);
		m_allUnits.push_back(replayUnit);
		m_allUnitsID.insert(replayUnit.getID());
	}
}

IDAReplayObserver::IDAReplayObserver():
	sc2::ReplayObserver(),
	m_techTree(*this)
{
}

void IDAReplayObserver::OnGameStart()
{
	setUnits();
	m_techTree.onStart();

}

void IDAReplayObserver::OnStep()
{
	setUnits();
}

void IDAReplayObserver::OnGameEnd()
{
}

void IDAReplayObserver::OnUnitDestroyed(const sc2::Unit* unit)
{
	ReplayUnit unitInformation = ReplayUnit(unit, *this);
	OnReplayUnitDestroyed(&unitInformation);
}

void IDAReplayObserver::OnReplayUnitDestroyed(const ReplayUnit *)
{
	
}

void IDAReplayObserver::OnUnitCreated(const sc2::Unit * unit)
{
	ReplayUnit unitInformation = ReplayUnit(unit, *this);
	OnReplayUnitCreated(&unitInformation);
}

void IDAReplayObserver::OnReplayUnitCreated(const ReplayUnit *)
{
}

void IDAReplayObserver::OnBuildingConstructionComplete(const sc2::Unit *unit)
{
	ReplayUnit unitInformation = ReplayUnit(unit, *this);
	OnReplayUnitCreated(&unitInformation);
}




ReplayUnit IDAReplayObserver::GetUnit(const CCUnitID tag) const
{	
	
		return ReplayUnit(Observation()->GetUnit(tag), *(IDAReplayObserver *)this);

}

bool IDAReplayObserver::UnitExists(const CCUnitID tag) const
{
	return m_allUnitsID.find(tag) != m_allUnitsID.end();
	
}






const std::vector<ReplayUnit>& IDAReplayObserver::GetAllUnits() const
{

	return m_allUnits;
}

CCRace IDAReplayObserver::GetPlayerRace(int player)
{	
	return ReplayControl()->GetReplayInfo().players[player].race;
}

std::string IDAReplayObserver::GetReplayPath()
{
	return ReplayControl()->GetReplayInfo().replay_path;
}

sc2::GameResult IDAReplayObserver::GetResultForPlayer(int player)
{
	return ReplayControl()->GetReplayInfo().players[player].game_result;
}

const TechTree & IDAReplayObserver::GetTechTree() const
{
	return m_techTree;
}

const TypeData & IDAReplayObserver::Data(const UnitType & type) const
{
	return m_techTree.getData(type);
}

const TypeData & IDAReplayObserver::Data(const CCUpgrade & type) const
{
	return m_techTree.getData(type);
}

const TypeData & IDAReplayObserver::Data(const MetaType & type) const
{
	return m_techTree.getData(type);
}

const TypeData & IDAReplayObserver::Data(const Unit & unit) const
{
	return m_techTree.getData(unit.getType());
}


