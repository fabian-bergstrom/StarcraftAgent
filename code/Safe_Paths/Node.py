from __future__ import annotations

from library import Point2D
from Safe_Paths.PotentialField import PotentialField
import math
from typing import Union

Point2D.distance = lambda self, other: math.sqrt((self.x - other.x) ** 2 + (self.y - other.y) ** 2)


class Node:
    """
    This class represents Nodes over containing additional data for 1x1 positions on the Starcraft map.
    Contains f and g-score initially set to math.inf which will be used in AStar.py.
    Con contain parent Node which in turn creates a binary tree of nodes for easier navigation.
    Can calculate distance between the nodes and checking whether this distance is within some radius.

    Attributes:
        position        The valid Point2D position on the map
        parent          The parent Node
        f               f-cost used in AStar.py
        g               g-cost used in AStar.py
        potential_field The potentialField instance

    """

    def __init__(self, position: Point2D, potential_field: PotentialField, parent: Node = None):
        """
        Constructor for Node class

        :param position: The point2D position
        :param potential_field: The connected potentialField instance for this node.
        :param parent: The parent node, default None.
        """

        assert potential_field.is_valid_point(position), "Invalid position"
        assert isinstance(position, Point2D) and isinstance(potential_field, PotentialField), "Invalid type"
        if parent is not None:
            assert isinstance(parent, Node), "Invalid type for parent."

        self.position = Point2D(int(position.x), int(position.y))
        self.parent = parent
        self.f = math.inf
        self.g = math.inf
        self.potential_field = potential_field

    def distance_to(self, other: Union[Node, Point2D]) -> float:
        """
        Calculate the distance between this node and other position iff the position is valid.

        :param other: The other node or position we want the distance to relate to this node.
        :return: The distance (float) between the two points if valid position, otherwise return infinity. A number > 0
        """
        assert isinstance(other, Node) or isinstance(other, Point2D), "Invalid type"

        if isinstance(other, Node):
            assert self.potential_field.is_valid_point(other.position), "Invalid position"
            return self.position.distance(other.position)

        elif isinstance(other, Point2D):
            assert self.potential_field.is_valid_point(other), "Invalid position"
            return self.position.distance(other)

    def euclidian_distance_heuristic(self, goal: Node) -> float:
        """
        Heuristic used by AStar.py, identical to distance_to method.

        :params goal: The goal node, used in AStar.py
        :return: The Euclidian distance between this node position and the goal position. A number > 0
        """
        assert isinstance(goal, Node), "Invalid type"
        return self.position.distance(goal.position)

    def is_within_radius(self, other: Union[Node, Point2D], radius: int) -> bool:
        """
        Check whether the other node or point is close within this node within a certain radius,

        :params other: The other node or point that shall be checked
        :params radius: The allowed radius between the points.
        :return: True if within radius, otherwise false.
        """
        assert isinstance(other, Node) or isinstance(other, Point2D), "'other' is invalid type"
        assert isinstance(radius, int), "radius is invalid type"
        assert (radius > 0), "Radius cannot be negative or 0"

        return True if self.distance_to(other) <= radius else False

    def __eq__(self, other):
        assert isinstance(other, Node) or isinstance(other, Point2D), "Invalid type"
        if isinstance(other, Node):
            return self.position == other.position
        elif isinstance(other, Point2D):
            return self.position == other

    def __ne__(self, other: Union[Node, Point2D]) -> bool:
        return not (self.position == other)

    def __lt__(self, other: Node) -> bool:
        assert isinstance(other, Node), "Invalid type"
        return self.f < other.f

    def __gt__(self, other: Node) -> bool:
        assert isinstance(other, Node), "Invalid type"
        return self.f > other.f

    def __le__(self, other: Node) -> bool:
        return (self < other) or (self.position == other)

    def __ge__(self, other: Node) -> bool:
        return (self > other) or (self.position == other)
