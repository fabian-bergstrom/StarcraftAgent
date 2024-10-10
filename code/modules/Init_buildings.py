seconds_for_worker_to_move_to_location = 4
from library import UNIT_TYPEID

class Node:
    def __init__(self, name, children, build_time, minerals, gas):
        self.name = name
        self.children = children
        self.build_time = build_time
        self.built = 0
        self.parent = None
        self.evaluated_cost = 0
        self.minerals_sec = None
        self.gas_sec = None
        self.minerals = minerals
        self.gas = gas
        self.supply_spots = 0
        self.techlab_needed = False


Worker = Node(UNIT_TYPEID.TERRAN_SCV, [], 12, 50, 0)
Worker.supply_spots = 1
Worker.built = 12

Marine = Node(UNIT_TYPEID.TERRAN_MARINE, [], 18, 50, 0)
Marine.supply_spots = 1

Reaper = Node(UNIT_TYPEID.TERRAN_REAPER, [], 32, 50, 50)
Reaper.supply_spots = 1

Marauder = Node(UNIT_TYPEID.TERRAN_MARAUDER, [], 21, 100, 25)
Marauder.supply_spots = 2
Marauder.techlab_needed = True

Ghost = Node(UNIT_TYPEID.TERRAN_GHOST, [], 29, 150, 125)
Ghost.supply_spots = 2
Ghost.techlab_needed = True

Hellion = Node(UNIT_TYPEID.TERRAN_HELLION, [], 21, 100, 0)
Hellion.supply_spots = 2

Widow_Mine = Node(UNIT_TYPEID.TERRAN_WIDOWMINE, [], 21, 75, 25)
Widow_Mine.supply_spots = 2

#Hellbat = Node(UNIT_TYPEID.TERRAN_HELLBAT, [], 21, 100, 0)
#Hellbat.supply_spots = 2

Viking = Node(UNIT_TYPEID.TERRAN_VIKINGFIGHTER, [], 30, 150, 75)
Viking.supply_spots = 2

Medivac = Node(UNIT_TYPEID.TERRAN_MEDIVAC, [], 30, 100, 100)
Medivac.supply_spots = 2

Liberator = Node(UNIT_TYPEID.TERRAN_LIBERATOR, [], 43, 150, 150)
Liberator.supply_spots = 3

Battle_Cruiser = Node(UNIT_TYPEID.TERRAN_BATTLECRUISER, [], 64, 400, 300)
Battle_Cruiser.supply_spots = 6
Battle_Cruiser.techlab_needed = True

Siege_Tank = Node(UNIT_TYPEID.TERRAN_SIEGETANK, [], 32, 150, 125)
Siege_Tank.supply_spots = 3
Siege_Tank.techlab_needed = True

Cyclone = Node(UNIT_TYPEID.TERRAN_CYCLONE, [], 32, 150, 100)
Cyclone.supply_spots = 3
Cyclone.techlab_needed = True

Thor = Node(UNIT_TYPEID.TERRAN_THOR, [], 43, 300, 200)
Thor.supply_spots = 6
Thor.techlab_needed = True

Banshee = Node(UNIT_TYPEID.TERRAN_BANSHEE, [], 43, 150, 100)
Banshee.supply_spots = 3
Banshee.techlab_needed = True

Raven = Node(UNIT_TYPEID.TERRAN_RAVEN, [], 60, 100, 200)
Raven.supply_spots = 2

Refinery = Node(UNIT_TYPEID.TERRAN_REFINERY, [], 21, 75, 0)
# TODO must build refinery to mine gas

Bunker = Node(UNIT_TYPEID.TERRAN_BUNKER, [], 29, 100, 0)

Orbital_Command = Node(UNIT_TYPEID.TERRAN_ORBITALCOMMAND, [], 25, 150, 0)

Fusion_Core = Node(UNIT_TYPEID.TERRAN_FUSIONCORE, [], 46, 150, 150)

