#include "Unit.h"
#include "IDABot.h"

Unit::Unit()
	: m_bot(nullptr)
	, m_unit(nullptr)
	, m_unitID(0)
{

}

Unit::Unit(const sc2::Unit * unit, IDABot & bot)
	: m_bot(&bot)
	, m_unit(unit)
	, m_unitID(unit->tag)
	, m_unitType(unit->unit_type, bot, bot)
{

}

Unit::Unit(const sc2::Unit * unit)
	: m_unit(unit)
	, m_unitID(unit->tag)
{

}

const sc2::Unit * Unit::getUnitPtr() const
{
	return m_unit;
}

const sc2::UnitTypeID & Unit::getAPIUnitType() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return m_unit->unit_type;
}

bool Unit::operator < (const Unit & rhs) const
{
	return m_unit < rhs.m_unit;
}

bool Unit::operator == (const Unit & rhs) const
{
	return m_unit == rhs.m_unit;
}

const UnitType & Unit::getType() const
{
	return m_unitType;
}


CCPosition Unit::getPosition() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return m_unit->pos;
}

CCTilePosition Unit::getTilePosition() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return Util::GetTilePosition(m_unit->pos);
}

CCHealth Unit::getHitPoints() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return m_unit->health;
}

CCHealth Unit::getShields() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return m_unit->shield;
}

CCHealth Unit::getEnergy() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return m_unit->energy;
}

float Unit::getBuildPercentage() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return m_unit->build_progress;
}

CCPlayer Unit::getPlayer() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	switch (m_unit->alliance)
	{
	case sc2::Unit::Alliance::Self:		return Players::Self;
	case sc2::Unit::Alliance::Enemy:	return Players::Enemy;
	case sc2::Unit::Alliance::Neutral:	return Players::Neutral;
	case sc2::Unit::Alliance::Ally:		return Players::Ally;
	default:							return Players::None;
	}
}

CCUnitID Unit::getID() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	CCUnitID id = m_unit->tag;
	BOT_ASSERT(id == m_unitID, "Unit ID changed somehow");
	return id;
}

bool Unit::isCompleted() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return m_unit->build_progress >= 1.0f;
}

bool Unit::isTraining() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return m_unit->orders.size() > 0 && m_unitType.isBuilding();
}

bool Unit::isBeingConstructed() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return !isCompleted() && m_unit->build_progress > 0.0f;
}

int Unit::getWeaponCooldown() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return (int)m_unit->weapon_cooldown;
}

bool Unit::isCloaked() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
#ifdef SC2API
	return m_unit->cloak;
#else
	return m_unit->isCloaked();
#endif
}

bool Unit::isFlying() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
#ifdef SC2API
	return m_unit->is_flying;
#else
	return m_unit->isFlying();
#endif
}

std::vector<CCBuff> Unit::buffs() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
#ifdef SC2API
	return m_unit->buffs;
#else
	return m_unit->buffs();
#endif
}

bool Unit::isAlive() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
#ifdef SC2API
	return m_unit->is_alive;
#else
	return m_unit->getHitPoints() > 0;
#endif
}

bool Unit::isPowered() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
#ifdef SC2API
	return m_unit->is_powered;
#else
	return m_unit->isPowered();
#endif
}

bool Unit::isIdle() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
#ifdef SC2API
	return m_unit->orders.empty();
#else
	return m_unit->isIdle() && !m_unit->isMoving() && !m_unit->isGatheringGas() && !m_unit->isGatheringMinerals();
#endif
}

bool Unit::isBurrowed() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
#ifdef SC2API
	return m_unit->is_burrowed;
#else
	return m_unit->isBurrowed();
#endif
}

bool Unit::isValid() const
{
	return m_unit != nullptr;
}

void Unit::stop() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
#ifdef SC2API
	m_bot->Actions()->UnitCommand(m_unit, sc2::ABILITY_ID::STOP);
