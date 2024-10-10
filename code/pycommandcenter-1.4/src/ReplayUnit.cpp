#include "ReplayUnit.h"



ReplayUnit::ReplayUnit(const sc2::Unit * unit, IDAReplayObserver & replayObserver)
	: m_replayObserver(&replayObserver), Unit(unit), m_type(unit->unit_type, replayObserver, replayObserver)
{
	
}

const UnitType & ReplayUnit::getType() const
{
	return m_type;
}

bool ReplayUnit::hasTarget() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	if (getUnitPtr()->orders.size() > 0) {
		if (getUnitPtr()->orders[0].target_unit_tag != NULL) {
			CCUnitID t_id = getUnitPtr()->orders[0].target_unit_tag;
			// IDAReplayObserver checks if the unit with this tag still exists
			if (m_replayObserver->UnitExists(t_id)){
				// IDAReplayObserver finds the unit with this tag, and returns true if valid
				return m_replayObserver->GetUnit(t_id).isValid();
			}
		}
	}

	return false;
}

ReplayUnit ReplayUnit::getTarget() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");


	// if unit has order, check tag of target of first order
	if (getUnitPtr()->orders.size() > 0) {
		// t_id is set to the unit tag of the target
		CCUnitID t_id = getUnitPtr()->orders[0].target_unit_tag;
		// Checks if the tag is a null tag or the unit have been removed
		if (t_id != sc2::NullTag && m_replayObserver->UnitExists(t_id)){
			// IDAReplayObserver finds the unit with this tag
			return m_replayObserver->GetUnit(t_id);
		}
	}
	return *this;
}

int ReplayUnit::getPlayer() const
{
	return m_unit->owner;
}


