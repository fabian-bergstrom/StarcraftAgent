#include "library.h"

namespace py = pybind11;

PYBIND11_MODULE(library, m)
{
    m.doc() = "Python API for playing Starcraft II";

    define_typeenums(m);
    define_unit(m);
	define_replay_unit(m);
    define_unittype(m);
    define_util(m);
    define_point(m);
    define_base_location(m);
    define_tech_tree(m);
    define_color(m);
    define_map_tools(m);
    define_building_placer(m);

    // Note: This is not sc2::Coordinator but a small wrapper class which
    // overrides the constructor of sc2::Coordinator, see library.h.
    py::class_<Coordinator>(m, "Coordinator")
        .def(py::init())
        .def(py::init<std::string>())
        .def("set_participants", &sc2::Coordinator::SetParticipants, "participants"_a)
        .def("launch_starcraft", &sc2::Coordinator::LaunchStarcraft)
        .def("start_game", &sc2::Coordinator::StartGame, "map_path"_a)
        .def("update", &sc2::Coordinator::Update)
        .def("set_real_time", &sc2::Coordinator::SetRealtime)
		.def("load_replay_list",&sc2::Coordinator::SetReplayPath, "replay_path"_a)
		.def("add_replay_observer",&sc2::Coordinator::AddReplayObserver, "replay_observer"_a)
		.def("set_replay_perspective",&sc2::Coordinator::SetReplayPerspective, "perspective"_a)
		.def("leave_game", &sc2::Coordinator::LeaveGame)
		;

    py::enum_<sc2::Race>(m, "Race")
        .value("Terran", sc2::Race::Terran)
        .value("Zerg", sc2::Race::Zerg)
        .value("Protoss", sc2::Race::Protoss)
        .value("Random", sc2::Race::Random);

    m.attr("PLAYER_SELF")    = py::int_((int) Players::Self);
    m.attr("PLAYER_ENEMY")   = py::int_((int) Players::Enemy);
    m.attr("PLAYER_NEUTRAL") = py::int_((int) Players::Neutral);
    m.attr("PLAYER_ALLY")    = py::int_((int) Players::Ally);

    py::class_<sc2::BuffID>(m, "BuffID")
        .def(py::init<sc2::BUFF_ID>());

    py::implicitly_convertible<sc2::BUFF_ID, sc2::BuffID>();

    py::class_<sc2::UnitTypeID>(m, "UnitTypeID")
        .def(py::init<sc2::UNIT_TYPEID>())
        .def("__eq__", [](const sc2::UnitTypeID &value, sc2::UNIT_TYPEID &value2) { return value == value2; });

    py::implicitly_convertible<sc2::UNIT_TYPEID, sc2::UnitTypeID>();

    py::class_<sc2::UpgradeID>(m, "UpgradeID")
        .def(py::init<sc2::UPGRADE_ID>());

    py::implicitly_convertible<sc2::UPGRADE_ID, sc2::UpgradeID>();

	py::class_<sc2::EffectID>(m, "EffectID")
		.def(py::init<sc2::EFFECT_ID>());

	py::implicitly_convertible<sc2::EFFECT_ID, sc2::EffectID>();

    py::class_<sc2::AbilityID>(m, "AbilityID")
        .def(py::init<sc2::ABILITY_ID>());

    py::implicitly_convertible<sc2::ABILITY_ID, sc2::AbilityID>();
    py::implicitly_convertible<sc2::AbilityID, sc2::ABILITY_ID>();

    py::class_<sc2::Agent>(m, "Agent")
        .def(py::init());

    // IDABot is a specialization of Agent
	py::class_<IDABot, PyIDABot, sc2::Agent>(m, "IDABot")
		.def(py::init())
		.def("on_game_start", &IDABot::OnGameStart)
		.def("on_step", &IDABot::OnStep)
		.def("send_chat", &IDABot::SendChat, "Sends the string 'message' to the game chat", "message"_a)
		.def("get_all_units", &IDABot::GetAllUnits, "Returns a list of all visible units, including minerals and geysers")
		.def("get_my_units", &IDABot::GetMyUnits, "Returns a list of all your units") 
		.def("get_player_race", &IDABot::GetPlayerRace, "Returns the players race, useful if you play Race.Random")
		.def("debug_create_unit", &IDABot::DebugCreateUnit, "This method creates the nr (INT) of units on the position :class:`library.Point2D`, the unit belongs to the Player Constant", "unit_type"_a, "p"_a, "player_id"_a = 0, "count"_a = 1)
		.def("debug_kill_unit", &IDABot::DebugKillUnit, "Kill the unit from debug mode")
		.def("debug_show_map", &IDABot::DebugShowMap, "Show the entire map through debug mode")
		.def("debug_fast_build", &IDABot::DebugFastBuild, "Set build time in game to 1 through debug mode")
		.def("debug_enemy_control", &IDABot::DebugEnemyControl, "Control the enemy through debug mode")
		.def("debug_ignore_food", &IDABot::DebugIgnoreFood, "Ignore the food in game through debug mode")
		.def("debug_ignore_resource_cost", &IDABot::DebugIgnoreResourceCost, "Ignore the resource cost in game, making, everything cost zero resources through debug mode")
		.def("debug_give_all_resources", &IDABot::DebugGiveAllResources, "Set the mineral and vespene gas to 5000 through debug mode")
		.def("debug_god_mode", &IDABot::DebugGodMode, "Give the player god mode")
		.def("debug_ignore_mineral", &IDABot::DebugIgnoreMineral, "Ignore the mineral cost in game through debug mode")
		.def("debug_no_cooldowns", &IDABot::DebugNoCooldowns, "Deactive cooldowns (Basically setting them to 0) through debug mode")
		.def("debug_give_all_tech", &IDABot::DebugGiveAllTech, "Give all the tech to the player through debug mode")
		.def("debug_give_all_upgrades", &IDABot::DebugGiveAllUpgrades, "Give all the upgrades to the player through debug mode")
		.def("debug_set_score", &IDABot::DebugSetScore, "Set the Players score in game through debug mode")
		.def("debug_end_game", &IDABot::DebugEndGame, "End the game through debug mode. If the Boolean is True then victory. If False, defeat.")
		.def("debug_set_energy", &IDABot::DebugSetEnergy, "Set the energy on a unit through debug mode")
		.def("debug_set_life", &IDABot::DebugSetLife, "Set the life on a unit through debug mode")
		.def("debug_set_shields", &IDABot::DebugSetShields, "Set the shields on a unit through debug mode")
		.def("get_enemy_base_location", &IDABot::GetEnemyBaseLocations, "Return the CCposition of the enemy base")
		.def("move_camera", &IDABot::CameraMove, "Move the camera to the position", "p"_a)
		.def("has_creep", &IDABot::HasCreep, "Returns true if there is creep at position p", "p"_a)
		.def("ability_for_upgrade", &IDABot::abilityForUpgrade, "Ability that researches this upgrade", "upgrade"_a)
		.def("upgrade_mineral_cost", &IDABot::UpgradeMineralCost, "Mineral cost of researching the upgrade", "upgrade"_a)
		.def("upgrade_gas_cost", &IDABot::UpgradeGasCost, "Vespene/gas cost of researching the upgrade", "upgrade"_a)
		.def("upgrade_research_time", &IDABot::UpgradeResearchTime, "Time in GameLoops to research this upgrade", "upgrade"_a)
		.def("effect_radius", &IDABot::RadiusEffect, "Size of the circle the effect impacts", "effect"_a)
		.def_property_readonly("base_location_manager", &IDABot::Bases, "An instance of the class :class:`library.BaseLocationManager`")
		.def_property_readonly("tech_tree", &IDABot::GetTechTree, "An instance of the class :class:`library.TechTree`")
		.def_property_readonly("map_tools", &IDABot::Map, "An instance of the class :class:`library.MapTools`")
		.def_property_readonly("building_placer", &IDABot::GetBuildingPlacer, "An instance of the class :class:`library.BuildingPlacer`")
		.def_property_readonly("start_location", &IDABot::GetStartLocation, "CCPosition representing the start location, note that it is the depot position that is returned.")
		.def_property_readonly("start_locations", &IDABot::GetStartLocations, "List of CCPositions representing the start locations, note that it is the depot positions and not the center positions")
		.def_property_readonly("minerals", &IDABot::GetMinerals, "How much minerals we currently have")
		.def_property_readonly("current_supply", &IDABot::GetCurrentSupply, "How much supply we are currently using")
		.def_property_readonly("max_supply", &IDABot::GetMaxSupply, "How much supply we can currently use")
		.def_property_readonly("gas", &IDABot::GetGas, "How much gas we currently have")
		.def_property_readonly("current_frame", &IDABot::GetCurrentFrame, "Which frame we are currently on");




	// API extended summer 2020



	py::class_<sc2::ReplayObserver>(m, "ReplayObserver")
		.def(py::init());

	py::class_<IDAReplayObserver, PyReplayObserver, sc2::ReplayObserver>(m, "IDAReplayObserver")
		.def(py::init())
		.def("on_game_start", &IDAReplayObserver::OnGameStart)
		.def("on_step", &IDAReplayObserver::OnStep)
		.def("on_game_end", &IDAReplayObserver::OnGameEnd)
		.def("get_all_units", &IDAReplayObserver::GetAllUnits, "Returns a list of all units")
		.def("get_player_race", &IDAReplayObserver::GetPlayerRace,"player_id"_a)
		.def("get_replay_path", &IDAReplayObserver::GetReplayPath)
		.def("get_result_for_player", &IDAReplayObserver::GetResultForPlayer, "player_id"_a)
		.def("on_unit_destroyed", &IDAReplayObserver::OnReplayUnitDestroyed, "unit"_a)
		.def("on_unit_created", &IDAReplayObserver::OnReplayUnitCreated, "unit"_a)
		.def_property_readonly("tech_tree", &IDAReplayObserver::GetTechTree)
		;

	

	py::enum_<sc2::GameResult>(m, "GameResult")
		.value("Win", sc2::GameResult::Win)
		.value("Loss", sc2::GameResult::Loss)
		.value("Tie", sc2::GameResult::Tie)
		.value("Undecided", sc2::GameResult::Undecided);

    py::class_<sc2::PlayerSetup>(m, "PlayerSetup");

    py::enum_<sc2::Difficulty>(m, "Difficulty")
        .value("VeryEasy", sc2::Difficulty::VeryEasy)
        .value("Easy", sc2::Difficulty::Easy)
        .value("Medium", sc2::Difficulty::Medium)
        .value("MediumHard", sc2::Difficulty::MediumHard)
        .value("Hard", sc2::Difficulty::Hard)
        .value("HardVeryHard", sc2::Difficulty::HardVeryHard)
        .value("VeryHard", sc2::Difficulty::VeryHard)
        .value("CheatVision", sc2::Difficulty::CheatVision)
        .value("CheatMoney", sc2::Difficulty::CheatMoney)
        .value("CheatInsane", sc2::Difficulty::CheatInsane);

	py::enum_<sc2::AIBuild>(m, "AIBuild")
		.value("RandomBuild", sc2::AIBuild::RandomBuild)
		.value("Rush", sc2::AIBuild::Rush)
		.value("Timing", sc2::AIBuild::Timing)
		.value("Power", sc2::AIBuild::Power)
		.value("Macro", sc2::AIBuild::Macro)
		.value("Air", sc2::AIBuild::Air);


	m.def("create_participants", &sc2::CreateParticipant, "Create participant from bot", "race"_a, "bot"_a, "player_name"_a = "");
	m.def("create_computer", &sc2::CreateComputer, "Create participant from built-in Starcraft computer", "race"_a, "difficulty"_a, "ai_build"_a = sc2::AIBuild::RandomBuild, "player_name"_a = "");

    /*
    py::class_<BuildDescription>(m, "BuildDescription")
        .def(py::init())
        .def_readwrite("producer_type", &BuildDescription::producer_type)
        .def_readwrite("result_type", &BuildDescription::result_type)
        .def_readwrite("ability_used", &BuildDescription::ability_used)
        .def_readwrite("buildings_needed", &BuildDescription::buildings_needed)
        .def_readwrite("addons_needed", &BuildDescription::addons_needed);

    py::class_<TechTreeImproved>(m, "TechTreeImproved")
        .def(py::init())
        .def("load_data", &TechTreeImproved::LoadData)
        .def("how_to_build", &TechTreeImproved::HowToBuild);
    */
}
