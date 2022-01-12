#pragma once

#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>

#include "Model.h"

namespace lns {

// 初期ルート作成用の貪欲法アルゴリズム。
// とりあえず版です。もっと良い初期ルート作成用のアルゴリズムを使用してください。

class GreedyAlgorithm final {
  const Problem &problem_;

public:
  GreedyAlgorithm(const Problem &problem) noexcept : problem_{problem} {
    ;
  }

  auto operator()() const noexcept {
    const auto routes = [&] {
      auto indices = [&] {
        auto result = std::vector<int>{};

        std::ranges::copy(
            std::views::iota(0, problem_.getCustomerSize()),
            std::back_inserter(result));

        return result;
      }();

      auto result = std::vector<std::vector<int>>{};

      std::ranges::copy(
          std::views::iota(0, problem_.getVehicleSize()) | std::views::transform([&](const auto &_) {
            auto result = std::vector<int>{};

            for (auto i = 0; i < (problem_.getCustomerSize() + problem_.getVehicleSize() - 1) / problem_.getVehicleSize() && !std::empty(indices); ++i) {
              const auto lastIndex = std::empty(result) ? problem_.getCustomerSize() : result.back();

              const auto it = std::ranges::min_element(indices, [&](const auto &index1, const auto &index2) {
                return problem_.getDistances()[lastIndex][index1] < problem_.getDistances()[lastIndex][index2];
              });

              result.emplace_back(*it);

              indices.erase(it);
            }

            return result;
          }),
          std::back_inserter(result));

      return result;
    }();

    return Solution{routes};
  }
};

} // namespace lns
