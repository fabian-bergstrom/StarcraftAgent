#include "BaseLocationManager.h"
#include "Util.h"

#include "IDABot.h"

BaseLocationManager::BaseLocationManager(IDABot & bot)
    : m_bot(bot)
{
    
}

void BaseLocationManager::onStart()
{
    m_tileBaseLocations = std::vector<std::vector<BaseLocation *>>(m_bot.Map().width(), std::vector<BaseLocation *>(m_bot.Map().height(), nullptr));
    m_playerStartingBaseLocations[Players::Self]  = nullptr;
    m_playerStartingBaseLocations[Players::Enemy] = nullptr; 
    
    // a BaseLocation will be anything where there are minerals to mine
    // so we will first look over all minerals and cluster them based on some distance
    const CCPositionType clusterDistance = Util::TileToPosition(12);
    
    // stores each cluster of resources based on some ground distance
    std::vector<std::vector<Unit>> resourceClusters;
    for (auto & mineral : m_bot.GetAllUnits())
    {
        // skip minerals that don't have more than 100 starting minerals
        // these are probably stupid map-blocking minerals to confuse us
        if (!mineral.getType().isMineral())
        {
            continue;
        }

        bool foundCluster = false;
        for (auto & cluster : resourceClusters)
        {
            float dist = Util::Dist(mineral, Util::CalcCenter(cluster));
            
            // quick initial air distance check to eliminate most resources
            if (dist < clusterDistance)
            {
                // now do a more expensive ground distance check
                float groundDist = dist; //m_bot.Map().getGroundDistance(mineral.pos, Util::CalcCenter(cluster));
                if (groundDist >= 0 && groundDist < clusterDistance)
                {
                    cluster.push_back(mineral);
                    foundCluster = true;
                    break;
                }
            }
        }

        if (!foundCluster)
        {
            resourceClusters.push_back(std::vector<Unit>());
            resourceClusters.back().push_back(mineral);
        }
    }

    // add geysers only to existing resource clusters
    for (auto & geyser : m_bot.GetAllUnits())
    {
        if (!geyser.getType().isGeyser())
        {
            continue;
        }

        for (auto & cluster : resourceClusters)
        {
            //int groundDist = m_bot.Map().getGroundDistance(geyser.pos, Util::CalcCenter(cluster));
            float groundDist = Util::Dist(geyser, Util::CalcCenter(cluster));

            if (groundDist >= 0 && groundDist < clusterDistance)
            {
                cluster.push_back(geyser);
                break;
            }
        }
    }

    // add the base locations if there are more than 4 resouces in the cluster
    int baseID = 0;
    for (auto & cluster : resourceClusters)
    {
        if (cluster.size() > 4)
        {
            m_baseLocationData.push_back(BaseLocation(m_bot, baseID++, cluster));
        }
    }

    // construct the vectors of base location pointers, this is safe since they will never change
    for (auto & baseLocation : m_baseLocationData)
    {
        m_baseLocationPtrs.push_back(&baseLocation);

        // if it's a start location, add it to the start locations
        if (baseLocation.isStartLocation())
        {
            m_startingBaseLocations.push_back(&baseLocation);
        }

        // if it's our starting location, set the pointer
        if (baseLocation.isPlayerStartLocation(Players::Self))
        {
            m_playerStartingBaseLocations[Players::Self] = &baseLocation;
        }

        if (baseLocation.isPlayerStartLocation(Players::Enemy))
        {
            m_playerStartingBaseLocations[Players::Enemy] = &baseLocation;
        }
    }

    // construct the map of tile positions to base locations
    for (int x=0; x < m_bot.Map().width(); ++x)
    {
        for (int y=0; y < m_bot.Map().height(); ++y)
        {
            for (auto & baseLocation : m_baseLocationData)
            {
                CCPosition pos(Util::TileToPosition(x + 0.5f), Util::TileToPosition(y + 0.5f));

                if (baseLocation.containsPosition(pos))
                {
                    m_tileBaseLocations[x][y] = &baseLocation;
                    
                    break;
                }
            }
        }
    }

    // construct the sets of occupied base locations
    m_occupiedBaseLocations[Players::Self] = std::set<const BaseLocation *>();
    m_occupiedBaseLocations[Players::Enemy] = std::set<const BaseLocation *>();
}

