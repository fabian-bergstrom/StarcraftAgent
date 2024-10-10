from __future__ import annotations
from typing import TYPE_CHECKING, Any, Type, Union, Optional

if TYPE_CHECKING:
    from tasks.task import Task
    from modules.py_unit import PyUnit
    from agents.basic_agent import BasicAgent

from tasks.task import Status, Idle
from tasks.train import Train, BuildAddon, Morph, Research
from tasks.gather_minerals import GatherMinerals
from tasks.gather_gas import GatherGas
from tasks.build import Build
from tasks.scout import Scout
from tasks.attack import Attack
from library import PLAYER_SELF, PLAYER_ENEMY, UNIT_TYPEID, UnitType, UPGRADE_ID
from modules.extra import exists_producer_for, get_worker_type, has_prerequisites
from queue import SimpleQueue
from config import DEBUG_CONSOLE
from modules.create_build_order import *

RESOURCE_PRIO = 10
WORKER_PRIO = 6
UNIT_PRIO = 5
ADDON_PRIO = 4
UPGRADE_PRIO = 4
BUILDING_PRIO = 3
SCOUT_PRIO = 2
ATTACK_PRIO = 2


class TaskCollection:
    """A class that represents a collection of tasks."""

    def __init__(self):
        self.tasks: set[Task] = set()

    def __str__(self):
        return self.tasks.__str__()

    def __iter__(self):
        return self.tasks.__iter__()

    def get_tasks(self, task_type: Type[Task], target: Any, *unit_types: UnitType) -> set[Task]:
        """
        Gets all tasks matching conditions.
        :param task_type: The type of tasks to retrieve.
        :param target: The target the task should have.
        :param unit_types: The unit_types the task should have as candidates.
        :return: A set of tasks that matches the task_type, target, and unit_types.
        """
        return_set = self.tasks.intersection(
            {task for task in self.tasks if isinstance(task, task_type)}).intersection(
            {task for task in self.tasks if type(task.target) == type(target) and task.target == target})

        for unit_type in unit_types:
            return_set = return_set.intersection({task for task in self.tasks if unit_type in task.candidates})

        return return_set

    def add(self, task: Task) -> None:
        """Add a task to the collection."""
        self.tasks.add(task)

    def remove(self, task: Task) -> None:
        """Removes the task form the collection."""
        if task in self.tasks:
            self.tasks.remove(task)

    def is_empty(self) -> bool:
        """Returns whether the task collection is empty."""
        return len(self.tasks) == 0


