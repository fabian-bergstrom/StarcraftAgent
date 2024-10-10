#include "library.h"

namespace py = pybind11;

void define_map_tools(py::module & m)
{
    py::class_<DistanceMap>(m, "DistanceMap")
        .def("computer_distance_map", &DistanceMap::computeDistanceMap, "bot"_a, "start_tile"_a)
        .def("get_distance", py::overload_cast<const CCTilePosition &>(&DistanceMap::getDistance, py::const_), "position"_a)
        .def("get_distance", py::overload_cast<const CCPosition &>(&DistanceMap::getDistance, py::const_), "position"_a)
        .def("get_sorted_tiles", &DistanceMap::getSortedTiles)
        .def("get_start_tile", &DistanceMap::getStartTile)
        .def("draw", &DistanceMap::draw, "bot"_a);

    const CCColor white{ 255, 255, 255 };
    py::class_<MapTools>(m, "MapTools")
        .def_property_readonly("width", &MapTools::width, "The width of the map")
        .def_property_readonly("height", &MapTools::height, "The height of the map")
		.def_property_readonly("map_name", &MapTools::name, "The name of the map")
        //.def("terrainHeight", &MapTools::terrainHeight, py::const_)
        .def("draw_line", py::overload_cast<const CCPosition &, const CCPosition &, const CCColor &>(&MapTools::drawLine, py::const_), py::arg("start"), py::arg("stop"), py::arg("color") = sc2::Colors::White, "Draws a line with the given color between to two points.")
        .def("draw_tile", py::overload_cast<const CCTilePosition &, const CCColor &>(&MapTools::drawTile, py::const_), py::arg("tile"), py::arg("color") = sc2::Colors::White, "Draws an outline with the given color to the given tile.")
		.def("draw_box", py::overload_cast<const CCPosition &, const CCPosition &, const CCColor &>(&MapTools::drawBox, py::const_), py::arg("top_left"), py::arg("bottom_right"), py::arg("color") = sc2::Colors::White, "Draws a box with the given color from the top left cornor to the botom right")
        .def("draw_circle", py::overload_cast<const CCPosition &, CCPositionType, const CCColor &>(&MapTools::drawCircle, py::const_), py::arg("center"), py::arg("radius"), py::arg("color") = sc2::Colors::White, "Draws a circle with the given color with the provided point as center and the float as radius")
		.def("draw_resource_sphere", &MapTools::drawResourceSphere, py::arg("center"), py::arg("radius"), py::arg("color") = sc2::Colors::White, "Draws a 3D sphere with the given color with the provided point as center and the float as radius")
        .def("draw_text", &MapTools::drawText, "position"_a, "text"_a, "color"_a = sc2::Colors::White)
        .def("draw_text_screen", &MapTools::drawTextScreen, "percentage_x"_a, "percentage_y"_a, "text"_a, "color"_a = sc2::Colors::White)
        .def("is_valid_tile", py::overload_cast<int, int>(&MapTools::isValidTile, py::const_), "x"_a, "y"_a, "Checks if the position is valid for the map")
        .def("is_valid_tile", py::overload_cast<const CCTilePosition &>(&MapTools::isValidTile, py::const_), "point_2di"_a, "Checks if the tile is valid for the map")
        .def("is_valid_position", py::overload_cast<const CCPosition &>(&MapTools::isValidPosition, py::const_), "point_2d"_a, "Checks if the position is valid for the map")
        .def("is_powered", &MapTools::isPowered, "x"_a, "y"_a)
        .def("is_explored", py::overload_cast<int, int>(&MapTools::isExplored, py::const_), "x"_a, "y"_a, "Returns if the coordinates has been explored or not")
        .def("is_explored", py::overload_cast<const CCPosition &>(&MapTools::isExplored, py::const_), "point2d"_a, "Returns if the coordinate has been explored or not")
        .def("is_explored", py::overload_cast<const CCTilePosition &>(&MapTools::isExplored, py::const_), "point2di"_a, "Returns if the tile has been explored or not")
        .def("is_connected", py::overload_cast<int, int, int, int>(&MapTools::isConnected, py::const_), "x1"_a, "y1"_a, "x2"_a, "y2"_a, "Returns if the coordinates are connected")
        .def("is_connected", py::overload_cast<const CCTilePosition &, const CCTilePosition &>(&MapTools::isConnected, py::const_), "from"_a, "too"_a, "Returns if the tiles are connected")
        .def("is_connected", py::overload_cast<const CCPosition &, const CCPosition &>(&MapTools::isConnected, py::const_), "from"_a, "too"_a, "Returns if the positions are of two connected tiles")
        .def("is_walkable", py::overload_cast<int, int>(&MapTools::isWalkable, py::const_), "x"_a, "y"_a, "Returns if the coordinates is walkable")
        .def("is_walkable", py::overload_cast<const CCTilePosition &>(&MapTools::isWalkable, py::const_), "point2di"_a, "Returns if the tile is walkable")
        .def("is_buildable", py::overload_cast<int, int>(&MapTools::isBuildable, py::const_), "x"_a, "y"_a, "Return if it is possible to build at the provided coordinate")
        .def("is_buildable", py::overload_cast<const CCTilePosition &>(&MapTools::isBuildable, py::const_), "point2di"_a, "Return if it is possible to build on tile")
        .def("is_visible", &MapTools::isVisible, "x"_a, "y"_a, "Can you see the coordinates")
        .def("can_build_type_at_position", &MapTools::canBuildTypeAtPosition, "x"_a, "y"_a, "unit_type"_a, "Is it possible to build the provided unittype at the location")
        .def("is_depot_buildable_tile", &MapTools::isDepotBuildableTile, "x"_a, "y"_a, "Is it possbile do build a depot at the position")
        .def("get_ground_distance", &MapTools::getGroundDistance, "Returns the ground distance between the two points. Note that this uses a BFS approach and may overshoot a bit. The function will also do the calculations with integers resulting in that sometimes when close to a wall it might return -1 even though a path is available", "from"_a, "to"_a)
        .def("get_distance_map", py::overload_cast<const CCTilePosition &>(&MapTools::getDistanceMap, py::const_), "point2di"_a)
        .def("get_distance_map", py::overload_cast<const CCPosition &>(&MapTools::getDistanceMap, py::const_), "point2d"_a)
        .def("get_closest_tiles_to", &MapTools::getClosestTilesTo, "Returns a list of positions, where the first position is the closest and the last is the furthest", "point2di"_a)
        .def("get_least_recently_seen_tile", &MapTools::getLeastRecentlySeenTile, "Returns the tile that the most time has passed since it was visible");
}