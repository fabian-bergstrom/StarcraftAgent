#include "TechTree.h"
#include "IDABot.h"
#include "MetaType.h"

TechTree::TechTree(sc2::Client & client)
    : m_client(client), suppressWarnings(false)
{

}

void TechTree::onStart()
{
    initUnitTypeData();
    initUpgradeData();

    /* As it turns out, some of the hard-coded values are wrong. Instead of
    going through each and checking, we fix them with information from a
    (hopefully) up to date JSON-file. Sorry in advance, this was the best
    solution at the time. Hopefully the JSON-files are still up to date. */

    TechTreeImproved tree;
    bool success = tree.LoadData();

    // If the file was not successfully open/parsed, don't replace the unitTypeData.
    if (!success)
    {
        return;
    }

    std::set<sc2::UNIT_TYPEID> updated;

    for (const BuildDescription & description : tree.BuildDescriptions())
    {
        if (m_unitTypeData.count(UnitType(description.result_type, m_client)) == 0)
        {
            //std::cout << "Inserting new information about UNIT_TYPEID: " 
            //    << sc2::UnitTypeToName(description.result_type) << " (" 
            //    << static_cast<unsigned int>(description.result_type) << ")" 
            //    << std::endl;
        }

        TypeData & data = m_unitTypeData[UnitType(description.result_type, m_client)];

        // If this is the first time we a way to build this type, remove previous data
        if (updated.count(description.result_type) == 0)
        {
            data.whatBuilds.clear();
            data.requiredUnits.clear();
            data.requiredUpgrades.clear();
            updated.insert(description.result_type);
            data.buildTime = description.buildTime; // The time in seconds it takes to create the unit

            data.buildAbility = sc2::ABILITY_ID::INVALID;
        }

        data.whatBuilds.push_back(UnitType(description.producer_type, m_client));

        if (description.build_ability != sc2::ABILITY_ID::INVALID) data.buildAbility = description.build_ability;
        if (description.morph_ability != sc2::ABILITY_ID::INVALID) data.morphAbility = description.morph_ability;

        for (sc2::UNIT_TYPEID unit_typeid : description.buildings_needed)
        {
            data.requiredUnits.push_back(UnitType(unit_typeid, m_client));
        }

        for (sc2::UNIT_TYPEID unit_typeid : description.addons_needed)
        {
            data.requiredAddons.push_back(UnitType(unit_typeid, m_client));
        }
    }

    for (std::pair <const sc2::UpgradeID, TypeData> & pair : m_upgradeData)
    {
        const sc2::UpgradeID id = pair.first;
        TypeData & data = pair.second;

        data.whatBuilds.clear();
        data.requiredUnits.clear();
        data.requiredUpgrades.clear();

        for (const ResearchDescription & description : tree.HowToResearch(id))
        {
            data.buildAbility = description.ability_used;
            data.whatBuilds.push_back(UnitType(description.producer_type, m_client));

            for (sc2::UNIT_TYPEID unit_typeid : description.buildings_needed)
            {
                data.requiredUnits.push_back(UnitType(unit_typeid, m_client));
            }

            for (sc2::UPGRADE_ID upgrade_id : description.upgrades_needed)
            {
                data.requiredUpgrades.push_back(upgrade_id);
            }
        }
    }
}

void TechTree::setSuppressWarnings(bool b)
{
	suppressWarnings = b;
}

bool TechTree::getSuppressWarnings() const
{
	return suppressWarnings;
}

