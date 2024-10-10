#pragma once

#include "nlohmann/json.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "sc2api/sc2_typeenums.h"


struct BuildDescription
{
    sc2::UNIT_TYPEID producer_type;
    sc2::UNIT_TYPEID result_type;
    sc2::AbilityID build_ability;
    sc2::AbilityID morph_ability;

    std::vector<sc2::UNIT_TYPEID> buildings_needed;
    std::vector<sc2::UNIT_TYPEID> addons_needed;
    std::vector<sc2::UPGRADE_ID> upgrades_needed;
    int buildTime; // The time in seconds it takes to create the unit
};

struct ResearchDescription
{
    sc2::UPGRADE_ID result_type;
    sc2::UNIT_TYPEID producer_type;
    sc2::AbilityID ability_used;

    std::vector<sc2::UNIT_TYPEID> buildings_needed;
    std::vector<sc2::UPGRADE_ID> upgrades_needed;
};

class TechTreeImproved
{
    std::map<sc2::UNIT_TYPEID, std::vector<BuildDescription>> result_to_data;
    std::map<sc2::UPGRADE_ID, std::vector<ResearchDescription>> upgrade_to_data;

    std::vector<BuildDescription> build_descriptions;
    std::vector<ResearchDescription> research_descriptions;

    // If there is no BuildDescription for a given type, a reference to tihs list is returned.
    const std::vector<BuildDescription> empty_build {};
    const std::vector<ResearchDescription> empty_research {};

    void parse_unit(nlohmann::json::iterator it);
public:
    TechTreeImproved();
    bool LoadData();
    // Given a unit, how can we build it?
    const std::vector<BuildDescription> & HowToBuild(sc2::UnitTypeID unit) const;
    const std::vector<ResearchDescription> & HowToResearch(sc2::UpgradeID upgrade) const;

    const std::vector<BuildDescription> & BuildDescriptions() const { return build_descriptions; }
    const std::vector<ResearchDescription> & ResearchDescriptions() const { return research_descriptions; }
};
