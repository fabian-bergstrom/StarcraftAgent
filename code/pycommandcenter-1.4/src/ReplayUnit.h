#pragma once
#include "Unit.h"
#include "IDAReplayObserver.h"

class IDAReplayObserver;

//! A Unit that have a replayobserver insted of an Agent, 
class ReplayUnit: public Unit
{
	mutable IDAReplayObserver * m_replayObserver;
	UnitType m_type;

public:
		ReplayUnit(const sc2::Unit * unit, IDAReplayObserver & replayObserver);

		const UnitType & getType() const;
		bool hasTarget() const;
		ReplayUnit getTarget() const;
		int getPlayer() const;


};