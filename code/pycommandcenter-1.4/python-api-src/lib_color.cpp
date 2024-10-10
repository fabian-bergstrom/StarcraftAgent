#include "library.h"

namespace py = pybind11;

void define_color(py::module & m)
{
    py::class_<sc2::Color> color(m, "Color");
    color.def(py::init<>());
    color.def(py::init<int, int, int>(), "r"_a, "g"_a, "b"_a);
    color.def_readwrite("r", &sc2::Color::r, "Red");
    color.def_readwrite("g", &sc2::Color::g, "Green");
    color.def_readwrite("b", &sc2::Color::b, "Blue");
    color.def("__repr__", [](const sc2::Color & c) { return "<Color (" + std::to_string(c.r) + ", " + std::to_string(c.g) + ", " + std::to_string(c.b) + ")>"; });
    color.attr("WHITE") = sc2::Colors::White;
    color.attr("RED") = sc2::Colors::Red;
    color.attr("GREEN") = sc2::Colors::Green;
    color.attr("YELLOW") = sc2::Colors::Yellow;
    color.attr("BLUE") = sc2::Colors::Blue;
    color.attr("TEAL") = sc2::Colors::Teal;
    color.attr("PURPLE") = sc2::Colors::Purple;
    color.attr("BLACK") = sc2::Colors::Black;
    color.attr("GRAY") = sc2::Colors::Gray;
}