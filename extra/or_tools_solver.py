import numpy as np
import sys

from funcy import complement, drop, iterate, map, repeat, takewhile
from ortools.constraint_solver import pywrapcp, routing_enums_pb2


def read_problem(file):
    def read_locations():
        for _ in range(customer_count):
            yield tuple(map(float, file.readline().rstrip('\n').split()))

    vehicle_count = int(file.readline())
    customer_count = int(file.readline())
    locations = np.array(tuple(read_locations()) + ((0, 0),), dtype=np.float32)
    distance_matrix = np.sum(np.abs(locations[:, None, :] - locations[None, :, :]), axis=-1)

    return {'vehicle_count': vehicle_count, 'capacity': int((customer_count + vehicle_count - 1) / vehicle_count), 'customer_count': customer_count, 'locations': locations, 'distance_matrix': distance_matrix}


def solve(problem):
    def get_distance_matrix():
        return list(map(lambda distances: tuple(map(lambda distance: int(distance * 1e7), distances)), problem['distance_matrix']))

    def get_demands_vector():
        return tuple(repeat(1, problem['customer_count'])) + (0,)

    def get_capacity_vector():
        return tuple(repeat(problem['capacity'], problem['vehicle_count']))

    routing_manager = pywrapcp.RoutingIndexManager(problem['customer_count'] + 1, problem['vehicle_count'], problem['customer_count'])
    routing_model = pywrapcp.RoutingModel(routing_manager)

    routing_model.SetArcCostEvaluatorOfAllVehicles(routing_model.RegisterTransitMatrix(get_distance_matrix()))
    routing_model.AddDimensionWithVehicleCapacity(routing_model.RegisterUnaryTransitVector(get_demands_vector()), 0, get_capacity_vector(), True, 'Capacity')

    search_parameters = pywrapcp.DefaultRoutingSearchParameters()
    search_parameters.time_limit.seconds = 30
    search_parameters.local_search_metaheuristic = routing_enums_pb2.LocalSearchMetaheuristic.GUIDED_LOCAL_SEARCH

    routing_solution = routing_model.SolveWithParameters(search_parameters)

    if not routing_solution:
        return ((),)

    def create_route(vehicle):
        return tuple(drop(1, takewhile(complement(routing_model.IsEnd), iterate(lambda index: routing_solution.Value(routing_model.NextVar(index)), routing_model.Start(vehicle)))))

    return tuple(map(create_route, range(problem['vehicle_count'])))


def main():
    problem = read_problem(sys.stdin)
    solution = solve(problem)

    for route in solution:
        for index in route:
            print(index, end='\t')
        print()


if __name__ == '__main__':
    main()
