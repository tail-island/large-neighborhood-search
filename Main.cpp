#include <iostream>

#include "GreedyAlgorithm.h"
#include "IO.h"
#include "LargeNeighborhoodSearch.h"
#include "Model.h"

int main(int argc, char **argv) {
  const auto problem = lns::readProblem(std::cin);
  const auto solution = lns::LargeNeighborhoodSearch{problem}(lns::GreedyAlgorithm{problem}());

  lns::writeSolution(solution, std::cout);

  return 0;
}