class TaskManager:
    """A class that manages the tasks."""

    def __init__(self, agent: BasicAgent):
        self.current_tasks = TaskCollection()
        self.agent = agent
        self.task_queue = TaskCollection()
        self.have_attacked = 0

    def on_step(self, new_units: list[PyUnit]) -> None:
        """
        Handles all tasks for current step.

        New units are given idle tasks,
        current tasks are performed,
        new tasks are generated and queued, and
        queued tasks are distributed.
        """
        for py_unit in new_units:
            idle_task = Idle()
            py_unit.give_task(idle_task)
            self.current_tasks.add(idle_task)
        self.perform_tasks()
        self.generate_tasks()
        self.distribute_tasks()

    def generate_tasks(self) -> None:
        """Generates all tasks that need to be preformed by the bot."""
        self.gather()
        self.build()
        self.attack()
        if self.agent.current_frame == 1:
            self.scout()

    def gather(self) -> None:
        """
        All tasks for gathering minerals from mineral fields and gas from refineries are created here.
        The number of tasks created is equal to the number of missing workers to maximise output.
        """
        worker_types = self.agent.WORKER_TYPES
        for base in self.agent.base_location_manager.get_occupied_base_locations(PLAYER_SELF):
            townhalls = self.agent.unit_collection.get_group(
                lambda u: u.tile_position == base.depot_position and u.unit_type.is_resource_depot)
            if not townhalls or not townhalls.pop().is_completed:
                # This base is occupied by an indicator or an incomplete townhall
                break

            minerals = [mineral for mineral in base.minerals if mineral.minerals_left_in_mineralfield > 0]
            diff = len(minerals) * 2 - len(self.agent.unit_collection.get_group(base, *worker_types)) - len(
                self.task_queue.get_tasks(GatherMinerals, base, *worker_types))
            if diff > 0:
                for i in range(0, diff):
                    self.task_queue.add(GatherMinerals(base, RESOURCE_PRIO, self.agent))
            elif diff < 0:
                # TODO: Remove gather task from excessive workers
                pass
            refineries = self.agent.unit_collection.get_group(base, PLAYER_SELF, lambda
                u: u.unit_type.is_refinery and u.is_completed and u.gas_left_in_refinery > 0)
            for refinery in refineries:
                workers_in_refinery = self.agent.unit_collection.get_group(refinery)
                diff = 3 - len(workers_in_refinery) - len(
                    self.task_queue.get_tasks(GatherGas, refinery, *worker_types))
                if diff > 0:
                    for i in range(0, diff):
                        self.task_queue.add(GatherGas(refinery, RESOURCE_PRIO + 1 - i, self.agent))
                elif diff < 0:
                    # TODO: Remove gather task from excessive workers
                    pass

    def build(self) -> None:
        """
        All tasks for building, upgrading, and researching upgrade units and building are created here.
        If the top item on the build order can be built, a task corresponding to the action needed is created.
        Otherwise an action for training a worker is created.
        """
        to_build = self.agent.build_order.peek()
        if to_build is not None:
            type_to_build = UnitType(to_build, self.agent) if isinstance(to_build, UNIT_TYPEID) else to_build
            if self.can_produce(type_to_build):
                # We can build this now!
                production_task = self.get_production_task(type_to_build)
                if production_task:
                    self.task_queue.add(production_task)
                    self.agent.build_order.pop()
                    return

        Building_tree.minerals = self.agent.minerals
        Building_tree.gas = self.agent.gas
        Building_tree.supply = self.agent.max_supply - self.agent.internal_supply

        buildings = []
        if startOrder:
            to_be_built = startOrder.pop(0)
            buildings = find_build_order(to_be_built)[0]
        if AttackOrder and self.agent.current_frame > 6500:
            to_be_built = AttackOrder.pop(0)
            buildings = find_build_order(to_be_built)[0]
        if Expand and self.agent.current_frame > 10000:
            to_be_built = Expand.pop(0)
            buildings = find_build_order(to_be_built)[0]

        for building in buildings:
            for node in Building_tree.nodes:
                if node.name == building:
                    node.built += 1
            self.agent.build_order.add(building)

    def can_produce(self, type_to_build: Union[UnitType, UPGRADE_ID]) -> bool:
        """Returns whether we have the required units/upgrades to produce a type."""
        return exists_producer_for(self.agent, type_to_build) and has_prerequisites(self.agent, type_to_build)

    def get_production_task(self, type_to_build: Union[UnitType, UPGRADE_ID]) -> Optional[Task]:
        """Returns a task that can create/upgrade the unit/upgrade type."""
        if isinstance(type_to_build, UPGRADE_ID):
            return Research(type_to_build, UPGRADE_PRIO, self.agent)

        if not type_to_build.is_building:
            return Train(type_to_build, UNIT_PRIO, self.agent)

        if type_to_build.is_addon:
            return BuildAddon(type_to_build, ADDON_PRIO, self.agent)

        if type_to_build.get_equivalent_units:
            return Morph(type_to_build, UPGRADE_PRIO, self.agent)

        pos = self.agent.py_building_placer.find_position(type_to_build)
        if pos:
            return Build(type_to_build, pos, BUILDING_PRIO, self.agent)

        return None

    def attack(self) -> None:
        """Generates attack tasks targeting the enemy starting base for every combat unit."""
        pos = self.agent.base_location_manager.get_player_starting_base_location(PLAYER_ENEMY).position

        if self.task_queue.is_empty() and self.agent.build_order.is_empty() and\
                self.agent.current_frame > 10 and self.agent.current_frame - self.have_attacked > 1000 :
            self.have_attacked = self.agent.current_frame
            py_units = self.agent.unit_collection.get_group(PLAYER_SELF, lambda u: u.unit_type.is_combat_unit, Idle)
            for i in range(0, len(py_units)):
                self.task_queue.add(Attack(pos, ATTACK_PRIO, self.agent))

    def scout(self) -> None:
        """
        Finds the two closest bases to the enemy's starting location and creates a task to scout these bases and the
        staring base of the opponent.
        """
        # Scout, compiles a list over bases to scout0

        enemy_base_pos = self.agent.base_location_manager.get_player_starting_base_location(PLAYER_ENEMY).position
        # Getting a complete list of all base positions, but removing PLAYER_SELF and PLAYER_ENEMY starting bases.
        bases_pos = [base.position for base in self.agent.base_location_manager.base_locations if
                     not (base.is_player_start_location(PLAYER_SELF) or base.is_player_start_location(PLAYER_ENEMY))]
        # Calculating distance from PLAYER_ENEMY starting base location to all other potential bases.
        bases_pos.sort(key=lambda p: int(self.agent.map_tools.get_ground_distance(enemy_base_pos, p)))

        scout_bases = SimpleQueue()
        # Add the two bases closest to the enemy base, with the furthest added first
        for i in range(min(2, len(bases_pos)))[::-1]:
            scout_bases.put(bases_pos[i])
        # Add enemy base - the final base location to visit before finished.
        scout_bases.put(enemy_base_pos)
        # Creates a new task and provides a queue of coordinates to visit
        new_task = Scout(scout_bases, SCOUT_PRIO, self.agent)
        self.task_queue.add(new_task)

    def perform_tasks(self) -> None:
        """Calls every unit's on_step and handles if the task of the unit is done, not done, or failed."""
        for py_unit in self.agent.unit_collection.get_group(PLAYER_SELF):
            status: Status = py_unit.on_step()
            if status == Status.DONE:
                self.current_tasks.remove(py_unit.task)
                idle_task = Idle()
                py_unit.give_task(idle_task)
                self.current_tasks.add(idle_task)
            elif status == Status.NOT_DONE:
                pass
            elif status.is_fail():
                self.current_tasks.remove(py_unit.task)
                if py_unit.task.restart_on_fail:
                    self.task_queue.add(py_unit.task)
                if py_unit.is_alive:
                    idle_task = Idle()
                    py_unit.give_task(idle_task)
                    self.current_tasks.add(idle_task)

    def distribute_tasks(self) -> None:
        """
        Distributes the tasks in the task queue to the units. If a task can't be given to a unit it remains in the task
        queue to be distributed in the future.
        """
        tasks_to_remove = []
        checked_tasks: set[str] = set()

        for task in sorted(self.task_queue, key=lambda t: t.prio):
            if (str(task)) in checked_tasks:
                continue

            py_units = self.agent.unit_collection.get_group(PLAYER_SELF, lambda
                u: u.is_completed and u.task.prio > task.prio and u.is_alive and u.unit_type.unit_typeid in task.candidates)
            py_units = sorted(py_units, key=lambda u: (
                -u.task.prio, u.position.square_distance(u.task.target_position) if u.task.target_position else 0))

            task_started = False
            for py_unit in py_units:
                old_task = py_unit.task
                status = py_unit.give_task(task)
                if not status.is_fail():
                    if old_task.restart_on_fail:
                        # Old task overridden, re-add it to task queue
                        self.task_queue.add(old_task)
                    self.current_tasks.remove(old_task)
                    self.current_tasks.add(task)
                    tasks_to_remove.append(task)
                    if task.target:
                        self.agent.unit_collection.add_to_group(task.target, py_unit)
                    if old_task.target:
                        self.agent.unit_collection.remove_from_group(old_task.target, py_unit)
                    task_started = True
                    break
                else:
                    if DEBUG_CONSOLE:
                        print(f"Failed to start task {task} with unit {py_unit}: {status}")
                    if status == Status.FAIL_CANT_AFFORD:
                        break

            if not task_started:
                # No unit can perform ths task this tick
                checked_tasks.add(str(task))

        for task in tasks_to_remove:
            self.task_queue.remove(task)
