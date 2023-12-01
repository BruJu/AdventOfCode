#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <set>
#include <map>
#include <ranges>

// https://adventofcode.com/2018/day/03

Output day_2018_03(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::map<bj::Position, std::set<int>> claimed_positions;

  std::set<int> all_ids;
  for (const std::string & line : lines) {
    std::vector<int> ints = bj::extract_ints_from_line(line);
    const int id = ints[0];
    const int left = ints[1];
    const int top = ints[2];
    const int right = left + ints[3] - 1;
    const int down = top + ints[4] - 1;

    bj::Rectangle rectangle(left, top, right, down);

    rectangle.for_each_position([&](const bj::Position & pos) {
      claimed_positions[pos].emplace(id);
    });

    all_ids.emplace(id);
  }

  std::set<int> in_multiples;
  size_t multiply_claimed = 0;
  for (const auto & claimed_by : claimed_positions | std::views::values) {
    if (claimed_by.size() > 1) {
      ++multiply_claimed;

      for (const auto id : claimed_by) {
        in_multiples.emplace(id);
      }
    }
  }

  int untouched_claim = 0;
  for (const auto id : all_ids) {
    if (!in_multiples.contains(id)) {
      untouched_claim = id;
      break;
    }
  }

  return Output(multiply_claimed, untouched_claim);
}