void TechTree::initUnitTypeData()
{
    m_unitTypeData[UnitType(0, m_client)] = TypeData();

    // Protoss Buildings                                                                                  unit  bld   wrk    rfn    sup    hall   add
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLONOVERCHARGED, m_client)] =        { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false,  true, false, false, sc2::ABILITY_ID::EFFECT_PHOTONOVERCHARGE, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE, m_client), UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_PYLON, m_client)] =                   { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false,  true, false, false, sc2::ABILITY_ID::BUILD_PYLON, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, m_client)] =                   { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false, false,  true, false, sc2::ABILITY_ID::BUILD_NEXUS, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR, m_client)] =             { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false,  true, false, false, false, sc2::ABILITY_ID::BUILD_ASSIMILATOR, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client)] =         { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_CYBERNETICSCORE, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_client), UnitType(sc2::UNIT_TYPEID::PROTOSS_WARPGATE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_DARKSHRINE, m_client)] =              { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_DARKSHRINE, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_client)] =             { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_FLEETBEACON, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_STARGATE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_client)] =                   { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_FORGE, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_client)] =                 { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_GATEWAY, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_STARGATE, m_client)] =                { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_STARGATE, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client) }, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON, m_client)] =            { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_PHOTONCANNON, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY, m_client)] =             { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_ROBOTICSBAY, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, m_client)] =        { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_ROBOTICSFACILITY, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE, m_client)] =          { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_TEMPLARARCHIVE, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_client)] =         { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_TWILIGHTCOUNCIL, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_WARPGATE, m_client)] =                { sc2::Race::Protoss, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::MORPH_WARPGATE, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_client) }, {}, { sc2::UPGRADE_ID::WARPGATERESEARCH } }; 

    // Protoss Units                                                                                      unit  bld   wrk    rfn    sup    hall   add
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_PROBE, m_client)] =                   { sc2::Race::Protoss, 0, 0, 1, 0, true, false,  true, false, false, false, false, sc2::ABILITY_ID::TRAIN_PROBE, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE, m_client)] =          { sc2::Race::Protoss, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_MOTHERSHIPCORE, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_NEXUS, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_ZEALOT, m_client)] =                  { sc2::Race::Protoss, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_ZEALOT, sc2::ABILITY_ID::TRAINWARP_ZEALOT, { UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_SENTRY, m_client)] =                  { sc2::Race::Protoss, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_SENTRY, sc2::ABILITY_ID::TRAINWARP_SENTRY, { UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_STALKER, m_client)] =                 { sc2::Race::Protoss, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_STALKER, sc2::ABILITY_ID::TRAINWARP_STALKER, { UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_HIGHTEMPLAR, m_client)] =             { sc2::Race::Protoss, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_HIGHTEMPLAR, sc2::ABILITY_ID::TRAINWARP_HIGHTEMPLAR, { UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_DARKTEMPLAR, m_client)] =             { sc2::Race::Protoss, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_DARKTEMPLAR, sc2::ABILITY_ID::TRAINWARP_DARKTEMPLAR, { UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_DARKSHRINE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_ADEPT, m_client)] =                   { sc2::Race::Protoss, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_ADEPT, sc2::ABILITY_ID::TRAINWARP_ADEPT, { UnitType(sc2::UNIT_TYPEID::PROTOSS_GATEWAY, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_COLOSSUS, m_client)] =                { sc2::Race::Protoss, 0, 0, 6, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_COLOSSUS,  0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_DISRUPTOR, m_client)] =               { sc2::Race::Protoss, 0, 0, 3, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_DISRUPTOR, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_WARPPRISM, m_client)] =               { sc2::Race::Protoss, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_WARPPRISM, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_OBSERVER, m_client)] =                { sc2::Race::Protoss, 0, 0, 1, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_OBSERVER, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, m_client) }, {}, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_IMMORTAL, m_client)] =                { sc2::Race::Protoss, 0, 0, 4, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_IMMORTAL, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, m_client) }, {}, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_CARRIER, m_client)] =                 { sc2::Race::Protoss, 0, 0, 6, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_CARRIER, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_STARGATE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_ORACLE, m_client)] =                  { sc2::Race::Protoss, 0, 0, 3, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_ORACLE, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_STARGATE, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_PHOENIX, m_client)] =                 { sc2::Race::Protoss, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_PHOENIX, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_STARGATE, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_VOIDRAY, m_client)] =                 { sc2::Race::Protoss, 0, 0, 4, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_VOIDRAY, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_STARGATE, m_client) }, {}, {} };  
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_TEMPEST, m_client)] =                 { sc2::Race::Protoss, 0, 0, 4, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_TEMPEST, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_STARGATE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_INTERCEPTOR, m_client)] =             { sc2::Race::Protoss, 0, 0, 0, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::BUILD_INTERCEPTORS, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CARRIER, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_ORACLESTASISTRAP, m_client)] =        { sc2::Race::Protoss, 0, 0, 0, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::BUILD_STASISTRAP, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_ORACLE, m_client) }, {}, {} }; 

    // Terran Buildings                                                                      m  g  s  t  unit  bld   wrk    rfn    sup    hall   add
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, m_client)] =            { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false,  true, false, sc2::ABILITY_ID::BUILD_COMMANDCENTER, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT, m_client)] =              { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false,  true, false, false, sc2::ABILITY_ID::BUILD_SUPPLYDEPOT, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} };  
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_REFINERY, m_client)] =                 { sc2::Race::Terran, 0, 0, 0, 0, true, true, false,  true, false, false, false, sc2::ABILITY_ID::BUILD_REFINERY, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client)] =                   { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_ARMORY, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORYFLYING, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, m_client)] =                 { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_BARRACKS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED, m_client) }, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_SENSORTOWER, m_client)] =              { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_SENSORTOWER, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY, m_client)] =                  { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_FACTORY, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_FUSIONCORE, m_client)] =               { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_FUSIONCORE, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORTFLYING, m_client) }, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT, m_client)] =                 { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_STARPORT, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORYFLYING, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY, m_client)] =             { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_GHOSTACADEMY, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_BUNKER, m_client)] =                   { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_BUNKER, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_client)] =           { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_ENGINEERINGBAY, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING, m_client) }, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_MISSILETURRET, m_client)] =            { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_MISSILETURRET, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND, m_client)] =           { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::MORPH_ORBITALCOMMAND, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, m_client) }, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS, m_client)] =        { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::MORPH_PLANETARYFORTRESS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_client) }, {} };

    // Terran Addons                                                                         m  g  s  t  unit  bld   wrk    rfn    sup    hall   add
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR, m_client)] =          { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, true, sc2::ABILITY_ID::BUILD_REACTOR_BARRACKS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, m_client) }, {}, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB, m_client)] =          { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, true, sc2::ABILITY_ID::BUILD_TECHLAB_BARRACKS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, m_client) }, {}, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR, m_client)] =           { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, true, sc2::ABILITY_ID::BUILD_REACTOR_FACTORY, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY, m_client) }, {}, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB, m_client)] =           { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, true, sc2::ABILITY_ID::BUILD_TECHLAB_FACTORY, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY, m_client) }, {}, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR, m_client)] =          { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, true, sc2::ABILITY_ID::BUILD_REACTOR_STARPORT, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT, m_client) }, {}, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB, m_client)] =          { sc2::Race::Terran, 0, 0, 0, 0, true, true, false, false, false, false, true, sc2::ABILITY_ID::BUILD_TECHLAB_STARPORT, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT, m_client) }, {}, {} };

    // Terran Equivalent Buildings
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED, m_client)] =       m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT, m_client)];
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING, m_client)] =           m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, m_client)];
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING, m_client)] =      m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, m_client)];
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORYFLYING, m_client)] =            m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY, m_client)];
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING, m_client)] =     m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND, m_client)];
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORTFLYING, m_client)] =           m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT, m_client)];

    // Terran Units                                                                          m  g  s  t  unit  bld    wrk    rfn    sup    hall   add
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client)] =                      { sc2::Race::Terran, 0, 0, 1, 0, true, false,  true, false, false, false, false, sc2::ABILITY_ID::TRAIN_SCV, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_GHOST, m_client)] =                    { sc2::Race::Terran, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_GHOST, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_MARAUDER, m_client)] =                 { sc2::Race::Terran, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_MARAUDER, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_TECHLAB, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_MARINE, m_client)] =                   { sc2::Race::Terran, 0, 0, 1, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_MARINE, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, m_client) }, {}, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_REAPER, m_client)] =                   { sc2::Race::Terran, 0, 0, 1, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_REAPER, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, m_client) }, {}, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_HELLION, m_client)] =                  { sc2::Race::Terran, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_HELLION, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_CYCLONE, m_client)] =                  { sc2::Race::Terran, 0, 0, 3, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_CYCLONE, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY, m_client) }, {}, {} };  
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_SIEGETANK, m_client)] =                { sc2::Race::Terran, 0, 0, 3, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_SIEGETANK, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_TECHLAB, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_THOR, m_client)] =                     { sc2::Race::Terran, 0, 0, 6, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_THOR, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY, m_client) }, {}, {} };  
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_WIDOWMINE, m_client)] =                { sc2::Race::Terran, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_WIDOWMINE, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_NUKE, m_client)] =                     { sc2::Race::Terran, 0, 0, 0, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::BUILD_NUKE, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_BANSHEE, m_client)] =                  { sc2::Race::Terran, 0, 0, 3, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_BANSHEE, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_TECHLAB, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER, m_client)] =            { sc2::Race::Terran, 0, 0, 6, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_BATTLECRUISER, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_LIBERATOR, m_client)] =                { sc2::Race::Terran, 0, 0, 3, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_LIBERATOR, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER, m_client)] =            { sc2::Race::Terran, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_RAVEN, m_client)] =                    { sc2::Race::Terran, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_RAVEN, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_TECHLAB, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB, m_client), UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_MEDIVAC, m_client)] =                  { sc2::Race::Terran, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_MEDIVAC, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT, m_client) }, {}, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_MULE, m_client)] =                     { sc2::Race::Terran, 0, 0, 0, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::EFFECT_CALLDOWNMULE, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND, m_client) }, {}, {} }; 
	m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_THORAP, m_client)] =                   { sc2::Race::Terran, 0, 0, 6, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_THOR, 0,{ UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY, m_client) },{},{} };

    // Zerg Buildings                                                                      m  g  s  t  unit  bld   wrk    rfn    sup    hall   add
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, m_client)] =                   { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false,  true, false, sc2::ABILITY_ID::BUILD_HATCHERY, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_EXTRACTOR, m_client)] =                  { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false,  true, false, false, false, sc2::ABILITY_ID::BUILD_EXTRACTOR, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL, m_client)] =               { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_SPAWNINGPOOL, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_client)] =           { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_EVOLUTIONCHAMBER, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_BANELINGNEST, m_client)] =               { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_BANELINGNEST, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_HYDRALISKDEN, m_client)] =               { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_HYDRALISKDEN, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT, m_client)] =             { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_INFESTATIONPIT, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_NYDUSCANAL, m_client)] =                 { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_NYDUSWORM, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_NYDUSNETWORK, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_NYDUSNETWORK, m_client)] =               { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_NYDUSNETWORK, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_ROACHWARREN, m_client)] =                { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_ROACHWARREN, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_SPINECRAWLER, m_client)] =               { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_SPINECRAWLER, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_client)] =                      { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_SPIRE, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_client)] =               { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::MORPH_GREATERSPIRE, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_SPORECRAWLER, m_client)] =               { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_SPORECRAWLER, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_ULTRALISKCAVERN, m_client)] =            { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false, false, false, sc2::ABILITY_ID::BUILD_ULTRALISKCAVERN, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client)] =                       { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false,  true, false, sc2::ABILITY_ID::MORPH_LAIR, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, m_client) }, {UnitType(sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL, m_client)}, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client)] =                       { sc2::Race::Zerg, 0, 0, 0, 0, true, true, false, false, false,  true, false, sc2::ABILITY_ID::MORPH_HIVE, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT, m_client) }, {} };  

    // Zerg Units                                                                          m  g  s  t  unit  bld    wrk    rfn    sup    hall   add
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_OVERLORD, m_client)] =                   { sc2::Race::Zerg, 0, 0, 0, 0, true, false, false, false,  true, false, false, sc2::ABILITY_ID::TRAIN_OVERLORD, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_LARVA, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_BANELING, m_client)] =                   { sc2::Race::Zerg, 0, 0, 0, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::MORPH_BANELING, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_LARVA, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_BANELINGNEST, m_client) }, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_CORRUPTOR, m_client)] =                  { sc2::Race::Zerg, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_CORRUPTOR, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_LARVA, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_DRONE, m_client)] =                      { sc2::Race::Zerg, 0, 0, 1, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_DRONE, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_LARVA, m_client) }, {}, {} }; 
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_HYDRALISK, m_client)] =                  { sc2::Race::Zerg, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_HYDRALISK, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_LARVA, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HYDRALISKDEN, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_LURKERDENMP, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_INFESTOR, m_client)] =                   { sc2::Race::Zerg, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_INFESTOR, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_LARVA, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_MUTALISK, m_client)] =                   { sc2::Race::Zerg, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_MUTALISK, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_LARVA, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_ROACH, m_client)] =                      { sc2::Race::Zerg, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_ROACH, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_LARVA, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_ROACHWARREN, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_SWARMHOSTMP, m_client)] =                { sc2::Race::Zerg, 0, 0, 3, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_SWARMHOST, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_LARVA, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_ULTRALISK, m_client)] =                  { sc2::Race::Zerg, 0, 0, 6, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_ULTRALISK, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_LARVA, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_ULTRALISKCAVERN, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_VIPER, m_client)] =                      { sc2::Race::Zerg, 0, 0, 3, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_VIPER, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_LARVA, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_ZERGLING, m_client)] =                   { sc2::Race::Zerg, 0, 0, 1, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_ZERGLING, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_LARVA, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_QUEEN, m_client)] =                      { sc2::Race::Zerg, 0, 0, 2, 0, true, false, false, false, false, false, false, sc2::ABILITY_ID::TRAIN_QUEEN, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL, m_client) }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_LARVA, m_client)] =                      { sc2::Race::Zerg, 0, 0, 2, 0, true, false, false, false, false, false, false, 0, 0, { UnitType() }, { UnitType() }, {} };
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_EGG, m_client)] =                        { sc2::Race::Zerg, 0, 0, 2, 0, true, false, false, false, false, false, false, 0, 0, { UnitType() }, { UnitType() }, {} };

    // Set the Mineral / Gas cost of each unit
    for (auto & kv : m_unitTypeData)
    {
        if (!kv.first.isValid()) { continue; }
        
        auto & data = m_client.Observation()->GetUnitTypeData()[kv.first.getAPIUnitType()];
                
        kv.second.mineralCost = data.mineral_cost;
        kv.second.gasCost     = data.vespene_cost;
    }

    // fix the cumulative prices of morphed buildings
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client)].mineralCost -= getData(UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client)).mineralCost;
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client)].mineralCost -= getData(UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, m_client)).mineralCost;
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS, m_client)].mineralCost -= getData(UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, m_client)).mineralCost;
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND, m_client)].mineralCost -= getData(UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, m_client)).mineralCost;
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_client)].mineralCost -= getData(UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_client)).mineralCost;
}

