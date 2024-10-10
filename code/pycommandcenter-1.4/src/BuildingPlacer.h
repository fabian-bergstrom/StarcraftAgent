#pragma once

#include "Common.h"

class IDABot;
class BaseLocation;
class UnitType;
class Unit;

class BuildingPlacer
{
    IDABot & m_bot;

    std::vector< std::vector<bool> > m_reserveMap;

    // queries for various BuildingPlacer data
    bool buildable(const UnitType & type, int x, int y) const;
    bool isReserved(int x, int y) const;
    bool isInResourceBox(int x, int y) const;
    bool tileOverlapsBaseLocation(int x, int y, UnitType type) const;

public:

    BuildingPlacer(IDABot & bot);

    void onStart();
	void updateReserved(const std::vector<Unit> & units);
	void freeAllTiles();

    // determines whether we can build at a given location
    bool canBuildHere(int bx, int by, const UnitType & type) const;
	bool canBuildHereWithSize(int bx, int by, int width, int height);
    bool canBuildHereWithSpace(int bx, int by, const UnitType & type, int buildDist) const;

    // returns a build location near a building's desired location
    CCTilePosition getBuildLocationNear(const CCTilePosition & p, const UnitType & type, int buildDist, size_t search_count = 1000) const;

    void drawReservedTiles();

    void reserveTiles(int x, int y, int width, int height);
    void freeTiles(int x, int y, int width, int height);
    CCTilePosition getRefineryPosition();
};
