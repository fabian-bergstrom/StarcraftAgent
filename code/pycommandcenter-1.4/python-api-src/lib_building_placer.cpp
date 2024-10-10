#include "library.h"

namespace py = pybind11;

void define_building_placer(py::module & m)
{
    py::class_<BuildingPlacer>(m, "BuildingPlacer")
        .def("can_build_here", &BuildingPlacer::canBuildHere, "x"_a, "y"_a, "unit_type"_a, "Returns if the provided unittype it possible to be built at the location. Note: This function uses the width and height of the unittype and this is not correct for addons. So to check addons please use can_build_here_with_size with a hardcoded size instead.")
		.def("can_build_here_with_size", &BuildingPlacer::canBuildHereWithSize, "x"_a, "y"_a, "width"_a, "height"_a, "Checks if it is possible to build something with the provided width and height at the provided coordinates. Note: False if the it overlaps with a baselocation")
        .def("can_build_here_with_spaces", &BuildingPlacer::canBuildHereWithSpace, "Creates a square with the help of x, y, distance_to_building and the size of the unit_type. Good approach if we later gonna make a addon to the building. Note: Does not reserve those extra tiles given by distance_to_building for the future! Note: This function uses the width and height of the unittype and this is not correct for addons. So to check addons please use can_build_here_with_size with a hardcoded size instead.", "x"_a, "y"_a, "unit_type"_a, "distance_to_building"_a)
        .def("get_build_location_near", &BuildingPlacer::getBuildLocationNear, "The search_count is how many building we should check (nearby buildings, instead of i < size, we can switch size to search_count). distance_to_building is the distance to the closest building.", "point2di"_a, "unit_type"_a, "distance_to_building"_a = 2, "search_count"_a = 1000)
        .def("reserve_tiles", &BuildingPlacer::reserveTiles, "It's possible to reserve tiles, which makes it impossible to build at the position given by x and y.", "x"_a, "y"_a, "width"_a, "height"_a)
        .def("free_tiles", &BuildingPlacer::freeTiles,"Free the tile (x, y) from reservation", "x"_a, "y"_a, "width"_a, "height"_a);
}