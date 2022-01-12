#pragma once

#include <algorithm>
#include <chrono>
#include <limits>
#include <random>
#include <ranges>

#include "Model.h"

namespace lns {

constexpr float maxChangeRatio = 0.2;
constexpr int maxTrySize = 10000;

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

    auto result = solution;

    for (auto i = 0; i < removeSize;) {
      auto &route = result.getRoutes()[std::uniform_int_distribution{0, static_cast<int>(std::size(result.getRoutes())) - 1}(random_number_generator_)];

      if (std::empty(route)) {
        continue;
      }

      route.erase(std::begin(route) + std::uniform_int_distribution{0, static_cast<int>(std::size(route)) - 1}(random_number_generator_));

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

  auto operator()(const Solution &solution) noexcept {
    // 今回は、距離の増加が最も少ない場所に追加してみました。

    // 未訪問のお客様のインデックスを取得します。

    auto indices = [&] {
      auto result = std::vector<int>{};

      std::ranges::copy(
          std::views::iota(0, problem_.getCustomerSize()) | std::views::filter([&](const auto index) {
            for (const auto &route : solution.getRoutes()) {
              if (std::find(std::begin(route), std::end(route), index) != std::end(route)) {
                return false;
              }
            }

            return true;
          }),
          std::back_inserter(result));

      std::shuffle(std::begin(result), std::end(result), random_number_generator_); // 追加順によって結果が変わるので、シャッフルしておきます。

      return result;
    }();

    auto result = solution;

    // 未訪問のお客様を追加します。

    for (const auto &index : indices) {
      // 距離の増加が最も少ない場所を探します。

      auto [routeIt, it] = [&, &solution = result] {
        auto result = std::make_tuple(std::begin(solution.getRoutes()), std::begin(*std::begin(solution.getRoutes())));
        auto resultDelta = std::numeric_limits<float>::max();

        for (auto routeIt = std::begin(solution.getRoutes()); routeIt != std::end(solution.getRoutes()); ++routeIt) {
          for (auto it = std::begin(*routeIt); it != std::next(std::end(*routeIt)); ++it) {
            const auto prevIndex = it == std::begin(*routeIt) ? problem_.getCustomerSize() : *std::prev(it);
            const auto nextIndex = it == std::end(*routeIt) ? problem_.getCustomerSize() : *it;

            const auto delta = (problem_.getDistances()[prevIndex][index] + problem_.getDistances()[index][nextIndex]) - problem_.getDistances()[prevIndex][nextIndex];

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

  auto operator()(const Solution &initialSolution, const std::chrono::steady_clock::time_point &timeLimit, unsigned int seed = 0) noexcept {
    auto result = initialSolution;
    auto resultCost = getCost(problem_, result);

    auto random_number_generator = std::mt19937{seed};

    auto destroy = DestroyHeuristic{problem_, random_number_generator};
    auto rapair = RapairHeuristic{problem_, random_number_generator};

    auto tempSolution = result;

    for (;;) {
      for (auto i = 0; i < static_cast<int>(problem_.getCustomerSize() * maxChangeRatio); ++i) { // 破壊で削除されるお客様の数を少しずつ増やしていきます。
        for (auto j = 0; j < maxTrySize; ++j) {                                                  // 一定の回数トライします。
          if (std::chrono::steady_clock::now() > timeLimit) {
            return result;
          }

          tempSolution = rapair(destroy(tempSolution, i + 1));
          const auto cost = getCost(problem_, tempSolution);

          if (cost < resultCost) {
            std::cerr << cost << " <- " << resultCost << std::endl;

            result = tempSolution;
            resultCost = cost;

            j = 0; // このお客様削除数は有効だと考えて、トライを繰り返します。
          }
        }
      }
    }
  }
};

} // namespace lns
