#pragma once

#include "BaseLocation.h"

class IDABot;

class BaseLocationManager
{
    IDABot & m_bot;

    std::vector<BaseLocation>                       m_baseLocationData;
    std::vector<const BaseLocation *>               m_baseLocationPtrs;
    std::vector<const BaseLocation *>               m_startingBaseLocations;
    std::map<int, const BaseLocation *>             m_playerStartingBaseLocations;
    std::map<int, std::set<const BaseLocation *>>   m_occupiedBaseLocations;
    std::vector<std::vector<BaseLocation *>>        m_tileBaseLocations;

    BaseLocation * getBaseLocation(const CCPosition & pos) const;

public:

    BaseLocationManager(IDABot & bot);
    
    void onStart();
    void onFrame();
    void drawBaseLocations();

    const std::vector<const BaseLocation *> & getBaseLocations() const;
    const std::vector<const BaseLocation *> & getStartingBaseLocations() const;
    const std::set<const BaseLocation *> & getOccupiedBaseLocations(int player) const;
    const BaseLocation * getPlayerStartingBaseLocation(int player) const;
    
    const BaseLocation * getNextExpansion(int player) const;

private:
	void updateMinerals(BaseLocation & baseToUpdate);
	void updateGeysers(BaseLocation & baseToUpdate);


};
