#include "../../common/advent_of_code.hpp"
#include "../../util/regex_helper.hpp"
#include <vector>
#include <set>
#include <ranges>

// https://adventofcode.com/2022/day/4
// 
// A list of pairs of intervals:
// 2-4,3-7
//
// (A) Count the number of pairs with contained intervals
// (B) Count the number of pairs with overlapping intervals

struct Range {
  int left;
  int right;

  [[nodiscard]] bool is_contained_by(const Range & other) const noexcept {
    return other.left <= left && other.right >= right;
  }

  [[nodiscard]] bool is_fully_on_the_left_of(const Range & other) const noexcept {
    return right < other.left;
  }
};

struct TwoRanges {
  static constexpr const char * Regex_Pattern = R"(([0-9]*)-([0-9]*),([0-9]*)-([0-9]*))";

  Range first;
  Range second;

  explicit TwoRanges(std::vector<std::string> values) {
    first = Range{ std::stoi(values[0]), std::stoi(values[1]) };
    second = Range{ std::stoi(values[2]), std::stoi(values[3]) };
  };
 
  [[nodiscard]] bool has_contained() const noexcept {
    return first.is_contained_by(second) || second.is_contained_by(first);
  }

  [[nodiscard]] bool has_overlap() const noexcept {
    return !(first.is_fully_on_the_left_of(second) || second.is_fully_on_the_left_of(first));
  }
};

Output day_2022_04(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::vector<TwoRanges> ranges = bj::lines_to_class_by_regex<TwoRanges>(lines);

  const long long int part_a = std::ranges::distance(ranges | std::views::filter([](const TwoRanges & rng) { return rng.has_contained(); }));
  const long long int part_b = std::ranges::distance(ranges | std::views::filter([](const TwoRanges & rng) { return rng.has_overlap(); }));

  return Output(part_a, part_b);
}
