import os.path
import matplotlib.pyplot as plot
import sys

from funcy import cycle, pairwise
from matplotlib.collections import LineCollection


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


def main():
    locations = read_locations(sys.argv[1])

    figure, axes = plot.subplots()

    for route, color in zip(read_routes(), cycle(('r', 'g', 'b', 'c', 'm', 'y'))):
        axes.add_collection(LineCollection(tuple(route_to_lines(locations, route)), colors=color))

    axes.set_xlim(-1, 1)
    axes.set_ylim(-1, 1)

    figure.set_figwidth(8)
    figure.set_figheight(8)

    plot.savefig(os.path.join(os.path.dirname(sys.argv[1]), os.path.splitext(os.path.basename(sys.argv[1]))[0] + ".png"))
    plot.show()


if __name__ == '__main__':
    main()
