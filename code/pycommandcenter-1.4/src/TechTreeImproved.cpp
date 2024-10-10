#include "TechTreeImproved.h"

using json = nlohmann::json;

TechTreeImproved::TechTreeImproved() { }

sc2::UNIT_TYPEID string_to_id(const std::string & str)
{
    return static_cast<sc2::UNIT_TYPEID>(std::stoi(str));
}

std::string id_to_string(int id)
{
    return sc2::UnitTypeToName(static_cast<sc2::UNIT_TYPEID>(id));
}

void add_requirement(ResearchDescription & description, json & requirement)
{
    std::string type = requirement["type"];

    if (type == "and")
    {
        for (auto & subrequirement : requirement["operands"])
        {
            add_requirement(description, subrequirement);
        }
    }
    else if (type == "not")
    {
        // Ignore this. This is used for: "We cannot do upgrade X if upgrade X is already running somewhere."
    }
    else if (type == "unitCount")
    {
        if (requirement["state"] == "CompleteOnly")
        {
            for (auto & unit : requirement["unit"])
            {
                //std::cout << "Just building: " << sc2::UnitTypeToName(static_cast<sc2::UNIT_TYPEID>(unit)) << " (" << unit << ")" << std::endl;
                description.buildings_needed.push_back(static_cast<sc2::UNIT_TYPEID>(unit));
            }
        }
        else
        {
            std::cout << "Unexpected state: " << requirement["state"] << std::endl;
        }
    }
    else if (type == "eq")
    {
        // TODO: Should we be more careful here?
        sc2::UPGRADE_ID id = static_cast<sc2::UPGRADE_ID>(requirement["operands"][0]["upgrade"]);
        int count = static_cast<int>(requirement["operands"][1]["value"]);

        if (count == 1)
        {
            description.upgrades_needed.push_back(id);
        }
    }
}

void add_requirement(BuildDescription & description, json & requirement)
{
    std::string type = requirement["type"];
    if (type == "and")
    {
        for (auto & subrequirement : requirement["operands"])
        {
            add_requirement(description, subrequirement);
        }
    }
    else if (type == "unitCount")
    {
        // Units that needs to be completed at the unit location -> Addons
        if (requirement["state"] == "CompleteOnlyAtUnit")
        {
            for (auto & unit : requirement["unit"])
            {
                //std::cout << "Addon: " << sc2::UnitTypeToName(static_cast<sc2::UNIT_TYPEID>(unit)) << " (" << unit << ")" << std::endl;
                sc2::UNIT_TYPEID unit_typeid = static_cast<sc2::UNIT_TYPEID>(unit);
                if (unit_typeid == sc2::UNIT_TYPEID::TERRAN_TECHLAB)
                {
                    switch (description.producer_type)
                    {
                    case sc2::UNIT_TYPEID::TERRAN_BARRACKS:
                        unit_typeid = sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB;
                        break;
                    case sc2::UNIT_TYPEID::TERRAN_FACTORY:
                        unit_typeid = sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB;
                        break;
                    case sc2::UNIT_TYPEID::TERRAN_STARPORT:
                        unit_typeid = sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB;
                        break;
                    default:
                        std::wcout << "Unknown producer type for TECHLAB addon: " << sc2::UnitTypeToName(unit_typeid) << std::endl;
                    }
                }
                description.addons_needed.push_back(unit_typeid);
            }
        }
        else if (requirement["state"] == "CompleteOnly")
        {
            for (auto & unit : requirement["unit"])
            {
                //std::cout << "Just building: " << sc2::UnitTypeToName(static_cast<sc2::UNIT_TYPEID>(unit)) << " (" << unit << ")" << std::endl;
                description.buildings_needed.push_back(static_cast<sc2::UNIT_TYPEID>(unit));
            }
        }
        else
        {
            //std::cout << "Unsupported" << std::endl;
            //std::cout << requirement << std::endl;
        }
    }
}

void parse_build_description(BuildDescription & description, json & build_item)
{
    description.result_type = static_cast<sc2::UNIT_TYPEID>(build_item["unit"]);
    description.build_ability = static_cast<sc2::ABILITY_ID>(build_item["ability"]);
    description.buildTime = static_cast<int>(build_item["time"]); // The time in seconds it takes to create the unit parsed from the json file

    if (build_item.find("requires") != build_item.end())
    {
        //std::cout << "Building unit: " << id_to_string(build_item["unit"]) << " requires" << std::endl;
        auto & requires = build_item["requires"][0];
        add_requirement(description, requires);
    }
}

