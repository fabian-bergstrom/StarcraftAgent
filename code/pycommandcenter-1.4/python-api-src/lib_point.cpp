#include "library.h"

namespace py = pybind11;

void define_point(py::module & m)
{
    // Note that Point2D has dynamic attributes, allowing students to add additional methods to it

    py::class_<sc2::Point2D>(m, "Point2D", py::dynamic_attr())
        .def(py::init<float, float>())
        .def(py::init())
		.def(py::init([](const sc2::Point2DI & p) {
			return sc2::Point2D(p.x, p.y);
		}))
        .def_readwrite("x", &sc2::Point2D::x)
        .def_readwrite("y", &sc2::Point2D::y)
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self *= float())
        .def(py::self /= float())
        // These does not compile, but it is not a good idea to compare floats with floats anyway
        .def(py::self == py::self) // compiles now and this one is needed for __hash__. Outdated comment??
        //.def(py::self != py::self)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * float())
        .def(float() * py::self)
        .def(py::self / float())
        .def(float() / py::self)
		.def("__eq__", 
			[](const sc2::Point2D & p1, const sc2::Point2D & p2) {
				return (p1.x == p2.x && p1.y == p2.y); 
			})
		.def("__hash__",
			[](const sc2::Point2D & p) {
				return py::make_tuple(p.x, p.y).attr("__hash__")();
			})
        .def("__repr__",
            [](const sc2::Point2D & p) {
                return "<Point2D: (" + std::to_string(p.x) + ", " + std::to_string(p.y) + ")>";
            })
        .def("__str__", 
            [](const sc2::Point2D & p) {
                return "(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ")";
            });

    py::class_<sc2::Point2DI>(m, "Point2DI", py::dynamic_attr())
        .def(py::init<int, int>())
		.def(py::init())
		.def(py::init([](const sc2::Point2D & p) {
			return sc2::Point2DI(p);
		}))
        .def_readwrite("x", &sc2::Point2DI::x)
        .def_readwrite("y", &sc2::Point2DI::y)
        .def(py::self == py::self)
        .def(py::self != py::self)
		.def("__hash__",
			[](const sc2::Point2DI & p) {
				return py::make_tuple(p.x, p.y).attr("__hash__")();
		})
        .def("__repr__",
            [](const sc2::Point2DI & p) {
                return "<Point2DI: (" + std::to_string(p.x) + ", " + std::to_string(p.y) + ")>";
            })
        .def("__str__", 
            [](const sc2::Point2DI & p) {
                return "(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ")";
            });
}