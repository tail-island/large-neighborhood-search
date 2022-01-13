#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <ranges>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "Model.h"

namespace lns {

template <typename T>
inline auto readData(std::istream &stream) noexcept {
  auto result = T{};

  stream >> result;

  return result;
}

inline auto readProblem(std::istream &stream) noexcept {
  const auto vehicleSize = readData<int>(stream);

  const auto customerSize = readData<int>(stream);

  const auto locations = [&] {
    auto result = std::vector<std::tuple<float, float>>{};

    std::ranges::copy(
        std::views::iota(0, customerSize) | std::views::transform([&](const auto &_) {
          const auto x = readData<float>(stream);
          stream.get(); // 区切り文字のtabを飛ばします。
          const auto y = readData<float>(stream);

          return std::make_tuple(x, y);
        }),
        std::back_inserter(result));

    result.emplace_back(0, 0); // 後続処理が楽になるように、depotを追加しておきます。

    return result;
  }();

  const auto distances = [&] {
    auto result = std::vector<std::vector<float>>{};

    std::ranges::copy(
        locations | std::views::transform([&](const auto &location1) {
          auto result = std::vector<float>{};

          std::ranges::copy(
              locations | std::views::transform([&](const auto &location2) {
                return std::sqrt(std::pow(std::get<0>(location1) - std::get<0>(location2), 2.0f) + std::pow(std::get<1>(location1) - std::get<1>(location2), 2.0f));
              }),
              std::back_inserter(result));

          return result;
        }),
        std::back_inserter(result));

    return result;
  }();

  return Problem{vehicleSize, customerSize, locations, distances};
}

inline auto writeSolution(const Solution &solution, std::ostream &stream) noexcept {
  auto result = std::ostringstream{};

  std::ranges::copy(
      solution.getRoutes() | std::views::transform([&](const auto &route) {
        auto result = std::ostringstream{};

        if (std::empty(route)) {
          return result.str();
        }

        std::copy(std::begin(route), std::prev(std::end(route)), std::ostream_iterator<int>(result, "\t"));
        result << *std::prev(std::end(route));

        return result.str();
      }),
      std::ostream_iterator<std::string>(result, "\n"));

  stream << result.str();
}

} // namespace lns