void parse_research_item(ResearchDescription & description, json & research_item)
{
    //std::cout << "Parsing " << research_item["upgradeName"] << std::endl;

    description.result_type = static_cast<sc2::UPGRADE_ID>(research_item["upgrade"]);
    description.ability_used = static_cast<sc2::ABILITY_ID>(research_item["ability"]);

    if (research_item.find("requires") != research_item.end() && research_item["requires"].size() > 0)
    {
        auto & requires = research_item["requires"][0];
        add_requirement(description, requires);
    }
}

void TechTreeImproved::parse_unit(json::iterator it)
{
    sc2::UNIT_TYPEID producer_id = string_to_id(it.key());
    //std::string name = sc2::UnitTypeToName(producer_id);

    if (it.value().find("builds") != it.value().end())
    {
        auto & builds = it.value()["builds"];

        for (auto & build_item : builds)
        {
            BuildDescription description;
            description.producer_type = producer_id;
            parse_build_description(description, build_item);

            if (result_to_data.count(description.result_type) > 0)
            {
                //std::cout << "Found more than one way to build " << sc2::UnitTypeToName(description.result_type) << " (" << (unsigned int) description.result_type << ")" << std::endl;
                result_to_data[description.result_type].push_back(description);
            }
            else
            {
                result_to_data[description.result_type] = { description };
            }
            build_descriptions.push_back(description);
        }
    }

    // TODO: Use the result from the call to find for actually looking up data later, instead of searching twice
    if (it.value().find("researches") != it.value().end())
    {
        //std::cout << "Found upgrades on unit " << it.value()["name"] << std::endl;

        for (auto & research_item : it.value()["researches"])
        {
            ResearchDescription description;
            description.producer_type = producer_id;

            parse_research_item(description, research_item);

            if (upgrade_to_data.count(description.result_type))
            {
                upgrade_to_data[description.result_type].push_back(description);
            }
            else
            {
                upgrade_to_data[description.result_type] = { description };
            }
            research_descriptions.push_back(description);
        }
    }

    if (it.value().find("morphs") != it.value().end())
    {
        for (auto & morph_item : it.value()["morphs"])
        {
            BuildDescription description;
            description.producer_type = producer_id;
            description.morph_ability = static_cast<sc2::ABILITY_ID>(morph_item["ability"]);
            description.result_type = static_cast<sc2::UNIT_TYPEID>(morph_item["unit"]);

            if (morph_item.find("requires") != morph_item.end())
            {
                for (auto & requirement : morph_item["requires"])
                {
                    std::string type = requirement["type"];
                    if (type == "unitCount")
                    {
                        for (auto & unit : requirement["unit"])
                        {
                            sc2::UNIT_TYPEID unit_typeid = static_cast<sc2::UNIT_TYPEID>(unit);
                            description.buildings_needed.push_back(unit_typeid);
                        }
                    }
                    else if (type == "upgradeCount")
                    {
                        sc2::UPGRADE_ID upgrade_id = static_cast<sc2::UPGRADE_ID>(requirement["upgrade"]);
                        description.upgrades_needed.push_back(upgrade_id);
                    }
                }

            }

            build_descriptions.push_back(description);
        }
    }
}

bool TechTreeImproved::LoadData() {
    std::ifstream i("techtree.json");

    if (!i.good())
    {
        std::wcerr << "File techtree.json cannot be found, information regarding addons and required buildings will not be up to date. Please put techtree.json in working directory." << std::endl;
        return false;
    }

    // Parse the file's content
    json j;
    i >> j;

    // Time to parse content of the JSON file
    for (auto & race : j)
    {
        for (json::iterator it = race.begin(); it != race.end(); ++it)
        {
            parse_unit(it);
        }
    }
    return true;
}

const std::vector<BuildDescription> & TechTreeImproved::HowToBuild(sc2::UnitTypeID unit) const
{
    if (result_to_data.count(unit) > 0)
    {
        return result_to_data.at(unit);
    }
    else
    {
        std::cout << "No information about unit type " << sc2::UnitTypeToName(unit) << " (" << static_cast<int>(unit) << ")" << std::endl;
        return empty_build;
    }
}

const std::vector<ResearchDescription> & TechTreeImproved::HowToResearch(sc2::UpgradeID upgrade) const
{
    if (upgrade_to_data.count(upgrade))
    {
        return upgrade_to_data.at(upgrade);
    }
    else
    {
        std::cout << "No information about upgrade type " << sc2::UpgradeIDToName(upgrade) << " (" << static_cast<int>(upgrade) << ")" << std::endl;
        return empty_research;
    }
}