void BaseLocationManager::onFrame()
{   
    drawBaseLocations();

	// make sure that mineralfields and geysers are up to date in the different bases
	for (BaseLocation & baselocation : m_baseLocationData) {
		updateMinerals(baselocation);
		updateGeysers(baselocation);
	}

    // reset the player occupation information for each location
    for (auto & baseLocation : m_baseLocationData)
    {
		for(int i = 0; i < Players::Size; ++i)
			baseLocation.setPlayerOccupying(i, false);
    }

    // for each unit on the map, update which base location it may be occupying
    for (auto & unit : m_bot.UnitInfo().getUnits(Players::Self))
    {
        // we only care about buildings on the ground
        if (!unit.getType().isBuilding() || unit.isFlying())
        {
            continue;
        }

        BaseLocation * baseLocation = getBaseLocation(unit.getPosition());

        if (baseLocation != nullptr)
        {
            baseLocation->setPlayerOccupying(unit.getPlayer(), true);
        }
    }

    // update enemy base occupations
    for (const auto & kv : m_bot.UnitInfo().getUnitInfoMap(Players::Enemy))
    {
        const UnitInfo & ui = kv.second;

        if (!m_bot.Data(ui.type).isBuilding)
        {
            continue;
        }

        BaseLocation * baseLocation = getBaseLocation(ui.lastPosition);

        if (baseLocation != nullptr)
        {
            baseLocation->setPlayerOccupying(Players::Enemy, true);
        }
    }

    // update the starting locations of the enemy player
    // this will happen one of two ways:
    
    // 1. we've seen the enemy base directly, so the baselocation will know
    if (m_playerStartingBaseLocations[Players::Enemy] == nullptr)
    {
        for (auto & baseLocation : m_baseLocationData)
        {
            if (baseLocation.isPlayerStartLocation(Players::Enemy))
            {
                m_playerStartingBaseLocations[Players::Enemy] = &baseLocation;
            }
        }
    }
    
    // 2. we've explored every other start location and haven't seen the enemy yet
    if (m_playerStartingBaseLocations[Players::Enemy] == nullptr)
    {
        int numStartLocations = (int)getStartingBaseLocations().size();
        int numExploredLocations = 0;
        BaseLocation * unexplored = nullptr;

        for (auto & baseLocation : m_baseLocationData)
        {
            if (!baseLocation.isStartLocation())
            {
                continue;
            }

            if (baseLocation.isExplored())
            {
                numExploredLocations++;
            }
            else
            {
                unexplored = &baseLocation;
            }
        }

        // if we have explored all but one location, then the unexplored one is the enemy start location
        if (numExploredLocations == numStartLocations - 1 && unexplored != nullptr)
        {
            m_playerStartingBaseLocations[Players::Enemy] = unexplored;
            unexplored->setPlayerOccupying(Players::Enemy, true);
        }
    }

    // update the occupied base locations for each player
    m_occupiedBaseLocations[Players::Self] = std::set<const BaseLocation *>();
    m_occupiedBaseLocations[Players::Enemy] = std::set<const BaseLocation *>();
    for (auto & baseLocation : m_baseLocationData)
    {
        if (baseLocation.isOccupiedByPlayer(Players::Self))
        {
            m_occupiedBaseLocations[Players::Self].insert(&baseLocation);
        }

        if (baseLocation.isOccupiedByPlayer(Players::Enemy))
        {
            m_occupiedBaseLocations[Players::Enemy].insert(&baseLocation);
        }
    }

    // draw the debug information for each base location
    
}

