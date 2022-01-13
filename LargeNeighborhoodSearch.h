#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>
#include <ranges>
#include <tuple>

#include <boost/container/small_vector.hpp>

#include "Model.h"

namespace lns {

constexpr float maxChangeRatio = 0.2;
constexpr int maxTrySize = 100'000;

using Indices = boost::container::small_vector<int, 16>;

// 削除用のヒューリスティック。
// とりあえず版です。問題に合わせて修正してください。

class DestroyHeuristic final {
  const Problem &problem_;
  std::mt19937 &random_number_generator_;

public:
  explicit DestroyHeuristic(const Problem &problem, std::mt19937 &random_number_generator) noexcept : problem_{problem}, random_number_generator_{random_number_generator} {
    ;
  }

  auto operator()(const Solution &solution, int removeSize) noexcept {
    // 今回は、ランダムに削除してみました。

    auto result = std::make_tuple(solution, Indices{});

    for (auto i = 0; i < removeSize;) {
      auto &route = std::get<0>(result).getRoutes()[std::uniform_int_distribution{0, static_cast<int>(std::size(std::get<0>(result).getRoutes())) - 1}(random_number_generator_)];

      if (std::empty(route)) {
        continue;
      }

      const auto it = std::begin(route) + std::uniform_int_distribution{0, static_cast<int>(std::size(route)) - 1}(random_number_generator_);

      std::get<1>(result).emplace_back(*it);
      route.erase(it);

      ++i;
    }

    return result;
  }
};

// 追加用のヒューリスティック。
// とりあえず版です。問題に合わせて修正してください。

class RapairHeuristic final {
  const Problem &problem_;
  std::mt19937 &random_number_generator_;

public:
  explicit RapairHeuristic(const Problem &problem, std::mt19937 &random_number_generator) noexcept : problem_{problem}, random_number_generator_{random_number_generator} {
    ;
  }

  auto operator()(const std::tuple<Solution, Indices> &solutionAndIndices) noexcept {
    // 今回は、距離の増加が最も少ない場所に追加してみました。

    auto [solution, indices] = solutionAndIndices;

    auto result = solution;

    // 未訪問のお客様を追加します。

    for (const auto &index : indices) {
      // 距離の増加が最も少ない場所を探します。

      auto [routeIt, it] = [&, &solution = result] {
        auto result = std::make_tuple(std::begin(solution.getRoutes()), std::begin(*std::begin(solution.getRoutes())));
        auto resultDelta = std::numeric_limits<float>::max();

        for (auto routeIt = std::begin(solution.getRoutes()); routeIt != std::end(solution.getRoutes()); ++routeIt) {
          if (std::size(*routeIt) >= problem_.getVehicleCapacity()) {
            continue;
          }

          for (auto it = std::begin(*routeIt); it != std::next(std::end(*routeIt)); ++it) {
            const auto prevIndex = it == std::begin(*routeIt) ? problem_.getCustomerSize() : *std::prev(it); // 最初はdepot。
            const auto nextIndex = it == std::end(*routeIt) ? problem_.getCustomerSize() : *it;              // 最後もdepot。

            const auto delta = (problem_.getDistanceMatrix()[prevIndex][index] + problem_.getDistanceMatrix()[index][nextIndex]) - problem_.getDistanceMatrix()[prevIndex][nextIndex];

            if (delta < resultDelta) {
              result = std::make_tuple(routeIt, it);
              resultDelta = delta;
            }
          }
        }

        return result;
      }();

      // 距離の増加が最も少ない場所に追加します。

      routeIt->insert(it, index);
    }

    return result;
  }
};

class LargeNeighborhoodSearch final {
  const Problem &problem_;

public:
  explicit LargeNeighborhoodSearch(const Problem &problem) noexcept : problem_{problem} {
    ;
  }

  auto operator()(const Solution &initialSolution, unsigned int seed = 0) noexcept {
    auto result = initialSolution;
    auto resultCost = getCost(problem_, result);

    auto random_number_generator = std::mt19937{seed};

    auto destroy = DestroyHeuristic{problem_, random_number_generator};
    auto rapair = RapairHeuristic{problem_, random_number_generator};

    auto tempSolution = result;

    for (const auto &i : std::views::iota(0, static_cast<int>(problem_.getCustomerSize() * maxChangeRatio))) { // 破壊で削除されるお客様の数を少しずつ増やしていきます。
      std::cerr << "Removing: " << (i + 1) << std::endl;

      for (auto j = 0; j < maxTrySize; ++j) { // 一定の回数トライします。
        tempSolution = rapair(destroy(tempSolution, i + 1));
        const auto cost = getCost(problem_, tempSolution);

        if (cost < resultCost) {
          std::cerr << cost << " <- " << resultCost << std::endl;

          result = tempSolution;
          resultCost = cost;

          j = -1; // このお客様削除数は有効だと考えて、トライをやり直します。
        }
      }
    }

    return result;
  }
};

} // namespace lns
