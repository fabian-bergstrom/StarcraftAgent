import math
from library import Point2D, MapTools
import numpy as np
import time

from enum import IntEnum


class Mapping(IntEnum):
    """
    Enum mapping Potential, validity and time. Used in PotentialField class
    """
    POTENTIAL = 0
    VALIDITY = 1
    TIME = 2


OBSTACLE_POTENTIAL = 40  # Potential for obstacles.
POTENTIAL_TIME_FRAME = 40  # Time frame for renewing potential.


class PotentialField:
    """
    A class representing the potential field over the Star-craft map. Used for setting potential, time and validity over
    each 1x1 square of the map referring to the maps width and height as boundary. Can find and set potential for square
    and also squares within some radius. Can also find the lowest or highest potential of the map within some radius.

    Attributes:
        map_tools       The maptools API in star craft
        height          Height of map
        width           Width of map
        start_time      Time when potentialField was initialized
        potential_field 3D Matrix of the map using np.array. Information regarding potential, validity, time for tiles.
    """

    def __init__(self, map_tools: MapTools):
        """
        Constructor for PotentialField class

        :param map_tools: MapTools API for Star-craft
        """
        assert isinstance(map_tools, MapTools), "Invalid type"
        assert map_tools.height >= 0 and map_tools.width >= 0, "Invalid map boundary"
        self.map_tools = map_tools
        self.height = map_tools.height
        self.width = map_tools.width

        self.start_time = time.time()  # Starting timer when initialize instance

        # Map the potential, validity and time. Use potential_field[_x][_y][Mapping.POTENTIAL.value] = _value
        # For setting validity: potential_field[_x][_y][Mapping.VALIDITY.value] = True/False --> Change digit to 1/0
        self.potential_field = np.ones((self.width, self.height, 3))
        self.set_initial_valid_points()

    # Getters and setters

    def set_potential(self, position: Point2D, potential):
        """
        Set potential for some position

        :param position: A point2D position
        :param potential: Int or float number.
        """
        assert isinstance(position, Point2D) and (isinstance(potential, float) or isinstance(potential, int)), \
            "Invalid type"
        assert self.is_valid_point(position), "Invalid position on map"
        self.potential_field[int(position.x)][int(position.y)][Mapping.POTENTIAL.value] = potential

    def get_potential(self, position: Point2D):
        """
        Get the potential of position. Renew potential if exceeding POTENTIAL_TIME_FRAME seconds and set potential 1.

        :param position: Point2D position
        :return: The potential of position.
        """
        assert isinstance(position, Point2D), "Invalid type"
        assert self.is_valid_point(position), "Invalid position on map"

        previous_time = self.get_time(position)
        # Renew time if exceeded POTENTIAL_TIME_FRAME
        if time.time() - previous_time > POTENTIAL_TIME_FRAME:
            self.set_potential(position, 1)
            self.set_time(position, time.time())
        return self.potential_field[int(position.x)][int(position.y)][Mapping.POTENTIAL.value]

    def set_valid_point(self, position: Point2D, validity: bool):
        """
        Set validity for position

        :param position: A Point2D position
        :param validity: Set validity true or false.
        """
        assert isinstance(position, Point2D) and isinstance(validity, bool), "Invalid type"
        assert self.is_valid_point(position), "Invalid position on map"
        self.potential_field[int(position.x)][int(position.y)][Mapping.VALIDITY.value] = validity

    def get_valid_point(self, position: Point2D) -> bool:
        """
        Get validity of position

        :param position: A Point2D position
        :return: true if point is valid, otherwise false. Might be binary 0=false, 1=true
        """
        assert isinstance(position, Point2D), "Invalid type"
        assert self.is_valid_point(position), "Invalid position on map"
        return self.potential_field[int(position.x)][int(position.y)][Mapping.VALIDITY.value]

    def set_time(self, position: Point2D, t: float) -> None:
        """
        Set time to position.

        :param position: A Point2D position
        :param t: Time to be set, use time.time() to set time.
        """
        assert isinstance(position, Point2D) and isinstance(t, float), "Invalid type"
        assert self.is_valid_point(position), "Invalid position on map"
        assert t >= 0.0, "Time cannot be negative"
        self.potential_field[int(position.x)][int(position.y)][Mapping.TIME.value] = t

    def get_time(self, position: Point2D) -> float:
        """
        Get time when potential was last changed for position

        :param position: A Point2D
        :return: The time set at that particular point, if returning 1 then potential has not been updated.
        """
        assert isinstance(position, Point2D), "Invalid type"
        assert self.is_valid_point(position), "Invalid position on map"
        return self.potential_field[int(position.x)][int(position.y)][Mapping.TIME.value]

    def is_valid_point(self, position: Point2D) -> bool:
        """
        Check if position is valid on the map.

        :param position: A Point2D position
        :return True if position is valid on the map otherwise false.
        """
        assert isinstance(position, Point2D), "Invalid type"
        return True if self.map_tools.is_valid_position(position) else False

    def set_initial_valid_points(self) -> None:
        """
        Set valid points for all points on the map.
        A point is valid iff valid on the map and is walkable by the units.
        If not valid then set potential to that point and surrounding points within potential_radius set to 1.
        """
        start_time = time.time()
        potential_radius = 1
        for x in range(self.width):
            for y in range(self.height):
                pos = Point2D(x, y)
                if self.is_valid_point(pos):
                    if not self.map_tools.is_walkable(x, y):
                        self.set_valid_point(pos, False)
                        self.set_area_potential(pos, potential_radius, OBSTACLE_POTENTIAL)

        print("Valid point time: ", time.time() - start_time)

    def optimal_potential(self, position: Point2D, radius: int, highest: bool = True) -> Point2D:
        """
        Retrieve the most optimal potential, 'optimal' in terms of searching for the lowest or highest potential within
        some radius related to the position. Only search for valid points and keep updating the current optimal point.
        If searching for highest point and found some higher potential, update point. If searching for lowest point and
        found some lower potential, then update point.

        :param position: Point2D position center.
        :param radius: Integer radius around position to be searched
        :param highest: If true, then search for the highest potential. If false then search for lowest.
        :return: The position that gives the highest or lowest potential.
        """

        assert isinstance(position, Point2D) and isinstance(radius, int) and isinstance(highest, bool), "Invalid type"
        assert radius > 0, "Radius cannot be negative or 0"

        if highest:
            curr_optimal_potential = -math.inf
        else:
            curr_optimal_potential = math.inf

        curr_optimal_point = Point2D()
        for x in range(int(position.x) - radius, int(position.x) + radius + 1):
            for y in range(int(position.y) - radius, int(position.y) + radius + 1):
                pos = Point2D(x, y)
                if self.is_valid_point(pos):
                    if not highest and self.get_potential(pos) < curr_optimal_potential \
                            or highest and self.get_potential(pos) > curr_optimal_potential:
                        curr_optimal_potential = self.get_potential(pos)
                        curr_optimal_point = pos
        return curr_optimal_point

    def get_area_potential(self, position: Point2D, radius: int) -> int:
        """
        Get the total potential for a position within radius, only if the position is valid for the map.

        :param position: The center position
        :param radius: Integer radius around center position
        :return: An integer which resembles the potential of the area
        """
        assert isinstance(radius, int) and isinstance(position, Point2D), "Invalid type"
        assert self.is_valid_point(position), "Invalid position"
        assert radius > 0, "Radius cannot be negative or 0"

        potential = 0
        for x in range(int(position.x) - radius, int(position.x) + radius + 1):
            for y in range(int(position.y) - radius, int(position.y) + radius + 1):
                pos = Point2D(x, y)
                if self.is_valid_point(pos):
                    potential += self.get_potential(pos)
        return potential

    def set_area_potential(self, position: Point2D, radius: int, potential: int, allow_time=False) -> None:
        """
        Setting the area potential around position within radius, only if the position is valid on the map.

        :param position: Point2D position which marks the center.
        :param radius: The radius around center position
        :param potential: An integer setting the potential
        :param allow_time: If true then will allow to set the current
        time to the position, if false then do not allow for instance static obstacles.
        """
        assert (isinstance(radius, int) and isinstance(position, Point2D) and isinstance(potential, int)), "Wrong types"
        assert radius > 0, "Radius cannot be negative or 0"
        assert self.is_valid_point(position), "Invalid position"

        curr_time = time.time()
        for x in range(int(position.x) - radius, int(position.x) + radius + 1):
            for y in range(int(position.y) - radius, int(position.y) + radius + 1):
                pos = Point2D(x, y)
                if self.is_valid_point(pos):
                    self.set_potential(pos, potential)
                    if allow_time:
                        self.set_time(pos, curr_time)

    def evasion_point(self, curr_pos: Point2D, next_point: Point2D, enemies: list) -> Point2D:
        """
        Calculate new position that depending on
            1. Attractive force to next_point
            2. Repulsive resulting forces to all Point2D:s in the list enemies.

        :param curr_pos: The current Point2D position
        :param next_point: The next/goal point we want to move to.
        :param enemies: List of either enemies or Point2D positions.
        :return The new Point2D position considering our inputs.
        """
        assert isinstance(curr_pos, Point2D) and isinstance(next_point, Point2D) and isinstance(enemies, list),\
            "Invalid input(s)"
        assert self.is_valid_point(curr_pos) and self.is_valid_point(next_point), "Invalid position"

        # Calculating repulsion
        F_repulsion = Point2D(0, 0)
        for enemy_pos in enemies:
            assert isinstance(enemy_pos, Point2D), "Data in 'enemies' does not contains the type Point2D"
            F_repulsion = F_repulsion + self.repulsion(curr_pos, enemy_pos)

        new_point = next_point + F_repulsion
        assert isinstance(new_point, Point2D), "Return type not valid"
        return new_point

    def repulsion(self, curr: Point2D, target: Point2D) -> Point2D:
        """
        Calculates the repulsion between two Point2D positions curr and target. By doing (curr - target) we receive
        the opposite resulted direction between the points. The initial force constant K_rep is set to 1

         :param curr: Current Point2D position
         :param target: The other Point2D position
         :return New Point2D coordinate.
        """
        assert isinstance(curr, Point2D) and isinstance(target, Point2D), "Invalid type(s)"
        assert self.is_valid_point(curr) and self.is_valid_point(target), "Invalid pos."
        K_rep = 1  # Repulsion constant, can modify how far away to evade the unit.
        return K_rep * (curr - target)

