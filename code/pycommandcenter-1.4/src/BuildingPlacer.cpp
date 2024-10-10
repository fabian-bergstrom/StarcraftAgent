#include "Common.h"
#include "BuildingPlacer.h"
#include "IDABot.h"
#include "Util.h"

BuildingPlacer::BuildingPlacer(IDABot & bot)
    : m_bot(bot)
{

}

void BuildingPlacer::onStart()
{
    m_reserveMap = std::vector< std::vector<bool> >(m_bot.Map().width(), std::vector<bool>(m_bot.Map().height(), false));
}

void BuildingPlacer::updateReserved(const std::vector<Unit> & units)
{
	freeAllTiles();
	
	for (Unit unit : units) {
		reserveTiles(unit.getTilePosition().x, unit.getTilePosition().y, unit.getType().tileWidth(), unit.getType().tileHeight());
	}
}

void BuildingPlacer::freeAllTiles() {
	for (int x = 0; x < m_reserveMap.size(); x++) {
		for (int y = 0; y < m_reserveMap[x].size(); y++) {
			m_reserveMap[x][y] = false;
		}
	}
}

bool BuildingPlacer::isInResourceBox(int tileX, int tileY) const
{
    return m_bot.Bases().getPlayerStartingBaseLocation(Players::Self)->isInResourceBox(tileX, tileY);
}

// makes final checks to see if a building can be built at a certain location
bool BuildingPlacer::canBuildHere(int bx, int by, const UnitType & type) const
{
    if (isInResourceBox(bx, by))
    {
        return false;
    }

	int width = type.tileWidth();
	int height = type.tileHeight();
	int xdelta = (int)std::ceil((width - 1.0) / 2);
	int ydelta = (int)std::ceil((height - 1.0) / 2);

    // check the reserve map, that it is a valid tile and not a wall
    for (int x = bx - xdelta; x < bx + width - xdelta; x++)
    {
        for (int y = by - ydelta; y < by + height - ydelta; y++)
        {
            if (!m_bot.Map().isValidTile(x, y) || isReserved(x, y) || !m_bot.Map().isWalkable(x, y))
            {
                return false;
            }
        }
    }

    // if it overlaps a base location return false
    if (tileOverlapsBaseLocation(bx, by, type))
    {
        return false;
    }

    return true;
}

bool BuildingPlacer::canBuildHereWithSize(int bx, int by, int width, int height)
{
	if (isInResourceBox(bx, by))
	{
		return false;
	}

	int xdelta = (int)std::ceil((width - 1.0) / 2);
	int ydelta = (int)std::ceil((height - 1.0) / 2);

	// check the reserve map, that it is a valid tile and not a wall
	for (int x = bx - xdelta; x < bx + width - xdelta; x++)
	{
		for (int y = by - ydelta; y < by + height - ydelta; y++)
		{
			if (!m_bot.Map().isValidTile(x, y) || isReserved(x, y) || !m_bot.Map().isWalkable(x, y))
			{
				return false;
			}
		}
	}

	// Check so it doesn't overlap with a baselocation
	// dimensions of the proposed location

	int tx1 = bx - xdelta;
	int ty1 = by - ydelta;
	int tx2 = tx1 + width - xdelta;
	int ty2 = ty1 + height - ydelta;

	// for each base location
	for (const BaseLocation * base : m_bot.Bases().getBaseLocations())
	{
		// dimensions of the base location
		int bx1 = (int)base->getDepotPosition().x;
		int by1 = (int)base->getDepotPosition().y;
		int bx2 = bx1 + Util::GetTownHall(m_bot.GetPlayerRace(Players::Self), m_bot).tileWidth();
		int by2 = by1 + Util::GetTownHall(m_bot.GetPlayerRace(Players::Self), m_bot).tileHeight();

		// conditions for non-overlap are easy
		bool noOverlap = (tx2 < bx1) || (tx1 > bx2) || (ty2 < by1) || (ty1 > by2);

		// If there is overlap, return false
		if (!noOverlap)
		{
			return false;
		}
	}

	// otherwise there is no overlap
	return true;
}

