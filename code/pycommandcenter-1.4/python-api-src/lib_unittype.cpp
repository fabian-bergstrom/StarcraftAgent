#include "library.h"

namespace py = pybind11;

void define_unittype(py::module & m)
{
    py::class_<UnitType>(m, "UnitType")
		.def(py::init([](const sc2::UnitTypeID & type, IDABot & bot) {
			return UnitType(type, bot, bot);
		}))
        .def(py::self == py::self)
        .def_property_readonly("unit_typeid", [](UnitType & unit_type) { return static_cast<sc2::UNIT_TYPEID>(unit_type.getAPIUnitType()); })
        .def_property_readonly("name", &UnitType::getName, "The name of the unit as a sting.")
        .def_property_readonly("race", &UnitType::getRace, "The race the unit belongs to.")
        .def_property_readonly("movement_speed", &UnitType::getMovementSpeed, "Movement speed of unit type.")
        .def_property_readonly("sight_range", &UnitType::getSightRange, "Range the unit reveals vision.")
        .def_property_readonly("required_structure", &UnitType::getRequiredStructure, "Structure required to build this unit. (Or any with the same tech_alias)")
        .def_property_readonly("is_valid", &UnitType::isValid, "Its a valid unit type")
        .def_property_readonly("is_building", &UnitType::isBuilding, "Is this unit type a building or not")
        .def_property_readonly("is_combat_unit", &UnitType::isCombatUnit, "The unit is not any of the following: worker, supply provider, building, larva, egg")
        .def_property_readonly("is_supply_provider", &UnitType::isSupplyProvider, "The unit provides supply")
        .def_property_readonly("is_resource_depot", &UnitType::isResourceDepot, "The unit is one of the following: hatchery, lair, hive, commandcenter, orbialtcommand, planetaryfortress, nexus")
        .def_property_readonly("is_refinery", &UnitType::isRefinery, "The unit is one of the following (depending on race): refinery, assimilator, extractor")
        .def_property_readonly("is_detector", &UnitType::isDetector, "Is this a unit type which is a detector unit")
        .def_property_readonly("is_geyser", &UnitType::isGeyser, "Is the unit type a geyser")
        .def_property_readonly("is_mineral", &UnitType::isMineral, "Is the unit type a mineralfield")
        .def_property_readonly("is_worker", &UnitType::isWorker, "Is the unit type a unit which is a worker")
        .def_property_readonly("is_morphed_building", &UnitType::isMorphedBuilding, "Has this building been mophed")
        // Not implemented in CommandCenter
        //.def_property_readonly("can_attack", &UnitType::canAttack)
        //.def_property_readonly("can_Move", &UnitType::canMove)
        .def_property_readonly("is_addon", &UnitType::isAddon, "Is this unit type a addon")
		.def_property_readonly("can_attack_ground", &UnitType::canAttackGound, "True if this unit can attack ground units")
		.def_property_readonly("can_attack_air", &UnitType::canAttackAir, "True if this unit can attack air units")
        .def_property_readonly("attack_range", &UnitType::getAttackRange, "Returns the attack range of the unit type")
		.def_property_readonly("attack_damage", &UnitType::getAttackDamage, "Returns the base attack damage of the unit type")
        .def_property_readonly("tile_width", &UnitType::tileWidth, "Returns the width of the unit type")
        .def_property_readonly("tile_height", &UnitType::tileHeight, "Returns the height of the unit type")
        .def_property_readonly("supply_provided", &UnitType::supplyProvided, "Amount of supply provided by the unit type")
        .def_property_readonly("supply_required", &UnitType::supplyRequired, "Amount of supply required for the unit type")
        .def_property_readonly("mineral_price", &UnitType::mineralPrice, "Mineral price of the unit type")
        .def_property_readonly("gas_price", &UnitType::gasPrice, "Gas price of the unit type")
        .def_property_readonly("is_overlord", &UnitType::isOverlord, "Is this a ZERG_OVERLORD")
        .def_property_readonly("is_larva", &UnitType::isLarva, "is this a ZERG_LARVA")
        .def_property_readonly("is_egg", &UnitType::isEgg, "is this a ZERG_EGG")
        .def_property_readonly("is_queen", &UnitType::isQueen, "is this a ZERG_QUEEN")
        .def_property_readonly("is_tank", &UnitType::isTank, "is this a TERRAN_SIEGETANK or TERRAN_SIEGETANKSIEGED")
		.def_property_readonly("get_equivalent_units", &UnitType::getEquivalentUnits, "Units this is equivalent to in terms of satisfying tech requirements.")
		.def_property_readonly("required_attached", &UnitType::requiredAttached, "Whether tech_requirement is an add-on.")
		.def_property_readonly("build_time", &UnitType::getBuildTime, "How long the unit takes to build.")
        .def("__hash__", [](const UnitType & unit_type) { return std::hash<CCUnitID>{}(unit_type.getAPIUnitType()); })
        .def(py::self == py::self)
        .def("__repr__", [](const UnitType & unit_type) { return "<UnitType: '" + unit_type.getName() + "'>"; });

        // Not implemented in CommandCenter
        //.def("whatBuilds", &UnitType::whatBuilds);
}
