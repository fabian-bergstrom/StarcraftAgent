#include "library.h"

namespace py = pybind11;

void define_tech_tree(py::module & m)
{
    py::class_<TypeData>(m, "TypeData")
        .def_readonly("race",                 &TypeData::race)
        .def_readonly("mineral_cost",         &TypeData::mineralCost,                  "mineral cost of the item")
        .def_readonly("gas_cost",             &TypeData::gasCost,                      "gas cost of the item")
        .def_readonly("supply_cost",          &TypeData::supplyCost,                   "supply cost of the item")
        .def_readonly("build_time",           &TypeData::buildTime,                    "build time of the item in seconds (should be 32 game updates per tick, can someone verify this?)")
        .def_readonly("is_unit",              &TypeData::isUnit)
        .def_readonly("is_building",          &TypeData::isBuilding)
        .def_readonly("is_worker",            &TypeData::isWorker)
        .def_readonly("is_refinery",          &TypeData::isRefinery)
        .def_readonly("is_supply_provider",   &TypeData::isSupplyProvider)
        .def_readonly("is_resource_depot",    &TypeData::isResourceDepot)
        .def_readonly("is_addon",             &TypeData::isAddon)
        .def_readonly("build_ability",        &TypeData::buildAbility,                 "the ability that creates this item")
        .def_readonly("warp_ability",         &TypeData::warpAbility,                  "the ability that creates this item via warp-in")
        .def_readonly("what_builds",          &TypeData::whatBuilds,                   "any of these units can build the item")
        .def_readonly("required_units",       &TypeData::requiredUnits,                "owning ONE of these is required to make  -> List[UnitType]")
        .def_readonly("required_upgrades",    &TypeData::requiredUpgrades,             "having ALL of these is required to make  -> List[UPGRADE_ID]")
        .def_readonly("required_addons",      &TypeData::requiredAddons,               "a unit of this type must be present next to the producer  -> List[UnitType]");

    py::class_<TechTree>(m, "TechTree")
        .def("get_data", py::overload_cast<const UnitType &>(&TechTree::getData, py::const_))
        .def("get_data", py::overload_cast<const CCUpgrade &>(&TechTree::getData, py::const_))
		.def("suppress_warnings", &TechTree::setSuppressWarnings, "Suppress type and uppgrade warnings" ,"b"_a)
		;
}