//returns true if we can build this type of unit here with the specified amount of space.
bool BuildingPlacer::canBuildHereWithSpace(int bx, int by, const UnitType & type, int buildDist) const
{
    //if we can't build here, we of course can't build here with space
    if (!canBuildHere(bx, by, type))
    {
        return false;
    }

    // height and width of the building
    int width  = type.tileWidth();

    int height = type.tileHeight();
	int xdelta = (int)std::ceil((width - 1.0) / 2);
	int ydelta = (int)std::ceil((height - 1.0) / 2);

    // TODO: make sure we leave space for add-ons. These types of units can have addons:

    // define the rectangle of the building spot
    int startx = bx - buildDist - xdelta;
    int starty = by - buildDist - ydelta;
    int endx   = bx + width + buildDist - xdelta;
    int endy   = by + height + buildDist - ydelta;

    // TODO: recalculate start and end positions for addons

    // if this rectangle doesn't fit on the map we can't build here
    if (startx < 0 || starty < 0 || endx > m_bot.Map().width() || endx < bx + width - xdelta || endy > m_bot.Map().height() || endy < by + height - ydelta)
    {
        return false;
    }

	if (!m_bot.Map().canBuildTypeAtPosition(bx, by, type))
	{
		return false;
	}


    // if we can't build here, or space is reserved, or it's in the resource box, we can't build here
    for (int x = startx; x < endx; x++)
    {
        for (int y = starty; y < endy; y++)
        {
            if (!type.isRefinery())
            {
                if (!buildable(type, x, y) || isReserved(x, y) || !m_bot.Map().isWalkable(x, y)) // added isWalkabale in order to check towards walls
                {
                    return false;
                }
            }
        }
    }

    return true;
}

// BuildDist is the distance from the position where the building is gonna be placed.

CCTilePosition BuildingPlacer::getBuildLocationNear(const CCTilePosition & p, const UnitType & t, int buildDist, size_t search_count) const
{
    //Timer t;
    //t.start();

    // get the precomputed vector of tile positions which are sorted closes to this location
    auto & closestToBuilding = m_bot.Map().getClosestTilesTo(p);

    //double ms1 = t.getElapsedTimeInMilliSec();

    // iterate through the list until we've found a suitable location
    for (size_t i(0); i < closestToBuilding.size() && (search_count == 0 || i < search_count); ++i)
    {
        auto & pos = closestToBuilding[i];

        if (canBuildHereWithSpace(pos.x, pos.y, t, buildDist))
        {
            //double ms = t.getElapsedTimeInMilliSec();
            //printf("Building Placer Took %d iterations, lasting %lf ms @ %lf iterations/ms, %lf setup ms\n", (int)i, ms, (i / ms), ms1);

            return pos;
        }
    }

    //double ms = t.getElapsedTimeInMilliSec();
    //printf("Building Placer Failure: %s - Took %lf ms\n", b.type.getName().c_str(), ms);
	std::cout << "Warning! Could not find valid placement for " << t.getName() << " near (" << p.x << ", " << p.y << "). Returning (0, 0) instead.";

    return CCTilePosition(0, 0);
}

bool BuildingPlacer::tileOverlapsBaseLocation(int x, int y, UnitType type) const
{
    // if it's a resource depot we don't care if it overlaps
    if (type.isResourceDepot())
    {
        return false;
    }

    // dimensions of the proposed location

	int xdelta = (int)std::ceil((type.tileWidth() - 1.0) / 2);
	int ydelta = (int)std::ceil((type.tileHeight() - 1.0) / 2);

    int tx1 = x - xdelta;
    int ty1 = y - ydelta;
    int tx2 = tx1 + type.tileWidth() - xdelta;
    int ty2 = ty1 + type.tileHeight() - ydelta;

    // for each base location
    for (const BaseLocation * base : m_bot.Bases().getBaseLocations())
    {
        // dimensions of the base location
        int bx1 = (int)base->getDepotPosition().x;
        int by1 = (int)base->getDepotPosition().y;
        int bx2 = bx1 + Util::GetTownHall(m_bot.GetPlayerRace(Players::Self), m_bot).tileWidth();
        int by2 = by1 + Util::GetTownHall(m_bot.GetPlayerRace(Players::Self), m_bot).tileHeight();

        // conditions for non-overlap are easy
        bool noOverlap = (tx2 < bx1) || (tx1 > bx2) || (ty2 < by1) || (ty1 > by2);

        // if the reverse is true, return true
        if (!noOverlap)
        {
            return true;
        }
    }

    // otherwise there is no overlap
    return false;
}

