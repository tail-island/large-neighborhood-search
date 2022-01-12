#include <chrono>
#include <iostream>

#include "GreedyAlgorithm.h"
#include "IO.h"
#include "LargeNeighborhoodSearch.h"
#include "Model.h"

int main(int argc, char **argv) {
  const auto startTime = std::chrono::steady_clock::now();

  const auto problem = lns::readProblem(std::cin);
  const auto solution = lns::LargeNeighborhoodSearch{problem}(lns::GreedyAlgorithm{problem}(), startTime + std::chrono::seconds(5));

  lns::writeSolution(solution, std::cout);

  return 0;
}
