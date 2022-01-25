import matplotlib.pyplot as plot
# import os.path
import sys

from funcy import cycle, pairwise, partial
from math import sqrt
from matplotlib.collections import LineCollection, RegularPolyCollection


def read_locations(path):
    def impl():
        with open(path) as f:
            f.readline()
            for _ in range(int(f.readline().rstrip('\n'))):
                yield tuple(map(float, f.readline().rstrip('\n').split()))

    return tuple(impl()) + ((0, 0),)


def read_routes():
    return tuple(map(lambda line: tuple(map(int, line.rstrip('\n').split())), sys.stdin.readlines()))


def route_to_lines(locations, route):
    for index1, index2 in pairwise((len(locations) - 1,) + route + (len(locations) - 1,)):
        yield locations[index1], locations[index2]


def route_to_distance(locations, route):
    result = 0

    for index1, index2 in pairwise((len(locations) - 1,) + route + (len(locations) - 1,)):
        result += sqrt((locations[index1][0] - locations[index2][0]) ** 2 + (locations[index1][1] - locations[index2][1]) ** 2)

    return result


def main():
    locations = read_locations(sys.argv[1])
    routes = read_routes()

    figure, axes = plot.subplots()

    for route, color in zip(routes, cycle(('r', 'g', 'b', 'c', 'm', 'y'))):
        axes.add_collection(LineCollection(tuple(route_to_lines(locations, route)), colors=color))

    axes.add_collection(RegularPolyCollection(len(locations), sizes=(25,), offsets=locations, transOffset=axes.transData, color='black'))

    axes.set_xlim(-1, 1)
    axes.set_ylim(-1, 1)

    figure.set_figwidth(8)
    figure.set_figheight(8)

    print(sum(map(partial(route_to_distance, locations), routes)))

    # plot.savefig(os.path.join(os.path.dirname(sys.argv[1]), os.path.splitext(os.path.basename(sys.argv[1]))[0] + ".png"))
    plot.show()


if __name__ == '__main__':
    main()
