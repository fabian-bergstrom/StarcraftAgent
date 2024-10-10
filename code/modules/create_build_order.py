from modules.Init_buildings import *
import math

startOrder = [[Refinery, Refinery, [Ghost, 5], Missile_Turret]]
AttackOrder = [[[Marine, 10], [Reaper, 5]]]
Expand = [[Command_Center, Refinery, Refinery]]


"""Function to evaluate the income rate and evaluate node 
costs in seconds depending on amount of workers"""
def evaluate_time(workers):
    for node in Building_tree.nodes:
        node.minerals_sec = get_mineral_sec(workers, node.minerals)
        node.gas_sec = get_gas_sec(workers, node.gas)

        node.evaluated_cost = (node.minerals_sec + node.gas_sec
                               + node.build_time)


def get_mineral_sec(workers, minerals_cost):
    minerals_per_frame = 0.045 * (workers * 0.8)  # average percentage working with minerals = 80%
    minerals_per_second = minerals_per_frame * 24
    return minerals_cost / minerals_per_second


def get_gas_sec(workers, gas_cost):
    gas_per_frame = 0.07 * (workers * 0.2)  # average percentage working with gas = 20%
    gas_per_second = gas_per_frame * 24
    return gas_cost / gas_per_second

def evaluate_number_of_buildings(path, node, bestSolution, units, saved_time):

    new_parents = 0
    best = 0
    lowest_cost = 0
    if node.parent.built == 0:
        parents = 1
    else:
        parents = node.parent.built
    for i in range(len(bestSolution) - 1):  # take already planned buildings into account
        if not isinstance(bestSolution[i], str):
            if bestSolution[i] == node.parent and bestSolution[i + 1] == "final":
                new_parents += 1
    if new_parents > 0:
        new_parents -= 1

    for i in range(units + 1):  # find best amount of buildings to have to train all units faster
        cost = ((math.ceil((units) / (i + parents + new_parents))) * node.build_time) + (
                i * node.parent.evaluated_cost)
        if cost < lowest_cost or lowest_cost == 0:
            lowest_cost = cost
            best = i

    for i in range(units - 1):  # add units to buildorder
        path.insert(0, "final")
        path.insert(0, node)
    if not node.techlab_needed:  # add best amout of buildings if not extra techlabs is needed
        saved_time = (units * node.build_time) - (
                (math.ceil((units) / (best + parents + new_parents))) * node.build_time)
        for i in range(best):
            path.insert(0, "final")
            path.insert(0, node.parent)
    return [path, saved_time]


"""
Function to create path to goal node and find the best amount of buildings
if goalnode is a unit, to see if adding extra buildings for concurrent training
to be faster overall"""
def path_to_goal_node(node, units, bestSolution, added_workers):

    goal = node
    path = [node, "final"]
    saved_time = 0
    workers = False

    if units != 0:  # if goalnode is a unit
        best_evaluation = evaluate_number_of_buildings(path, node, bestSolution, units, saved_time)
        path = best_evaluation[0]
        saved_time = best_evaluation[1]

    while node.parent and node.parent != Root:  # insert path to get to goal node
        if node == Worker:
            workers = True
            added_workers -= 1
        if added_workers != 0 or workers == False:
            path.insert(0, node.parent)
            node = node.parent
        else:
            node.parent = Root

    final_path = []
    for i in range(len(path)):  # remove all nodes in path that is not wanted/requirement already fulfilled
        if not path[i] == "final":
            if path[i].built == 0 or path[i] == goal or path[i] == Worker or path[i + 1] == "final":
                final_path.append(path[i])
        else:
            final_path.append(path[i])
    return [final_path, saved_time]


