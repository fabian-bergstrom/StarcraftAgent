#include "MyAgent.h"

MyAgent::MyAgent()
{
    military_goal[UnitType(sc2::UNIT_TYPEID::TERRAN_MARINE, *this)] = 30;
}

void MyAgent::OnGameStart()
{
    IDABot::OnGameStart();

    sc2::UnitTypeID wanted_type = sc2::UNIT_TYPEID::TERRAN_GHOST;
    sc2::UnitTypeData wanted_data = Observation()->GetUnitTypeData()[wanted_type];
    std::cout << "Looking up: " << wanted_data.name << std::endl;
    sc2::UnitTypeData requirement = Observation()->GetUnitTypeData()[wanted_data.tech_requirement];
    std::cout << "Found tech requirement: " << requirement.name << "(" << wanted_data.tech_requirement << ")" << std::endl;
    std::cout << std::boolalpha;
    std::cout << "Require attached (Is addon?): " << wanted_data.require_attached << std::endl;

    std::cout << "Found ability_id" << wanted_data.ability_id << std::endl;
    // TODO: What/who can use this ability??

    sc2::AbilityData ability_to_create = Observation()->GetAbilityData()[wanted_data.ability_id];
    std::cout << "Button name for ability " << ability_to_create.button_name << std::endl;
}

void MyAgent::OnStep()
{
    IDABot::OnStep();
	// This is the entry point of the bot.
	// This function is called every time the game loop is run.

	//if (!Bases().getPlayerStartingBaseLocation(Players::Enemy)->isExplored())
	//{
	//	assignScout();
	//}

    std::vector<UnitType> build_plan = CreateBuildPlan();
	manageWorkers(build_plan);
    manageBuilding(build_plan);
}

void MyAgent::manageBuilding(std::vector<UnitType> & build_plan)
{
    std::map<UnitType, int> currently_being_built = numberOfTypeBeingBuilt();

    for (auto & pair : currently_being_built)
        std::cout << "Building " << pair.second << " of " << pair.first.getName() << std::endl;

    int minerals = GetMinerals();
    int gas = GetGas();
    // TODO: Supply

    for (UnitType type : build_plan)
    {
        if (currently_being_built[type] > 0)
        {
            currently_being_built[type]--;
            std::cout << "Already building " << type.getName() << std::endl;
            continue;
        }

        // If we cannot afford the next thing, we don't want to build at all
        if (type.mineralPrice() > minerals || type.gasPrice() > gas)
        {
            break;
        }

        std::vector<Unit> producers = getProducer(MetaType(type, *this));

        // Reserve some resources
        // TODO: Only reserve resources if the corresponding worker is in BuildingWalking
        minerals -= type.mineralPrice();
        gas -= type.gasPrice();

        if (!producers.empty())
        {
            if (type.isBuilding())
            {
                for (Unit & worker : getWorkers())
                {
                    if (isFree(worker))
                    {
                        CCTilePosition position = getBuildPosition(type);
                        BuildStatus status{ type, position };
                        currently_building[worker] = status;
                        // Reserve the location
                        GetBuildingPlacer().reserveTiles(position.x, position.y, type.tileWidth(), type.tileHeight());

                        // Update economy book-keeping
                        if (type.isRefinery() || type.isResourceDepot())
                        {
                            economy_spending.minerals += type.mineralPrice();
                            economy_spending.gas += type.gasPrice();
                        }
                        else if (type.supplyProvided() == 0)
                        {
                            military_spending.minerals += type.mineralPrice();
                            military_spending.gas += type.gasPrice();
                        }
                        assignWork(worker, Assignment::BuildWalking);
                        return;
                    }
                }
            }
            else if (type.isCombatUnit() || type.isWorker())
            {
                // TODO: Remove code-duplication
                // Update economy book-keeping
                if (type.isWorker())
                {
                    economy_spending.minerals += type.mineralPrice();
                    economy_spending.gas += type.gasPrice();
                }
                else
                {
                    military_spending.minerals += type.mineralPrice();
                    military_spending.gas += type.gasPrice();
                }
                std::cout << "Training unit " << type.getName() << std::endl;
                producers.front().train(type);
                return;
            }
        }
    }
}

bool MyAgent::isFree(Unit & worker)
{
    if (workerAssignment.count(worker) > 0)
    {
        return workerAssignment[worker] == Assignment::Mineral;
    }
    else
    {
        return true;
    }
}

