#pragma once

#include <ranges>
#include <tuple>
#include <vector>

namespace lns {

class Problem final {
  int vehicleSize_;
  int customerSize_;
  std::vector<std::tuple<float, float>> locations_;
  std::vector<std::vector<float>> distances_;

public:
  explicit Problem(int vehiclesSize, int customersSize, const std::vector<std::tuple<float, float>> &locations, const std::vector<std::vector<float>> &distances) noexcept : vehicleSize_{vehiclesSize}, customerSize_{customersSize}, locations_{locations}, distances_{distances} {
    ;
  }

  auto getVehicleSize() const noexcept {
    return vehicleSize_;
  }

  auto getCustomerSize() const noexcept {
    return customerSize_;
  }

  const auto &getLocations() const noexcept {
    return locations_;
  }

  const auto &getDistances() const noexcept {
    return distances_;
  }
};

class Solution final {
  std::vector<std::vector<int>> routes_;

public:
  explicit Solution(const std::vector<std::vector<int>> &routes) noexcept : routes_{routes} {
    ;
  }

  const auto &getRoutes() const noexcept {
    return routes_;
  }

  auto &getRoutes() noexcept {
    return routes_;
  }
};

inline auto getCost(const Problem &problem, const Solution &solution) noexcept {
  auto result = 0.0f;

  for (const auto &route : solution.getRoutes()) {
    if (route.empty()) {
      continue;
    }

    result += problem.getDistances()[problem.getCustomerSize()][route[0]];

    for (const auto &i : std::views::iota(1, static_cast<int>(std::size(route)))) {
      result += problem.getDistances()[route[i - 1]][route[i]];
    }

    result += problem.getDistances()[route.back()][problem.getCustomerSize()];
  }

  return result;
}

} // namespace lns