"""
Function to return all wanted nodes in buildorder to fulfill all goalstates
"""
def path_names(path, savedTime):

    workers = Worker.built
    cost = 0
    final_path = []
    current_mineral = Building_tree.minerals
    current_gas = Building_tree.gas
    for nodeindex in range(len(path)):
        if not path[nodeindex] == "final":
            if (path[nodeindex].name not in final_path) or (path[nodeindex + 1] == "final") or path[nodeindex] == Worker:
                final_path.append(path[nodeindex].name)
                cost += path[nodeindex].evaluated_cost
                if (current_mineral - path[nodeindex].minerals) > 0:
                    cost -= get_mineral_sec(workers, path[nodeindex].minerals)
                    current_mineral -= path[nodeindex].minerals
                else:
                    cost -= get_mineral_sec(workers, current_mineral)
                    current_mineral = 0
                if (current_gas - path[nodeindex].gas) > 0:
                    cost -= get_gas_sec(workers, path[nodeindex].gas)
                    current_gas -= path[nodeindex].gas
                else:
                    cost -= get_gas_sec(workers, current_gas)
                    current_gas = 0
                if path[nodeindex] == Worker:
                    workers += 1
                    evaluate_time(workers)
    return [final_path, cost - savedTime]


"""Function to find the best build order with amount of workers in mind
, done by calculating supply space left after build order is done and afterwards 
evaluating if adding workers/1 extra supply depot and workers is worth it"""
def find_build_order(buildorder):
    evaluate_time(Worker.built)
    supply_left = 0  #zero in case scv will fill all up
    supply_left_now = Building_tree.supply  # See current supply left
    for node in buildorder:
        if isinstance(node, list):
            supply_left -= node[0].supply_spots * node[1]
    print(supply_left, "supply")

    while supply_left < 0:  # see how much supply is left after goal build is done
        buildorder.insert(0, Supply_depot)
        supply_left += Supply_depot.supply_spots
    return depth_first_and_bound(buildorder, Root, [Root],  0, 0, True, [], supply_left_now, False)


"""
Function to find paths to all goalnodes with 
their required buildings in mind
"""
def depth_first_and_bound(goalNode, state, queue, added_workers,
                       units_to_be_built, try_more_workers, solutions, supply_left_now, skip_next):

    queue.pop(0)
    if state == Worker:
        added_workers += 1
        supply_left_now -= 1
        if len(Worker.children) != 4 and try_more_workers:
            Worker.children.append(Worker)
            Worker.children.append(Command_Center)
            Worker.children.append(Supply_depot)
            Worker.children.append(Refinery)
            Root.children.pop(0)
    if isinstance(goalNode[0], list):
        units_to_be_built = goalNode[0][1]
        goalNode[0] = goalNode[0][0]
    if state == goalNode[0]:
        if solutions:
            path_to_goal_and_saved_time = path_to_goal_node(state, units_to_be_built, solutions[0][0], added_workers)
        else:
            path_to_goal_and_saved_time = path_to_goal_node(state, units_to_be_built, solutions, added_workers)

        if try_more_workers:
            solutions += [[path_to_goal_and_saved_time[0], path_to_goal_and_saved_time[1]]]

        else:
            if not skip_next:
                for solution in solutions:
                    solution[0] += path_to_goal_and_saved_time[0]
                    solution[1] += path_to_goal_and_saved_time[1]
        skip_next = False
    else:
        for child in state.children:
            if child != Worker or supply_left_now != -1:
                child.parent = state
                queue.insert(0, child)
            else:
                added_workers = 0
                skip_next = True
                try_more_workers = False
    while queue:
        return depth_first_and_bound(goalNode, queue[0], queue, added_workers, units_to_be_built,
                                    try_more_workers, solutions, supply_left_now, skip_next)
    goalNode.pop(0)
    if not queue and goalNode:
        return depth_first_and_bound(goalNode, Root, [Root], added_workers, 0,
                                    try_more_workers, solutions, supply_left_now, skip_next)
    else:
        Worker.children = []
        Root.children.insert(0, Worker)
        best = []
        for solution in solutions:
            if best == [] or path_names(solution[0], solution[1])[1] < best[1]:
                best = path_names(solution[0], solution[1])
        return best
