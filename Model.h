#pragma once

#include <ranges>
#include <tuple>
#include <vector>

#include <boost/container/small_vector.hpp>

namespace lns {

using Locations = std::vector<std::tuple<float, float>>;
using Distances = std::vector<float>;
using DistanceMatrix = std::vector<Distances>;

using Route = boost::container::small_vector<int, 64>;
using Routes = boost::container::small_vector<Route, 16>;

class Problem final {
  int vehicleSize_;
  int vehicleCapacity_; // キャパシティは全車共通にします。あと、キャパシティはすべての顧客を回れる数に自動で設定します（だから問題ファイルにキャパシティの記述はありません）。
  int customerSize_;
  Locations locations_;
  DistanceMatrix distanceMatrix_;

public:
  explicit Problem(int vehicleSize, int customerSize, const Locations &locations, const DistanceMatrix &distanceMatrix) noexcept : vehicleSize_{vehicleSize}, vehicleCapacity_{(customerSize + vehicleSize - 1) / vehicleSize}, customerSize_{customerSize}, locations_{locations}, distanceMatrix_{distanceMatrix} {
    ;
  }

  auto getVehicleSize() const noexcept {
    return vehicleSize_;
  }

  auto getVehicleCapacity() const noexcept {
    return vehicleCapacity_;
  }

  auto getCustomerSize() const noexcept {
    return customerSize_;
  }

  const auto &getLocations() const noexcept {
    return locations_;
  }

  const auto &getDistanceMatrix() const noexcept {
    return distanceMatrix_;
  }
};

class Solution final {
  Routes routes_;

public:
  explicit Solution(const Routes &routes) noexcept : routes_{routes} {
    ;
  }

  const auto &getRoutes() const noexcept {
    return routes_;
  }
};

inline auto getCost(const Problem &problem, const Solution &solution) noexcept {
  auto result = 0.0f;

  for (const auto &route : solution.getRoutes()) {
    if (route.empty()) {
      continue;
    }

    result += problem.getDistanceMatrix()[problem.getCustomerSize()][route[0]];

    for (const auto &i : std::views::iota(1, static_cast<int>(std::size(route)))) {
      result += problem.getDistanceMatrix()[route[i - 1]][route[i]];
    }

    result += problem.getDistanceMatrix()[route.back()][problem.getCustomerSize()];
  }

  return result;
}

} // namespace lns