std::vector<UnitType> MyAgent::CreateBuildPlan()
{
    // Count the total number of minerals, including all bases
    size_t number_of_minerals{ 0 };
    for (const BaseLocation * base : Bases().getOccupiedBaseLocations(Players::Self))
    {
        number_of_minerals += base->getMinerals().size();
    }
    size_t number_of_workers = getWorkers().size();
    size_t available_food = GetMaxSupply() - GetCurrentSupply();

    std::vector<UnitType> build_plan;

    std::cout << "Military spending (minerals) " << military_spending.minerals << std::endl;
    std::cout << "Economy spending (minerals) " << economy_spending.minerals << std::endl;

    // TODO: Make a better decision here
    if ((500 + military_spending.minerals) < economy_spending.minerals)
    {
        CreateMaximizeMilitaryBuildPlan(build_plan, available_food);
        CreateMaximizeEconomyBuildPlan(number_of_minerals, number_of_workers, available_food, build_plan);
    }
    else
    {
        CreateMaximizeEconomyBuildPlan(number_of_minerals, number_of_workers, available_food, build_plan);
        CreateMaximizeMilitaryBuildPlan(build_plan, available_food);
    }

    for (size_t i{}; i < build_plan.size() && i < 10; ++i)
    {
        std::cout << build_plan[i].getName() << ", ";
    }
    std::cout << std::endl;

    return build_plan;
}

void MyAgent::CreateMaximizeMilitaryBuildPlan(std::vector<UnitType> &build_plan, size_t &available_food)
{
    for (auto & pair : military_goal)
    {
        // How many do we already have?
        size_t units_of_type = GetUnits(pair.first).size();

        // First, do we meet the military goal for this UnitType?
        if (units_of_type >= pair.second) {
            continue;
        }

        // Second, can we produce anything of UnitType?
        std::vector<Unit> producers = getProducer(MetaType(pair.first, *this), true);
        // TODO: Don't build too many Barracks if we don't need to
        if (producers.empty())
        {
            // TODO: Calculate what we need to build using the TechTree
            // For now, let's build a barrack
            build_plan.push_back(UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, *this));
        }
        else
        {
            // TODO: This is never run, or is it??
            int units_needed = pair.second - units_of_type;
            int food_required = pair.first.supplyRequired();
            for (int i{}; i < units_needed; ++i)
            {
                if (available_food >= food_required)
                {
                    return;
                }
                else
                {
                    available_food -= food_required;
                    build_plan.push_back(pair.first);

                }
            }
        }
    }
}

void MyAgent::CreateMaximizeEconomyBuildPlan(size_t &number_of_minerals, size_t &number_of_workers, size_t &available_food, std::vector<UnitType> &build_plan)
{
    const int WORKER_PER_MINERAL_DEPOSIT = 3;
    const int FOOD_THRESHOLD_BUILD_DEPOT = 1;

    // Plan for the two next base expansions
    for (int i = 0; i < 2; i++)
    {
        while (WORKER_PER_MINERAL_DEPOSIT * number_of_minerals > number_of_workers)
        {
            if (available_food == FOOD_THRESHOLD_BUILD_DEPOT)
            {
                UnitType type{ sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT, *this };
                build_plan.push_back(type);

                available_food += type.supplyProvided(); // TODO: Maybe 16 sometimes? (+16 with Calldown: Supplies)
            }
            else
            {
                build_plan.push_back(UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, *this));
                available_food--;
                number_of_workers++;
            }
        }

        UnitType type{ sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, *this };
        build_plan.push_back(type);
        available_food += type.supplyProvided();

        const BaseLocation * next_expansion = Bases().getNextExpansion(Players::Self);
        if (next_expansion)
        {
            number_of_minerals += next_expansion->getMinerals().size();
        }
    }
}

void MyAgent::assignScout()
{
	// Assumes that at least one worker is unassigned
	for (Unit worker : getWorkers())
	{
		if (workerAssignment.count(worker) == 0)
		{
			assignWork(worker, Assignment::Scout);
			std::cout << "Assigned worker to Scout" << std::endl;
			break;
		}
	}
}

void MyAgent::manageWorkers(std::vector<UnitType> & build_plan)
{
	std::vector<Unit> workers = getWorkers();
	for (Unit worker : workers)
	{
		// If the worker has does not have an assignment
		if (workerAssignment.count(worker) == 0)
		{
			assignWork(worker, Assignment::Mineral);
		}
		else
		{
            CCTilePosition position;
			switch (workerAssignment[worker]) {
				case Assignment::Mineral:
				case Assignment::Gas:
					// Never change let the gas/mineral workers rest
					break;
				case Assignment::Scout:
					if (Bases().getPlayerStartingBaseLocation(Players::Enemy)->isExplored()) {
						assignWork(worker, Assignment::Mineral);
					}
					break;
                case Assignment::BuildWalking:
                    position = currently_building[worker].position;
                    if (position == Util::GetTilePosition(worker.getPosition()))
                    {
                        assignWork(worker, Assignment::BuildBuilding);
                    }
                    break;
                case Assignment::BuildBuilding:
                    // Probably done, 
                    if (worker.isIdle())
                    {
                        BuildStatus status = currently_building[worker];
                        GetBuildingPlacer().freeTiles(status.position.x, status.position.y, status.type.tileWidth(), status.type.tileHeight());
                        currently_building.erase(worker);
                        assignWork(worker, Assignment::Mineral);
                    }
                    break;
			}
		}
	}
}

