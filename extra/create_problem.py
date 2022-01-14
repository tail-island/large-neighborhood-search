import random


def create_problem():
    vehicles_size = random.randint(2, 6)
    customers_size = random.randint(32, 100)
    locations = map(lambda _: (random.uniform(-1, 1), random.uniform(-1, 1)), range(customers_size))

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
