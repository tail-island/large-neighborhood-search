import random

from funcy import concat


def create_problem():
    vehicles_size = 5
    customers_size = 60
    locations = concat(map(lambda _: (random.uniform(-1, -0.75), random.uniform(-1, -0.75)), range(15)),
                       map(lambda _: (random.uniform(0.75, 1), random.uniform(-1, -0.75)), range(15)),
                       map(lambda _: (random.uniform(-1, -0.75), random.uniform(0.75, 1)), range(15)),
                       map(lambda _: (random.uniform(0.75, 1), random.uniform(0.75, 1)), range(15)))

    return vehicles_size, customers_size, locations


def print_problem(vehicles_size, customers_size, locations):
    print(f'{vehicles_size}')
    print(f'{customers_size}')
    for x, y in locations:
        print(f'{x}\t{y}')


def main():
    print_problem(*create_problem())


if __name__ == '__main__':
    main()