#else
	m_unit->stop();
#endif
}

void Unit::attackUnit(const Unit & target) const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	BOT_ASSERT(target.isValid(), "Target is not valid");
#ifdef SC2API
	m_bot->Actions()->UnitCommand(m_unit, sc2::ABILITY_ID::ATTACK_ATTACK, target.getUnitPtr());
#else
	m_unit->attack(target.getUnitPtr());
#endif
}

void Unit::attackMove(const CCPosition & targetPosition) const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
#ifdef SC2API
	m_bot->Actions()->UnitCommand(m_unit, sc2::ABILITY_ID::ATTACK_ATTACK, targetPosition);
#else
	m_unit->attack(targetPosition);
#endif
}

void Unit::move(const CCPosition & targetPosition) const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
#ifdef SC2API
	m_bot->Actions()->UnitCommand(m_unit, sc2::ABILITY_ID::GENERAL_MOVE, targetPosition);
#else
	m_unit->move(targetPosition);
#endif
}

void Unit::move(const CCTilePosition & targetPosition) const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
#ifdef SC2API
	m_bot->Actions()->UnitCommand(m_unit, sc2::ABILITY_ID::GENERAL_MOVE, CCPosition((float)targetPosition.x, (float)targetPosition.y));
#else
	m_unit->move(CCPosition(targetPosition));
#endif
}

void Unit::rightClick(const Unit & target) const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
#ifdef SC2API
	m_bot->Actions()->UnitCommand(m_unit, sc2::ABILITY_ID::SMART, target.getUnitPtr());
#else
	m_unit->rightClick(target.getUnitPtr());
#endif
}

void Unit::repair(const Unit & target) const
{
	rightClick(target);
}

void Unit::build(const UnitType & buildingType, CCTilePosition pos) const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	if (m_bot->Map().isConnected(getTilePosition(), pos))
	{
		m_bot->Actions()->UnitCommand(m_unit, m_bot->Data(buildingType).buildAbility, Util::GetPosition(pos));
	}
	else
	{
		printf("Warning! Cannot build at location (%d, %d) and therefore ignoring command.", pos.x, pos.y);
	}
}

void Unit::buildTarget(const UnitType & buildingType, const Unit & target) const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	m_bot->Actions()->UnitCommand(m_unit, m_bot->Data(buildingType).buildAbility, target.getUnitPtr());
}

void Unit::train(const UnitType & type) const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	m_bot->Actions()->UnitCommand(m_unit, m_bot->Data(type).buildAbility);
}

void Unit::morph(const UnitType & type) const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	m_bot->Actions()->UnitCommand(m_unit, m_bot->Data(type).morphAbility);
}

void Unit::research(sc2::UpgradeID upgrade) const
{
    BOT_ASSERT(isValid(), "Unit is not valid");
    m_bot->Actions()->UnitCommand(m_unit, m_bot->Data(upgrade).buildAbility);
}

bool Unit::isConstructing(const UnitType & type) const
{
	sc2::AbilityID buildAbility = m_bot->Data(type).buildAbility;
	return (getUnitPtr()->orders.size() > 0) && (getUnitPtr()->orders[0].ability_id == buildAbility);
}

void Unit::ability(sc2::AbilityID ability) const
{
    m_bot->Actions()->UnitCommand(getUnitPtr(), ability, false);
}

void Unit::ability(sc2::AbilityID ability, const sc2::Point2D& point) const
{
    m_bot->Actions()->UnitCommand(getUnitPtr(), ability, point, false);
}

void Unit::ability(sc2::AbilityID ability, const Unit& target) const
{
    m_bot->Actions()->UnitCommand(getUnitPtr(), ability, target.getUnitPtr(), false);
}