void BaseLocationManager::updateMinerals(BaseLocation & baseToUpdate) {
	std::vector<Unit> newMinerals;
	for (Unit oldMineral : baseToUpdate.getMinerals()) {
		for (auto & mineral : m_bot.GetAllUnits()) {
			if (oldMineral.getPosition() == mineral.getPosition() && mineral.getType().isMineral()) {
				newMinerals.push_back(mineral);
				break;
			}
		}
	}
	baseToUpdate.setMinerals(newMinerals);
}

void BaseLocationManager::updateGeysers(BaseLocation & baseToUpdate) {
	std::vector<Unit> newGeysers;
	for (Unit oldGeyser : baseToUpdate.getGeysers()) {
		for (auto & geyser : m_bot.GetAllUnits()) {
			if (oldGeyser.getPosition() == geyser.getPosition() && geyser.getType().isGeyser()) {
				newGeysers.push_back(geyser);
				break;
			}
		}
	}
	baseToUpdate.setGeysers(newGeysers);
}

BaseLocation * BaseLocationManager::getBaseLocation(const CCPosition & pos) const
{
    if (!m_bot.Map().isValidPosition(pos)) { return nullptr; }

#ifdef SC2API
    return m_tileBaseLocations[(int)pos.x][(int)pos.y];
#else
    return m_tileBaseLocations[pos.x / 32][pos.y / 32];
#endif
}

void BaseLocationManager::drawBaseLocations()
{
    for (auto & baseLocation : m_baseLocationData)
    {
        baseLocation.draw();
    }

    // draw a purple sphere at the next expansion location
    const BaseLocation * next_expansion = getNextExpansion(Players::Self);
    CCTilePosition nextExpansionPosition = next_expansion->getDepotPosition();

    m_bot.Map().drawResourceSphere(sc2::Point3D(Util::GetPosition(nextExpansionPosition).x, Util::GetPosition(nextExpansionPosition).y, next_expansion->getMinerals()[0].getUnitPtr()->pos.z), 1, CCColor(255, 0, 255));
    m_bot.Map().drawText(Util::GetPosition(nextExpansionPosition), "Next Expansion Location", CCColor(255, 0, 255));
}

const std::vector<const BaseLocation *> & BaseLocationManager::getBaseLocations() const
{
    return m_baseLocationPtrs;
}

const std::vector<const BaseLocation *> & BaseLocationManager::getStartingBaseLocations() const
{
    return m_startingBaseLocations;
}

const BaseLocation * BaseLocationManager::getPlayerStartingBaseLocation(int player) const
{
    return m_playerStartingBaseLocations.at(player);
}

const std::set<const BaseLocation *> & BaseLocationManager::getOccupiedBaseLocations(int player) const
{
    return m_occupiedBaseLocations.at(player);
}

const BaseLocation * BaseLocationManager::getNextExpansion(int player) const
{
    const BaseLocation * homeBase = getPlayerStartingBaseLocation(player);
    const BaseLocation * closestBase = nullptr;
    int minDistance = std::numeric_limits<int>::max();

    CCPosition homeTile = homeBase->getPosition();
    
    for (auto & base : getBaseLocations())
    {
        // skip mineral only and starting locations (TODO: fix this)
        if (base->isMineralOnly() || base->isStartLocation())
        {
            continue;
        }

        // get the tile position of the base
        auto tile = base->getDepotPosition();
        
        UnitType base_type = Util::GetTownHall(m_bot.GetPlayerRace(Players::Self), m_bot);
        bool canBuildBaseAtLocation = m_bot.Map().canBuildTypeAtPosition(tile.x, tile.y, base_type);

        if (!canBuildBaseAtLocation)
        {
            continue;
        }

        // the base's distance from our main nexus
        int distanceFromHome = homeBase->getGroundDistance(tile);

        // if it is not connected, continue
        if (distanceFromHome < 0)
        {
            continue;
        }

        if (!closestBase || distanceFromHome < minDistance)
        {
            closestBase = base;
            minDistance = distanceFromHome;
        }
    }

    return closestBase;
}
