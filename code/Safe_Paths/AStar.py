from Safe_Paths.Node import Node
from queue import PriorityQueue, LifoQueue
from library import Point2D
from Safe_Paths.PotentialField import PotentialField
import time

CLOSENESS_RADIUS = 5  # The maximum radius between our point and the destination


def astar_search(start: Point2D, goal: Point2D, pot_field: PotentialField) -> LifoQueue:
    """
    Do A*-algorithm which will return a path from start to goal, considering lowest f-score and the potential on
    certain points on the map.

    :param start: A Point2D position where the search shall begin
    :param goal: A Point2D position where the search shall end
    :param pot_field: The potential field.
    :return: LifoQueue(Last-in, first-out queue) from start to end node.
     Return empty LifoQueue if no path found or already at goal position.
    """

    assert isinstance(start, Point2D) and isinstance(goal, Point2D) and isinstance(pot_field, PotentialField), \
        "Invalid type(s)"
    assert pot_field.is_valid_point(start) and pot_field.is_valid_point(goal), "Invalid positions"

    # Create nodes from position input
    start_node = Node(start, pot_field)
    goal_node = Node(goal, pot_field)

    # Return no queue if already close to end position.
    if start_node.is_within_radius(goal_node, CLOSENESS_RADIUS):
        return LifoQueue()

    # Datastructures containing nodes.
    created_nodes = dict()
    open_list = PriorityQueue()  # Sorted by lowest f-score.
    closed_list = []

    # Put the start node in open_list and initialize start_node values.
    start_node.g = 0
    start_node.f = start_node.euclidian_distance_heuristic(goal_node)
    open_list.put((start_node.f, start_node))

    start_time = time.time()    # Time stamping the time it takes to execute this function, used for debugging

    # Do A* as long as open_list contains elements to consider.
    while not open_list.empty():

        # Get the current node and append to closed_list
        curr = open_list.get()[1]
        closed_list.append(curr)

        # If within CLOSENESS_RADIUS to the goal, then backtrack path from end to start.
        if curr.is_within_radius(goal_node, CLOSENESS_RADIUS):
            print("Total A* time: ", time.time() - start_time)
            return backtrack_path(curr, goal_node)

        # Get all neighbors that are not yet closed.
        for neighbor in neighbors_within_radius(curr, created_nodes, closed_list, pot_field, CLOSENESS_RADIUS):
            tentative_g = curr.g + curr.distance_to(neighbor)

            # If the new g-score is better than assign new data to the neighbor.
            if tentative_g < neighbor.g:
                neighbor.g = tentative_g
                potential = pot_field.get_potential(neighbor.position)
                neighbor.f = tentative_g + potential * neighbor.euclidian_distance_heuristic(goal_node)

                # Put neighbor in open_list if not already there.
                if not is_within_pqueue(open_list, neighbor):
                    open_list.put((neighbor.f, neighbor))
    return LifoQueue()  # Could not find solution.


def is_within_pqueue(pqueue: PriorityQueue, node: Node) -> bool:
    """
    Check if the priorityQueue open_list contains a certain node

    :param pqueue: The priorityqueue that the item shall or shall not contain
    :param node: A node
    :return: True if pqueue contains node otherwise False
    """
    assert isinstance(pqueue, PriorityQueue) and isinstance(node, Node), "Invalid type(s)"
    return True if node in (x[1] for x in pqueue.queue) else False


def backtrack_path(node: Node, goal_node: Node) -> LifoQueue:
    """
    Reconstruct path from start to goal node, using node parents. If not already added, then add the goal node first.
    Then by using their parents the node can reach their neighbors along the path.

    :param node: The latest node found along the path
    :param goal_node: The end goal along the path.
    :return: LifoQueue from start to end node
    """
    assert isinstance(node, Node) and isinstance(goal_node, Node), "Invalid type(s)"

    # Add the goal node if possible
    path = LifoQueue()
    if goal_node != node:
        path.put(goal_node)
    # Use the parents to achieve the result.
    while node.parent is not None:
        path.put(node)
        node = node.parent
    return path


def neighbors_within_radius(node: Node, created_nodes: dict, closed_list: list, potential_field: PotentialField,
                            radius: int) -> list:
    """
    Loop over 'valid' neighbors for a given node within some given radius. Valid in terms of the position is on the map,
    the neighbor is not current position and closed_list does not contain it. Then returns list of valid neighbors from
    position and create node of this position if not already had done so.

    :param node: The node that we shall check its valid neighbors
    :param created_nodes: Dictionary of nodes created already in the form of [position]: Node
    :param closed_list:The closed list of nodes.
    :param potential_field:The potential field
    :param radius: The radius we shall check within as closeness to the node
    :return: list of valid neighbors.
    """

    assert isinstance(node, Node) and isinstance(created_nodes, dict) and isinstance(closed_list, list) and \
           isinstance(potential_field, PotentialField) and isinstance(radius, int), "Invalid type(s)"
    assert radius > 0, "Radius cannot be negative or 0"

    valid_neighbors = []
    pos_x = int(node.position.x)
    pos_y = int(node.position.y)
    # Iterate through adjacent positions for a node stepping radius steps per iteration.
    for x in range(pos_x - radius, pos_x + radius + 1, radius):
        for y in range(pos_y - radius, pos_y + radius + 1, radius):
            neighbor_pos = Point2D(x, y)
            # Valid neighbor if not on node position, not in closed_list and valid place on the map.
            if (node.position != neighbor_pos) and potential_field.is_valid_point(neighbor_pos) and \
                    (closed_list.count(neighbor_pos) == 0):

                # Create node of position if not already done so. Otherwise, get neighbor_node from created_nodes dict.
                if neighbor_pos not in created_nodes:
                    neighbor_node = Node(position=neighbor_pos, potential_field=potential_field, parent=node)
                    created_nodes[neighbor_pos] = neighbor_node
                else:
                    neighbor_node = created_nodes[neighbor_pos]
                valid_neighbors.append(neighbor_node)
    return valid_neighbors