Unit Unit::getTarget() const
{
    BOT_ASSERT(isValid(), "Unit is not valid");
    // if unit has order, check tag of target of first order
    if(getUnitPtr()->orders.size() > 0){
		// t_id is set to the unit tag of the target
		CCUnitID t_id = getUnitPtr()->orders[0].target_unit_tag;

		// if it doesn't have a target. Return itself
		if (m_bot->GetUnit(t_id) == nullptr) {
			return *this;
		}

		// IDABot finds the unit with this tag
		return m_bot->GetUnit(t_id);
    }
    return *this;
}

bool Unit::hasTarget() const
{
    BOT_ASSERT(isValid(), "Unit is not valid");

    if (getUnitPtr()->orders.size() > 0) {
        if (getUnitPtr()->orders[0].target_unit_tag != NULL) {
            CCUnitID t_id = getUnitPtr()->orders[0].target_unit_tag;
            // IDABot finds the unit with this tag, and returns true if valid
            return m_bot->GetUnit(t_id).isValid();
        }
    }

    return false;
}

bool Unit::isBlip() const
{
    BOT_ASSERT(isValid(), "Unit is not valid");
#ifdef SC2API
    return m_unit->is_blip;
#else
    return m_unit->isBlip();
#endif
}

CCHealth Unit::getMaxHitPoints() const
{
    BOT_ASSERT(isValid(), "Unit is not valid");
    return m_unit->health_max;
}

float Unit::getFacing() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return m_unit->facing;
}

float Unit::getRadius() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return m_unit->radius;
}

float Unit::getProgress() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");

	// If unit has order, return progress of first order
	if (getUnitPtr()->orders.size() > 0) {
		return getUnitPtr()->orders[0].progress;
	}

	return -1;
}

const std::vector<float> Unit::getAllProgress() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	// If unit has order, return progress of first order
	std::vector<float> progressions;
	if (getUnitPtr()->orders.size() > 0) {
		for(int i = 0; i < getUnitPtr()->orders.size(); i++) {
			progressions.push_back(getUnitPtr()->orders[i].progress);
		}
	}

	return progressions;
}

sc2::AbilityID Unit::getCurrentAbilityID() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");

	// If unit has order, return AbilityID of first order
	if (getUnitPtr()->orders.size() > 0) {
		return getUnitPtr()->orders[0].ability_id;
	}
	// return invalid AbilityID
	return sc2::ABILITY_ID::INVALID;
}

void Unit::holdPosition() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	m_bot->Actions()->UnitCommand(m_unit, sc2::ABILITY_ID::GENERAL_HOLDPOSITION);
}

void Unit::patrol(const CCPosition & targetPosition) const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	m_bot->Actions()->UnitCommand(m_unit, sc2::ABILITY_ID::GENERAL_PATROL, targetPosition);
}

void Unit::stopDance() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	m_bot->Actions()->UnitCommand(m_unit, sc2::ABILITY_ID::STOP_DANCE);
}

/*
// Implementation by Dawid Abucewicz
bool Unit::isCarryingMinerals() const
{
    BOT_ASSERT(isValid(), "Unit is not valid");
    std::vector<CCBuff> buffs = m_unit->buffs;
    for (auto & b : buffs)
    {
        if (b.to_string() == "271" or b.to_string() == "272")
        {
            return true;
        }
    }
    return false;
}
*/

int Unit::gasLeftInGeyser() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return m_unit->vespene_contents;
}

int Unit::mineralsLeftInMineralfield() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return m_unit->mineral_contents;
}

int Unit::getOwner() const
{
	BOT_ASSERT(isValid(), "Unit is not valid");
	return m_unit->owner;
}

// Implemented with Blizzard SC2 API
bool Unit::isCarryingGas() const
{
	return sc2::IsCarryingVespene(*m_unit);
}

bool Unit::isCarryingMinerals() const
{
	return sc2::IsCarryingMinerals(*m_unit);
}

float Unit::maxShields() const
{
	return m_unit->shield_max;
}

float Unit::maxEnergy() const
{
	return m_unit->energy_max;
}