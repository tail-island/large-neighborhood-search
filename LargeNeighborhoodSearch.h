#pragma once

#include <algorithm>
#include <iterator>
#include <limits>
#include <random>
#include <ranges>
#include <tuple>

#include <boost/container/small_vector.hpp>

#include "Model.h"

namespace lns {

constexpr float maxChangeRatio = 0.5;
constexpr int maxTrySize = 200'000;

using Indices = boost::container::small_vector<int, static_cast<int>(maxCapacity * maxChangeRatio)>;
using RandomEngine = std::mt19937;

// 削除用のヒューリスティック。
// とりあえず版です。問題に合わせて修正してください。

class DestroyHeuristic final {
  const Problem &problem_;
  RandomEngine &randomEngine_;

public:
  explicit DestroyHeuristic(const Problem &problem, RandomEngine &randomEngine) noexcept : problem_{problem}, randomEngine_{randomEngine} {
    ;
  }

  auto operator()(const Solution &solution, int removeSize) noexcept {
    // 今回は、ランダムに削除してみました。

    auto result = std::make_tuple(solution.getRoutes(), Indices{});

    for (auto i = 0; i < removeSize;) {
      const auto weights = [&, &routes = std::get<0>(result)] { // ルート選択の重みを作成します。重み付けしないと、短いルートのお客様が選択されやすくなってしまうため。
        auto result = boost::container::small_vector<int, static_cast<int>(maxCapacity * maxChangeRatio)>{};

        std::ranges::copy(
            routes | std::views::transform([](const auto &route) { return std::size(route); }),
            std::back_inserter(result));

        return result;
      }();

      auto &route = std::get<0>(result)[std::discrete_distribution<>{std::begin(weights), std::end(weights)}(randomEngine_)];

      if (std::empty(route)) {
        continue;
      }

      const auto it = std::begin(route) + std::uniform_int_distribution{0, static_cast<int>(std::size(route)) - 1}(randomEngine_);

      std::get<1>(result).emplace_back(*it);
      route.erase(it);

      ++i;
    }

    return std::make_tuple(Solution{std::get<0>(result)}, std::get<1>(result));
  }
};

// 追加用のヒューリスティック。
// とりあえず版です。問題に合わせて修正してください。

class RapairHeuristic final {
  const Problem &problem_;
  RandomEngine &randomEngine_;

public:
  explicit RapairHeuristic(const Problem &problem, RandomEngine &randomEngine) noexcept : problem_{problem}, randomEngine_{randomEngine} {
    ;
  }

  auto operator()(const std::tuple<Solution, Indices> &solutionAndIndices) noexcept {
    // 今回は、距離の増加が最も少ない場所に追加してみました。

    const auto &[solution, indices] = solutionAndIndices;

    auto result = solution.getRoutes();
    auto resultIndices = indices;

    std::ranges::shuffle(resultIndices, randomEngine_); // 追加順で結果が変わるので、念の為にシャッフルしておきます。

    for (const auto &index : resultIndices) {
      auto [routeIt, it] = [&, &routes = result] { // 距離の増加が最も少ない場所を探します。
        auto result = std::make_tuple(std::begin(routes), std::begin(*std::begin(routes)));
        auto resultDelta = std::numeric_limits<float>::max();

        for (auto routeIt = std::begin(routes); routeIt != std::end(routes); ++routeIt) {
          if (std::size(*routeIt) >= problem_.getVehicleCapacity()) { // キャパシティを超えないようにします。制約を満たしているかをLargeNeightborhoodSearchでチェックしていないので、Repairの中で解の正しさを保証しなければなりません。
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

      routeIt->insert(it, index);
    }

    return Solution{result};
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

    auto randomEngine = RandomEngine{seed};

    auto destroy = DestroyHeuristic{problem_, randomEngine};
    auto rapair = RapairHeuristic{problem_, randomEngine};

    auto tempSolution = result;

    for (const auto &i : std::views::iota(0, static_cast<int>(problem_.getCustomerSize() * maxChangeRatio))) { // 破壊時に削除されるお客様の数を少しずつ増やしていきます。
      for (auto improved = true; improved;) {
        improved = false;

        for (const auto &_ : std::views::iota(0, maxTrySize)) { // 一定の回数トライします。お客様削除数に合わせて増やした方が良いかもしれません。。。
          tempSolution = rapair(destroy(tempSolution, i + 1));
          const auto cost = getCost(problem_, tempSolution);

          if (cost < resultCost) {
            result = tempSolution;
            resultCost = cost;

            improved = true;
            break;
          }
        }
      }
    }

    return result;
  }
};

} // namespace lns
