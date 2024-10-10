#pragma once

#include "IDABot.h"

class MyAgent : public IDABot
{
    // Worker management
	enum Assignment {
		Scout,
		Mineral,
		Gas,
        BuildWalking,
        BuildBuilding
	};

    struct AssignmentData {
        // When assigned to building, this corresponds to the location for the building
        CCTilePosition buildGoal;
    };

    // Building management

    // Worker assignment book-keeping
	std::map<Unit, Assignment> workerAssignment;

    // When workers are assigned to either minerals or gas, they get assigned a base
    std::map<const BaseLocation *, std::vector<Unit>> base_assignment;

    std::vector<UnitType> build_order;  // TODO: Not used

    std::vector<UnitType> CreateBuildPlan();
    void CreateMaximizeMilitaryBuildPlan(std::vector<UnitType> &build_plan, size_t &available_food);
    void CreateMaximizeEconomyBuildPlan(size_t &number_of_minerals, size_t &number_of_workers, size_t &available_food, std::vector<UnitType> &build_plan);
	void assignScout();
	void manageWorkers(std::vector<UnitType> & build_plan);
    void manageBuilding(std::vector<UnitType> & build_plan);
    bool isFree(Unit & worker);
	void assignWork(Unit & worker, Assignment assignment);
    std::map<UnitType, int> numberOfTypeBeingBuilt() const;
    const BaseLocation * AssignBase(Unit & worker);
    CCTilePosition getBuildPosition(UnitType & building);
	Unit getClosestMineral(const CCPosition & unit) const;

    struct BuildStatus
    {
        UnitType type;
        CCTilePosition position;
        int idle;
    };
    std::map<Unit, BuildStatus> currently_building;

    // Military management
    std::map<UnitType, int> military_goal;

    // Economy
    struct Resources
    {
        int minerals;
        int gas;
    };

    Resources military_spending{};
    Resources economy_spending{};

    // Getters
	std::vector<Unit> getWorkers();

    // Maybe
    std::vector<Unit> getProducer(const MetaType & type, bool includeBusy = false, bool include_incomplete = false);

public:

    MyAgent();

    void OnGameStart() override;
    void OnStep() override;
};