Fusion_Core_with_techLab = Node(UNIT_TYPEID.TERRAN_FUSIONCORE, [Battle_Cruiser], 46, 150, 150)

TechLab_with_starport = Node(UNIT_TYPEID.TERRAN_STARPORTTECHLAB, [Fusion_Core_with_techLab, Banshee], 18, 50, 25)

Starport = Node(UNIT_TYPEID.TERRAN_STARPORT, [Fusion_Core, Viking, Medivac, Liberator, Raven, TechLab_with_starport],
                36, 150, 100)

Armory = Node(UNIT_TYPEID.TERRAN_ARMORY, [], 46, 150, 100)

Armory_with_techLab = Node(UNIT_TYPEID.TERRAN_ARMORY, [Thor], 46, 150, 150)

TechLab_with_Factory = Node(UNIT_TYPEID.TERRAN_FACTORYTECHLAB, [Siege_Tank, Cyclone, Armory_with_techLab], 18, 50, 25)

Factory = Node(UNIT_TYPEID.TERRAN_FACTORY, [Armory, Starport, Hellion, Widow_Mine, TechLab_with_Factory], 43, 150, 100)

Ghost_Academy = Node(UNIT_TYPEID.TERRAN_GHOSTACADEMY, [], 29, 150, 50)

Ghost_Academy_with_techLab = Node(UNIT_TYPEID.TERRAN_GHOSTACADEMY, [Ghost], 29, 150, 50)

TechLab_with_Barracks = Node(UNIT_TYPEID.TERRAN_BARRACKSTECHLAB, [Marauder, Ghost_Academy_with_techLab], 18, 50, 25)

Barracks = Node(UNIT_TYPEID.TERRAN_BARRACKS, [Ghost_Academy, Factory, Orbital_Command, Bunker, Marine, Reaper, TechLab_with_Barracks]
                , 46, 150, 0)

Supply_depot = Node(UNIT_TYPEID.TERRAN_SUPPLYDEPOT, [Barracks], 21, 100, 0)
Supply_depot.supply_spots = 8

Planetary_Fortress = Node(UNIT_TYPEID.TERRAN_PLANETARYFORTRESS, [], 36, 150, 150)

Sensor_Tower = Node(UNIT_TYPEID.TERRAN_SENSORTOWER, [],
                    18, 125, 100)

Missile_Turret = Node(UNIT_TYPEID.TERRAN_MISSILETURRET, [],
                      18, 100, 0)

Engineering_Bay = Node(UNIT_TYPEID.TERRAN_ENGINEERINGBAY, [Missile_Turret, Sensor_Tower, Planetary_Fortress]
                       , 25, 125, 0)

Command_Center = Node(UNIT_TYPEID.TERRAN_COMMANDCENTER, [Engineering_Bay], 71, 400, 0)
Command_Center.built = 1
Command_Center.supply_spots = 10

Root = Node(UNIT_TYPEID.TERRAN_KD8CHARGE, [Worker, Command_Center, Supply_depot, Refinery], 0, 0, 0)
#USING KD8CHARGE to make name checks possible, will never build root anyway

class Tree:
    def __init__(self, nodes, supply, minerals, gas):
        self.nodes = nodes
        self.supply = supply
        self.minerals = minerals
        self.gas = gas


Building_tree = Tree([Worker, Root, Command_Center, Engineering_Bay, Missile_Turret, Sensor_Tower, Planetary_Fortress,
                  Supply_depot, Barracks, Ghost_Academy, Factory, Armory, Starport,
                  Fusion_Core, Orbital_Command, Bunker, Refinery, Marine, Marauder,
                  Ghost, Reaper, Hellion, Widow_Mine, Viking, Medivac, Liberator, TechLab_with_Factory
                     , TechLab_with_Barracks, TechLab_with_starport, Armory_with_techLab,
                  Fusion_Core_with_techLab, Ghost_Academy_with_techLab, Battle_Cruiser,
                  Siege_Tank, Cyclone, Thor, Banshee, Raven], 3, 100, 0)