void TechTree::initUpgradeData()
{
    // 0 data for null / error return
    m_upgradeData[0] = TypeData();

    // Terran Upgrades
    m_upgradeData[sc2::UPGRADE_ID::BANSHEECLOAK] =                      { sc2::Race::Terran, 100, 100, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_BANSHEECLOAKINGFIELD, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::BANSHEESPEED] =                      { sc2::Race::Terran, 200, 200, 0, 2720, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_BANSHEEHYPERFLIGHTROTORS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::BATTLECRUISERENABLESPECIALIZATIONS]= { sc2::Race::Terran, 150, 150, 0,  960, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_BATTLECRUISERWEAPONREFIT, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::DRILLCLAWS] =                        { sc2::Race::Terran, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_DRILLINGCLAWS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::HIGHCAPACITYBARRELS] =               { sc2::Race::Terran, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_INFERNALPREIGNITER, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::HISECAUTOTRACKING] =                 { sc2::Race::Terran, 100, 100, 0, 1280, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_HISECAUTOTRACKING, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::LIBERATORAGRANGEUPGRADE] =           { sc2::Race::Terran, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ADVANCEDBALLISTICS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::MAGFIELDLAUNCHERS] =                 { sc2::Race::Terran, 100, 100, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_CYCLONELOCKONDAMAGE, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::MEDIVACINCREASESPEEDBOOST] =         { sc2::Race::Terran, 100, 100, 0, 1280, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_HIGHCAPACITYFUELTANKS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::NEOSTEELFRAME] =                     { sc2::Race::Terran, 100, 100, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_NEOSTEELFRAME, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PERSONALCLOAKING] =                  { sc2::Race::Terran, 150, 150, 0, 1920, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PERSONALCLOAKING, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PUNISHERGRENADES] =                  { sc2::Race::Terran,  50,  50, 0,  960, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_CONCUSSIVESHELLS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::RAVENCORVIDREACTOR] =                { sc2::Race::Terran, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_RAVENCORVIDREACTOR, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::RAVENRECALIBRATEDEXPLOSIVES] =       { sc2::Race::Terran, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_RAVENRECALIBRATEDEXPLOSIVES, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::SHIELDWALL] =						{ sc2::Race::Terran, 100, 100, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_COMBATSHIELD, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::STIMPACK] =                          { sc2::Race::Terran, 100, 100, 0, 2720, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_STIMPACK, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANBUILDINGARMOR] =               { sc2::Race::Terran, 150, 150, 0, 2240, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANSTRUCTUREARMORUPGRADE, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL1] =        { sc2::Race::Terran, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL2] =        { sc2::Race::Terran, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client) }, {sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL3] =        { sc2::Race::Terran, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client) }, {sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1] =       { sc2::Race::Terran, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2] =       { sc2::Race::Terran, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client) }, {sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL3] =       { sc2::Race::Terran, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_client) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client) }, {sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1] =           { sc2::Race::Terran, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL2] =           { sc2::Race::Terran, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client) }, {}, {sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL3] =           { sc2::Race::Terran, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client) }, {}, {sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL1] =  { sc2::Race::Terran, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL2] =  { sc2::Race::Terran, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client) }, {}, {sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL3] =  { sc2::Race::Terran, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client) }, {}, {sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1] =        { sc2::Race::Terran, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2] =        { sc2::Race::Terran, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client) }, {}, {sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL3] =        { sc2::Race::Terran, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_client) }, {}, {sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2} };

    // Protoss Upgrades
    m_upgradeData[sc2::UPGRADE_ID::ADEPTPIERCINGATTACK] =               { sc2::Race::Protoss, 100, 100, 0, 2240, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::BLINKTECH] =                         { sc2::Race::Protoss, 150, 150, 0, 2720, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_BLINK, 0,                   { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::CARRIERLAUNCHSPEEDUPGRADE] =         { sc2::Race::Protoss, 150, 150, 0, 1280, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_INTERCEPTORGRAVITONCATAPULT, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::CHARGE] =                            { sc2::Race::Protoss, 100, 100, 0, 2240, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_CHARGE, 0,                  { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::DARKTEMPLARBLINKUPGRADE] =           { sc2::Race::Protoss, 100, 100, 0, 2720, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_SHADOWSTRIKE, 0,            { UnitType(sc2::UNIT_TYPEID::PROTOSS_DARKSHRINE, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::EXTENDEDTHERMALLANCE] =              { sc2::Race::Protoss, 200, 200, 0, 2240, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_EXTENDEDTHERMALLANCE, 0,    { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::GRAVITICDRIVE] =                     { sc2::Race::Protoss, 100, 100, 0, 1280, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_GRAVITICDRIVE, 0,           { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::OBSERVERGRAVITICBOOSTER] =           { sc2::Race::Protoss, 100, 100, 0, 1280, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_GRAVITICBOOSTER, 0,         { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PHOENIXRANGEUPGRADE] =               { sc2::Race::Protoss, 150, 150, 0, 1440, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PHOENIXANIONPULSECRYSTALS, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL1] =            { sc2::Race::Protoss, 150, 150, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL1, 0,   { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL2] =            { sc2::Race::Protoss, 225, 225, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL2, 0,   { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_client) }, {sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL3] =            { sc2::Race::Protoss, 300, 300, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL3, 0,   { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_client) }, {sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1] =           { sc2::Race::Protoss, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2] =           { sc2::Race::Protoss, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_client) }, {sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3] =           { sc2::Race::Protoss, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_client) }, {sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1] =         { sc2::Race::Protoss, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2] =         { sc2::Race::Protoss, 150, 150, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_client) }, { sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1 } };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3] =         { sc2::Race::Protoss, 200, 200, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_client) }, { sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2 } };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1] =        { sc2::Race::Protoss, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2] =        { sc2::Race::Protoss, 150, 150, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_client) }, { sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1 } };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3] =        { sc2::Race::Protoss, 200, 200, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_client) }, { sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2 } };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL1] =              { sc2::Race::Protoss, 150, 150, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL2] =              { sc2::Race::Protoss, 225, 225, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_client) }, { sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL1 } };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL3] =              { sc2::Race::Protoss, 300, 300, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_client) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_client) }, { sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::PSISTORMTECH] =                      { sc2::Race::Protoss, 200, 200, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PSISTORM, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::WARPGATERESEARCH] =                  { sc2::Race::Protoss,  50,  50, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_WARPGATE, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_client) }, {}, {} };

    // Zerg Upgrades
    m_upgradeData[sc2::UPGRADE_ID::BURROW] =                            { sc2::Race::Zerg, 100, 100, 0, 1600, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_BURROW, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::CENTRIFICALHOOKS] =                  { sc2::Race::Zerg, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_CENTRIFUGALHOOKS, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_BANELINGNEST, m_client) }, {}, {} }; 
    m_upgradeData[sc2::UPGRADE_ID::CHITINOUSPLATING] =                  { sc2::Race::Zerg, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_CHITINOUSPLATING, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_ULTRALISKCAVERN, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::EVOLVEGROOVEDSPINES] =               { sc2::Race::Zerg, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_GROOVEDSPINES, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_HYDRALISKDEN, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::EVOLVEMUSCULARAUGMENTS] =            { sc2::Race::Zerg, 150, 150, 0, 1600, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_MUSCULARAUGMENTS, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_HYDRALISKDEN, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::GLIALRECONSTITUTION] =               { sc2::Race::Zerg, 100, 100, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_GLIALREGENERATION, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_ROACHWARREN, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {} };
    m_upgradeData[sc2::UPGRADE_ID::INFESTORENERGYUPGRADE] =             { sc2::Race::Zerg, 150, 150, 0, 1280, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PATHOGENGLANDS, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::NEURALPARASITE] =                    { sc2::Race::Zerg, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_NEURALPARASITE, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::OVERLORDSPEED] =                     { sc2::Race::Zerg, 100, 100, 0,  960, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PNEUMATIZEDCARAPACE, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {}, {} };                       
    m_upgradeData[sc2::UPGRADE_ID::TUNNELINGCLAWS] =                    { sc2::Race::Zerg, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TUNNELINGCLAWS, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_ROACHWARREN, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL1] =             { sc2::Race::Zerg, 150, 150, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGFLYERARMORLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL2] =             { sc2::Race::Zerg, 225, 225, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGFLYERARMORLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, { sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL1 } };
    m_upgradeData[sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL3] =             { sc2::Race::Zerg, 300, 300, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGFLYERARMORLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, { sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL2 } };
    m_upgradeData[sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL1] =            { sc2::Race::Zerg, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGFLYERATTACKLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL2] =            { sc2::Race::Zerg, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGFLYERATTACKLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, { sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL1 } };
    m_upgradeData[sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL3] =            { sc2::Race::Zerg, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGFLYERATTACKLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_client) }, {UnitType( sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, { sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL2 } };
    m_upgradeData[sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL1] =            { sc2::Race::Zerg, 150, 150, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGGROUNDARMORLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL2] =            { sc2::Race::Zerg, 225, 225, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGGROUNDARMORLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL3] =            { sc2::Race::Zerg, 300, 300, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGGROUNDARMORLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGLINGATTACKSPEED] =               { sc2::Race::Zerg, 200, 200, 0, 2080, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGLINGADRENALGLANDS, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGLINGMOVEMENTSPEED] =             { sc2::Race::Zerg, 100, 100, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGLINGMETABOLICBOOST, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL1] =            { sc2::Race::Zerg, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGMELEEWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL2] =            { sc2::Race::Zerg, 150, 150, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGMELEEWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL3] =            { sc2::Race::Zerg, 200, 200, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGMELEEWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL1] =          { sc2::Race::Zerg, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_client) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL2] =          { sc2::Race::Zerg, 150, 150, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_client), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL3] =          { sc2::Race::Zerg, 200, 200, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_client) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_client) }, {sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL2} };
}

const TypeData & TechTree::getData(const UnitType & type) const
{
    if (m_unitTypeData.find(type) == m_unitTypeData.end())
    {
		if (!suppressWarnings)
		{
			std::cout << "WARNING: Unit type not found: " << type.getName() << "\n";
		}
        return m_unitTypeData.begin()->second;
    }

    return m_unitTypeData.at(type);
}

const TypeData & TechTree::getData(const CCUpgrade & type)  const
{
    if (m_upgradeData.find(type) == m_upgradeData.end())
    {
		if (!suppressWarnings) 
		{
			std::cout << "WARNING: Upgrade not found: " << sc2::UpgradeIDToName(type) << "\n";
		}
        return m_unitTypeData.begin()->second;
    }

    return m_upgradeData.at(type);
}

const TypeData & TechTree::getData(const MetaType & type) const
{
    if (type.getMetaType() == MetaTypes::Unit)
    {
        return getData(type.getUnitType());
    }
    else if (type.getMetaType() == MetaTypes::Upgrade)
    {
        return getData(type.getUpgrade());
    }
    
    BOT_ASSERT(false, "Can't getData this type: %s", type.getName().c_str());

    return m_unitTypeData.begin()->second;
}
