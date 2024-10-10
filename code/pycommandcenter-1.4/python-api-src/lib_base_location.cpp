#include "library.h"

namespace py = pybind11;

void define_base_location(py::module & m)
{
    py::class_<BaseLocation>(m, "BaseLocation")
        .def_property_readonly("geysers", &BaseLocation::getGeysers, "List of geysers at base location (List of units)")
        .def_property_readonly("minerals", &BaseLocation::getMinerals, "List of mineral fields at base location (List of unit), Note: also returns the empty mineralfields")
        //.def_property_readonly("mineral_fields", &BaseLocation::getMinerals, "Alias for minerals in order to differentiate from harvested minerals") // This just did the same thing as minerals (above) removed to avoid confusion
        .def_property_readonly("is_start_location", &BaseLocation::isStartLocation, "True if the base location is a start location, False otherwise")
        .def_property_readonly("depot_position", &BaseLocation::getDepotPosition, "A suitable position for building a town hall (Command Center, Hatchery or Nexus), defined as a : class :`library.Point2DI`.")
        .def_property_readonly("position", &BaseLocation::getPosition, "The position of the center of the BaseLocation, defined as a :class:`library.Point2D`.")
        .def("get_ground_distance", py::overload_cast<const CCPosition &>(&BaseLocation::getGroundDistance, py::const_))
        .def("get_ground_distance", py::overload_cast<const CCTilePosition &>(&BaseLocation::getGroundDistance, py::const_))
        .def("is_occupied_by_player", &BaseLocation::isOccupiedByPlayer, "player constant"_a, "If the baselocation is occupied by the provided player. See :ref:`playerconstants` for more information")
        .def("is_player_start_location", &BaseLocation::isPlayerStartLocation, "player_constant"_a, "If the baselocation is the start location of the provided palyer. See :ref:`playerconstants` for more information")
        .def("contains_position", &BaseLocation::containsPosition, "If the baselocation contains the provided :class:`library.Point2D` position");

    py::class_<BaseLocationManager>(m, "BaseLocationManager")
        .def_property_readonly("base_locations", &BaseLocationManager::getBaseLocations, py::return_value_policy::reference, "A list of all :class:`library.BaseLocation` on the current map")
        .def_property_readonly("starting_base_locations", &BaseLocationManager::getStartingBaseLocations, py::return_value_policy::reference, "A list of all :class:`library.BaseLocation` on the current map which a player started at, indexed by Player constant(see :ref:`playerconstants`).")
        .def("get_occupied_base_locations", &BaseLocationManager::getOccupiedBaseLocations, py::return_value_policy::reference, "player_constant"_a)
        .def("get_player_starting_base_location", &BaseLocationManager::getPlayerStartingBaseLocation, py::return_value_policy::copy, "player_constant"_a, "Returns the :class:`library.BaseLocation` that provided :ref:`playerconstants` started at.")
        .def("get_next_expansion", &BaseLocationManager::getNextExpansion, py::return_value_policy::copy, "player_constant"_a, "Returns the :class:`library.BaseLocation` that is closest to the startlocation of provided :ref:`playerconstants` that is possible to expand to.");
}