void MyAgent::assignWork(Unit & worker, Assignment assignment)
{
	// Assigns worker to assignment
	workerAssignment[worker] = assignment;
	Unit mineral;
	const BaseLocation * enemyBaseLocation;
    CCTilePosition position;
    AssignmentData data;
    const BaseLocation * assigned_base;

	switch (assignment) {
	case Assignment::Mineral:
		// Select which base to mine for
        assigned_base = AssignBase(worker);
		mineral = getClosestMineral(assigned_base->getPosition());
		worker.rightClick(mineral);
		break;
	case Assignment::Scout:
		 enemyBaseLocation = Bases().getPlayerStartingBaseLocation(Players::Enemy);

		if (enemyBaseLocation)
		{
			std::cout << "Enemy base location known!" << std::endl;
			worker.move(enemyBaseLocation->getPosition());
		}
		else
		{
			std::cout << "Enemy base location unknown!" << std::endl;

			for (const BaseLocation * location : Bases().getStartingBaseLocations())
			{
				if (!Map().isExplored(location->getPosition()))
				{
					worker.move(location->getPosition());
					break;
				}
			}
		}
		break;
    case Assignment::BuildWalking:
        worker.move(currently_building[worker].position);
        break;
    case Assignment::BuildBuilding:
        worker.build(currently_building[worker].type, currently_building[worker].position);
        break;
	}

}

std::map<UnitType, int> MyAgent::numberOfTypeBeingBuilt() const
{
    std::map<UnitType, int> being_built;
    for (auto & pair : currently_building)
    {
        being_built[pair.second.type]++;
    }
    return being_built;
}

const BaseLocation * MyAgent::AssignBase(Unit & worker)
{
    std::set<const BaseLocation *> base_locations = Bases().getOccupiedBaseLocations(Players::Self);
    int most_needed_workers_value{std::numeric_limits<int>().min()};
    const BaseLocation * most_needed_workers_pointer{};
    
    for (const BaseLocation * base_location : base_locations)
    {
        int workers_needed = 3 * base_location->getMinerals().size();
        int number_of_workers;
        if (base_assignment.count(base_location) > 0)
        {
            std::vector<Unit> & workers = base_assignment[base_location];
            number_of_workers = workers.size();
        }
        else
        {
            number_of_workers = 0;
            std::vector<Unit> workers;
            base_assignment[base_location] = workers;
        }
        int needed_workers = workers_needed - number_of_workers;
        if (needed_workers > most_needed_workers_value)
        {
            most_needed_workers_value = needed_workers;
            most_needed_workers_pointer = base_location;
        }
    }
    std::vector<Unit> & workers = base_assignment[most_needed_workers_pointer];
    workers.push_back(worker);
    return most_needed_workers_pointer;
}

CCTilePosition MyAgent::getBuildPosition(UnitType & building)
{
    CCTilePosition tile_position;
    if (Util::GetTownHall(GetPlayerRace((int) Players::Self), *this) == building)
    {
        tile_position = Bases().getNextExpansion((int)Players::Self)->getDepotPosition();
    } 
    else
    {
        CCPosition position = Bases().getPlayerStartingBaseLocation(Players::Self)->getPosition();
        tile_position = GetBuildingPlacer().getBuildLocationNear(Util::GetTilePosition(position), building, 1);
    }
    return tile_position;
}

Unit MyAgent::getClosestMineral(const CCPosition & position) const
{
	Unit bestMineral;
	double bestDist = 100000;

	for (auto & mineral : GetAllUnits())
	{
		if (!mineral.getType().isMineral()) continue;

		double dist = Util::Dist(mineral, position);

		if (dist < bestDist)
		{
			bestMineral = mineral;
			bestDist = dist;
		}
	}

	return bestMineral;
}

std::vector<Unit> MyAgent::getWorkers()
{
	std::vector<Unit> workers;

	for (auto & unit : GetMyUnits())
	{
		if (unit.getType().isWorker())
        {
			workers.push_back(unit);
		}
	}

	return workers;
}

std::vector<Unit> MyAgent::getProducer(const MetaType & type, bool includeBusy, bool includeIncomplete)
{
    // get all the types of units that cna build this type
    auto & producerTypes = Data(type).whatBuilds;

    // make a set of all candidate producers
    std::vector<Unit> candidateProducers;
    for (auto unit : UnitInfo().getUnits(Players::Self))
    {
        // reasons a unit can not train the desired type
        if (std::find(producerTypes.begin(), producerTypes.end(), unit.getType()) == producerTypes.end()) { continue; }
        if (!includeIncomplete && !unit.isCompleted()) { continue; }
        if (!includeBusy && Data(unit).isBuilding && unit.isTraining()) { continue; }
        if (unit.isFlying()) { continue; }

        // TODO: if unit is not powered continue
        // TODO: if the type is an addon, some special cases
        // TODO: if the type requires an addon and the producer doesn't have one

        // if we haven't cut it, add it to the set of candidates
        candidateProducers.push_back(unit);
    }

    return candidateProducers;
}

