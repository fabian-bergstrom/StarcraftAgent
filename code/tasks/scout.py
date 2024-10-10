from __future__ import annotations

import math
from typing import TYPE_CHECKING, Optional

import modules.py_unit
from Safe_Paths.PotentialField import Mapping

if TYPE_CHECKING:
    from modules.py_unit import PyUnit
    from agents.basic_agent import BasicAgent

from library import UnitType
from tasks.task import Task, Status
from queue import SimpleQueue
import numpy as np

from Safe_Paths.AStar import *
import visualdebugger.path_debugger

FIND_SAFE_PATHS = True
PATH_DEBUGGER = False  # Note: PATH_DEBUGGER and HEAT_DEBUGGER shall not be active simultaneously
HEAT_DEBUGGER = False

Point2D.distance = lambda self, other: math.sqrt((self.x - other.x) ** 2 + (self.y - other.y) ** 2)


class Scout(Task):
    """Task for scouting a list of bases."""

    def __init__(self, scout_bases: SimpleQueue[Point2D], prio: int, agent: BasicAgent):
        super().__init__(prio=prio, candidates=agent.WORKER_TYPES)
        self.unit_type: Optional[UnitType] = None
        self.scout_bases: SimpleQueue[Point2D] = scout_bases
        self.scout_target: Optional[Point2D] = None
        self.fails: int = 0
        self.previous_pos: Optional[Point2D] = None
        # "Thus, what is of supreme importance in war is to attack the enemy's strategy", Sun Tzu, The Art of War.

        # Add more data if FIND_SAFE_PATHS implementation shall be considered.
        if FIND_SAFE_PATHS:
            self.potential_field = PotentialField(agent.map_tools)
            self.path = []  # The considered A* path
            self.path_point = Point2D()  # Particular point following A*
            self.start_time = time.time()  # Debug timer
            self.point_timer = time.time()  # Timer for spending time searching for path_point
            self.POINT_RADIUS = 4  # Radius the agent check if close to next point, minimize agent stopping.

            # The maximum amount of seconds the unit is allowed to search for the next point. Used to evade
            # enemies specifically scouting enemy main base, but also solves the bug where the scout might be stuck
            self.POINT_SEARCH_TIME = 10

        # Setup debug tools if one of them allowed. Both shall not be allowed simultaneous.
        if PATH_DEBUGGER or HEAT_DEBUGGER and not (PATH_DEBUGGER and HEAT_DEBUGGER):
            self.debugger = None
            self.set_up_debugging()

    def set_up_debugging(self):
        """
        Setup debugging tools for heatmap or path visualizer.
        """
        if PATH_DEBUGGER:
            self.debugger = visualdebugger.path_debugger.PathDebugger()
        elif HEAT_DEBUGGER:
            self.debugger = visualdebugger.heat_map_debugger.HeatMapDebugger()
        self.debugger.set_color_map({(20, 20): (255, 255, 0)})
        self.debugger.set_update_frequency(0.5)

    def on_start(self, py_unit: PyUnit) -> Status:
        """
        Start or restart the task.

        :return: Status.DONE if there is a list of targets and the task has been given to at suitable unit.
        Status.FAIL if unit not suitable.
        """

        # If debugging tools considered, start it here.
        if PATH_DEBUGGER or HEAT_DEBUGGER:
            self.debugger.on_start()

        # Preparing for different strategies depending on UNIT_TYPEID that's scouting.
        self.unit_type = py_unit.unit_type.unit_typeid

        #  If it has a target the task is restarted
        if self.target:
            self.move(py_unit, self.target)
            return Status.DONE

        # Sets first target, then removes it from list
        self.target = self.scout_bases.get()

        # From start only support for worker scouting.
        if self.unit_type in self.candidates:
            self.move(py_unit, self.target)
        # Features for other units could be added.
        else:
            return Status.FAIL
        return Status.DONE

    def on_step(self, py_unit: PyUnit) -> Status:
        """
        Checks if the task is continuing.

        :return: Status.DONE if unit is finished scouting. Status.NOT_DONE if it keeps scouting.
        Status.FAIL if unit is idle.
        """

        if not self.target:
            return Status.DONE

        if py_unit.is_idle:
            return Status.FAIL

        if not py_unit.is_alive:
            # If a scout dies, and we return FAIL yet another one is automatically sent to (probably) the same destiny.
            # So even if the scouting fails, we claim DONE. Another scouting mission has to be ordered.
            return Status.DONE

        # Are we near the coordinate yet? Just close enough. < distance 5 arbitrarily chosen.
        if py_unit.position.square_distance(self.target) < 5 ** 2:
            print("Time to reach goal: ", time.time() - self.start_time)
            return self.switch_target(py_unit)

        # Are we stuck?
        if py_unit.position == self.previous_pos:
            self.fails += 1
            if self.fails == 5:
                # Scout is really stuck. Trying to switch focus to next point of interest to scout.
                return self.switch_target(py_unit)
            return Status.NOT_DONE

        # Do stuff if our feature flag set to true.
        if FIND_SAFE_PATHS:

            nearby_enemies = self.get_nearby_enemies(py_unit)
            # If we don't have a path, then try to set a new A* path.
            if not self.path:
                self.set_new_path(py_unit)

            # If spending too much time searching for the next point then...
            elif time.time() - self.point_timer > self.POINT_SEARCH_TIME:
                # If stuck in enemy cluster, select new base.
                if nearby_enemies:
                    self.switch_target(py_unit)
                # Otherwise we could not find the next position in time, select next point.
                else:
                    self.path_point = self.path.get()
                    self.point_timer = time.time()

            # If there is nearby dangerous enemies then move to new point.
            elif nearby_enemies:
                new_point = self.potential_field.evasion_point(py_unit.position, self.path_point.position, nearby_enemies)
                # Only evade if the new point is available, otherwise continue with the A* path.
                self.move(py_unit, new_point)

            # Continue moving, if close within next point then select the next one.
            else:
                self.move(py_unit, self.path_point.position)
                if self.path_point.is_within_radius(py_unit.position, self.POINT_RADIUS) and self.path:
                    self.path_point = self.path.get()
                    self.point_timer = time.time()

        # If feature flag not selected, then do simple move to the target.
        else:
            self.move(py_unit, self.target)

        # If debugger available, call their visualization methods respectively.
        if PATH_DEBUGGER:
            self.debugger.on_step(self.visualize_path())
        elif HEAT_DEBUGGER:
            self.debugger.on_step(self.visualize_heat())

        # We're still on the move.
        self.fails = 0
        self.previous_pos = py_unit.position
        return Status.NOT_DONE

    def switch_target(self, py_unit: PyUnit) -> Status:
        """Switch the task target to the next base in the queue"""
        if self.scout_bases.empty():
            # No more coordinates in list to scout.
            return Status.DONE
        else:
            # Switching target to next coordinates in list to scout.
            self.target = self.scout_bases.get()
            self.move(py_unit, self.target)

            # By switching base target, also search for new path.
            if FIND_SAFE_PATHS:
                self.set_new_path(py_unit)

            return Status.NOT_DONE

    def set_new_path(self, py_unit: PyUnit) -> None:
        """
        Try to set new path using A* and reset the debug timer. If possible set the first point.
        Does not set path if astar_search method returns empty queue meaning too close to goal or
        perhaps some other problem.

        :param py_unit: The PyUnit given by this class.
        """
        assert isinstance(py_unit, modules.py_unit.PyUnit), "Invalid type"
        self.path = astar_search(py_unit.position, self.target, self.potential_field)
        self.start_time = time.time()
        if self.path:
            self.path_point = self.path.get()
            self.point_timer = time.time()

    def move(self, py_unit: PyUnit, position: Point2D) -> None:
        """
        The default unit.move method but also check if position is valid to consider moving there.
        Only check if feature flag is active, otherwise do the default unit.move method.

        :param py_unit: The unit that shall move
        :param position: The Point2D position to move to
        """
        assert isinstance(position, Point2D) and isinstance(py_unit, modules.py_unit.PyUnit), "Invalid type(s)"
        if FIND_SAFE_PATHS:
            if self.potential_field.is_valid_point(position):
                py_unit.move(position)
        else:
            py_unit.move(position)

    def get_nearby_enemies(self, py_unit: PyUnit) -> list:
        """
        Iterates through all attacking units in the game. If our distance is within the enemy attacking range, then
        set very high potential on that position and surrounding squares. If either my unit or the enemy unit is within
        our respective sight range then mark the enemy position and surrounding squares as dangerous potential.
        Finally, retrieve all nearby enemies following either two constraints.

        :param py_unit: The unit
        :return A list containing all nearby dangerous enemies. Empty list means no nearby dangerous enemies.
        """
        assert isinstance(py_unit, modules.py_unit.PyUnit)
        nearby_enemies = []
        dangerous_enemy_potential = 150  # Potential for enemies capable for attacking unit.
        enemy_potential = 50  # Potential for enemies
        potential_radius = 1  # Radius within enemy potential setting

        for enemy in self.get_attacking_units(py_unit):
            distance_between = py_unit.position.distance(enemy.position)

            # The enemy can attack this unit, mark position as very dangerous
            if distance_between <= enemy.unit_type.attack_range:
                nearby_enemies.append(enemy.position)
                self.potential_field.set_area_potential(enemy.position, potential_radius,
                                                        dangerous_enemy_potential, allow_time=True)

            # The enemy can spot this unit or this unit can spot the enemy, mark position as dangerous.
            elif enemy.unit_type.sight_range <= distance_between or\
                    py_unit.unit.unit_type.sight_range <= distance_between:
                nearby_enemies.append(enemy.position)
                self.potential_field.set_area_potential(enemy.position, potential_radius,
                                                        enemy_potential, allow_time=True)
        return nearby_enemies

    def get_attacking_units(self, py_unit: PyUnit) -> list:
        """
        Return a list of enemies capable of attacking this unit.

        :param py_unit: The unit
        :return List of attacking enemy units, empty list means there is no attacking units left in the game.
        """
        attackers = []
        all_units = py_unit.agent.get_all_units()
        my_units = py_unit.agent.get_my_units()
        other_units = list(filter(lambda x: x not in my_units, all_units))  # Units that are not my units.

        # Get enemy units capable of attacking this unit
        for other in other_units:
            if other.unit_type.attack_damage > 0:
                attackers.append(other)
        return attackers

    def visualize_path(self) -> None:
        """
        Used for debugging which uses path_visualizer to map points from our set path.
        """
        debug_dict = {}
        for node in self.path.queue:
            debug_dict[(int(node.position.y), int(node.position.x))] = 20

        if debug_dict:
            self.debugger.set_display_values(debug_dict)

    def visualize_heat(self):
        """
        Used for debugging which uses heat_visualizer to map potential points on from our map.
        Potential points such as obstacles and enemies.
        """
        # Visualize potential
        heat = np.zeros((self.potential_field.height, self.potential_field.width))
        field = self.potential_field.potential_field
        for x in range(0, self.potential_field.width):
            for y in range(0, self.potential_field.height):
                if field[x][y][Mapping.POTENTIAL] > 10:
                    heat[y][x] = 20

        self.debugger.set_display_values(heat)

