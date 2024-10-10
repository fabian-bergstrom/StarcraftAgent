#pragma once

#include <pybind11/pybind11.h>
#include <sc2api/sc2_api.h>
#include "../src/IDABot.h"
#include "../src/IDAReplayObserver.h"
#include <iostream>
#include <pybind11/stl.h> /* Automatic conversion from std::vector to Python lists */
#include <pybind11/operators.h> /* Convenient operator support */

// Lets us use "x"_a instead of py::arg("x"), great.
using namespace pybind11::literals;

// Wrapper class since the initialization uses pure argc/argv and these cannot be wrapped into Python correctly
class Coordinator : public sc2::Coordinator
{
public:
    Coordinator() : sc2::Coordinator()
    {
        std::vector<std::string> arguments = { "pycommandcenter" };
        CustomLoadSettings(arguments);
    }

    Coordinator(std::string path) : sc2::Coordinator()
    {	
        std::vector<std::string> arguments = { "pycommandcenter", "-e", path };
        CustomLoadSettings(arguments);
    }

    void CustomLoadSettings(std::vector<std::string> arguments)
    {
        std::vector<char*> argv;
        for (const auto& arg : arguments)
            argv.push_back((char*)arg.data());
        argv.push_back(nullptr);
        LoadSettings(argv.size() - 1, argv.data());

    }
};

class PyIDABot : public IDABot
{
public:
    using IDABot::IDABot;
    void OnGameStart() override
    {
        PYBIND11_OVERLOAD_NAME(
            void,
            IDABot,
            "on_game_start",
            OnGameStart
        );
    }
    void OnStep() override
    {
        PYBIND11_OVERLOAD_NAME(
            void,
            IDABot,
            "on_step",
            OnStep
        );
    }
};


class PyReplayObserver : public IDAReplayObserver
{
public:
	using IDAReplayObserver::IDAReplayObserver;
	void OnGameStart() override
	{
		PYBIND11_OVERLOAD_NAME(
			void,
			IDAReplayObserver,
			"on_game_start",
			OnGameStart
		);
	}
	void OnStep() override
	{
		PYBIND11_OVERLOAD_NAME(
			void,
			IDAReplayObserver,
			"on_step",
			OnStep
		);
	}
	void OnGameEnd() override
	{
		PYBIND11_OVERLOAD_NAME(
			void,
			IDAReplayObserver,
			"on_game_end",
			OnGameEnd

		);
	}
	void OnReplayUnitDestroyed(const ReplayUnit *unit) override
	{
		PYBIND11_OVERLOAD_NAME(
			void,
			IDAReplayObserver,
			"on_unit_destroyed",
			OnReplayUnitDestroyed,
			unit
		);
	}
	void OnReplayUnitCreated(const ReplayUnit *unit) override
	{
		PYBIND11_OVERLOAD_NAME(
			void,
			IDAReplayObserver,
			"on_unit_created",
			OnReplayUnitCreated,
			unit
		);
	}
	
};

// The functions below are all defined in different .cpp files, in order 
// to keep compilation snappy
void define_typeenums(pybind11::module & m);
void define_unit(pybind11::module & m);
void define_replay_unit(pybind11::module & m);
void define_unittype(pybind11::module &m);
void define_util(pybind11::module &m);
void define_point(pybind11::module &m);
void define_base_location(pybind11::module & m);
void define_tech_tree(pybind11::module & m);
void define_color(pybind11::module & m);
void define_map_tools(pybind11::module & m);
void define_building_placer(pybind11::module & m);
