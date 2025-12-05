#include "../../common/advent_of_code.hpp"
#include "./2025.hpp"
#include <vector>
#include <span>
#include <utility>
#include <regex>
#include <ranges>
#include <algorithm>
#include <map>
#include "../../util/position.hpp"

// https://adventofcode.com/2025/day/5

// Pretty sure this is identical to a previous year problem

namespace {

struct Range {
  std::int64_t min;
  std::int64_t max;

  [[nodiscard]] bool contains(std::int64_t value) const noexcept {
    return min <= value && value <= max;
  }

  bool try_fuse(const Range & other) {
    if (other.max + 1 < min || other.min > max + 1) {
      return false;
    }

    min = std::min(min, other.min);
    max = std::max(max, other.max);
    return true;
  }

};

std::pair<
  std::vector<Range>,
  std::vector<std::int64_t>
> boring_input_processing(const std::vector<std::string> & lines) {
  std::vector<Range> ranges;
  std::vector<std::int64_t> values;

  for (const std::string & line : lines) {
    if (line.empty()) continue;

    const auto dash_pos = line.find('-');
    if (dash_pos != std::string::npos) {
      const auto min = std::stoll(line.substr(0, dash_pos));
      const auto max = std::stoll(line.substr(dash_pos + 1));
      ranges.push_back(Range{min, max});
    } else {
      values.push_back(std::stoll(line));
    }
  }

  return { ranges, values };
}

std::int64_t count_fresh_ingredients(
  std::span<const Range> fresh_ranges,
  std::span<const std::int64_t> ingredients_id
) {
  int values_in_range = 0;
  for (const std::int64_t value : ingredients_id) {

    const bool found_in_range = std::ranges::any_of(fresh_ranges,
      [value](const Range & range) {
        return range.contains(value);
      });

    if (found_in_range) {
      values_in_range++;
    }
  }

  return values_in_range;
}

std::int64_t count_total_fresh(
  std::vector<Range> fresh_ranges
) {
  while (true) {
    size_t size_before = fresh_ranges.size();

    for (auto it = fresh_ranges.begin(); it != fresh_ranges.end(); ++it) {
      auto it2 = it + 1;
      while (it2 != fresh_ranges.end()) {
        if (it->try_fuse(*it2)) {
          it2 = fresh_ranges.erase(it2);
        } else {
          ++it2;
        }
      }
    }

    size_t size_after = fresh_ranges.size();

    if (size_before == size_after) {
      break;
    }
  }

  std::int64_t total_fresh = 0;
  for (const Range & range : fresh_ranges) {
    total_fresh += (range.max - range.min + 1);
  }
  return total_fresh;
}


}

Output day_2025_05(const std::vector<std::string> & lines, const DayExtraInfo &) {
  auto [ranges, values] = boring_input_processing(lines);

  const auto part_a = count_fresh_ingredients(ranges, values);

  const auto part_b = count_total_fresh(ranges);

  return Output(part_a, part_b);
}