bool BuildingPlacer::buildable(const UnitType & type, int x, int y) const
{
    // TODO: does this take units on the map into account?
    if (!m_bot.Map().isValidTile(x, y))
    {
        return false;
    }

    // todo: check that it won't block an addon

    return true;
}

void BuildingPlacer::reserveTiles(int bx, int by, int width, int height)
{
	// THIS is never called, that's why spacing doesnt work correctly
    int rwidth = (int)m_reserveMap.size();
    int rheight = (int)m_reserveMap[0].size();

	int xdelta = (int)std::ceil((width - 1.0) / 2);
	int ydelta = (int)std::ceil((height - 1.0) / 2);

    for (int x = bx - xdelta; x < bx + width - xdelta && x < rwidth; x++)
    {
        for (int y = by - ydelta; y < by + height - ydelta && y < rheight; y++)
        {
            m_reserveMap[x][y] = true;
        }
    }
}

void BuildingPlacer::drawReservedTiles()
{
	// Why is there a return here? Should we not use the function? /Hannes J�mtner
    return;
    int rwidth = (int)m_reserveMap.size();
    int rheight = (int)m_reserveMap[0].size();

    for (int x = 0; x < rwidth; ++x)
    {
        for (int y = 0; y < rheight; ++y)
        {
            if (m_reserveMap[x][y] || isInResourceBox(x, y))
            {
                m_bot.Map().drawTile(x, y, CCColor(255, 255, 0));
            }
        }
    }
}

void BuildingPlacer::freeTiles(int bx, int by, int width, int height)
{
    int rwidth = (int)m_reserveMap.size();
    int rheight = (int)m_reserveMap[0].size();

	int xdelta = (int)std::ceil((width - 1.0) / 2);
	int ydelta = (int)std::ceil((height - 1.0) / 2);

    for (int x = bx - xdelta; x < bx + width - xdelta && x < rwidth; x++)
    {
        for (int y = by - ydelta; y < by + height - ydelta && y < rheight; y++)
        {
            m_reserveMap[x][y] = false;
        }
    }
}

CCTilePosition BuildingPlacer::getRefineryPosition()
{
    CCPosition closestGeyser(0, 0);
    double minGeyserDistanceFromHome = std::numeric_limits<double>::max();
    CCPosition homePosition = m_bot.GetStartLocation();


    for (auto & unit : m_bot.GetAllUnits())
    {
		UnitType refinery = Util::GetRefinery(m_bot.GetPlayerRace(Players::Self), m_bot);


        if (!unit.getType().isGeyser())
        {
            continue;
        }

        CCPosition geyserPos(unit.getPosition());


		// can't build a refinery on top of another
		if (!m_bot.Map().canBuildTypeAtPosition((int)geyserPos.x, (int)geyserPos.y, refinery))
		{
			continue;
		}

        // check to see if it's next to one of our depots
        bool nearDepot = false;
        for (auto & unit : m_bot.UnitInfo().getUnits(Players::Self))
        {
            if (unit.getType().isResourceDepot() && Util::Dist(unit, geyserPos) < 10)
            {
                nearDepot = true;
            }
        }

        if (nearDepot)
        {
            double homeDistance = Util::Dist(unit, homePosition);

            if (homeDistance < minGeyserDistanceFromHome)
            {
                minGeyserDistanceFromHome = homeDistance;
                closestGeyser = unit.getPosition();
            }
        }
    }
    return CCTilePosition((int)closestGeyser.x, (int)closestGeyser.y);
}

bool BuildingPlacer::isReserved(int x, int y) const
{
    int rwidth = (int)m_reserveMap.size();
    int rheight = (int)m_reserveMap[0].size();
    if (x < 0 || y < 0 || x >= rwidth || y >= rheight)
    {
        return false;
    }

    return m_reserveMap[x][y];
}

