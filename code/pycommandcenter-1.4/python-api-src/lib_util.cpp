#include "library.h"

namespace py = pybind11;

void define_util(py::module & mod)
{
    py::module m = mod.def_submodule("util");

    /*
    TODO: Implement and document these functions
    Maybe should be included in Point instead?
    CCTilePosition  GetTilePosition(const CCPosition & pos);
    CCPosition      GetPosition(const CCTilePosition & tile);

    UnitType        GetTownHall(const CCRace & race, IDABot & bot);
    UnitType        GetRefinery(const CCRace & race, IDABot & bot);
    UnitType        GetSupplyProvider(const CCRace & race, IDABot & bot);
    */

    // Redundant, just call Race.Terran.name instead
    //m.def("get_string_from_race", &Util::GetStringFromRace);
    m.def("dist", py::overload_cast<const Unit &, const Unit &>(&Util::Dist));
    m.def("dist", py::overload_cast<const Unit &, const CCPosition &>(&Util::Dist));
    m.def("dist", py::overload_cast<const CCPosition &, const CCPosition &>(&Util::Dist));
}