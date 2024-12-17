#include "../../common/advent_of_code.hpp"
#include <list>
#include <ranges>
#include <algorithm>
#include <map>

// https://adventofcode.com/2024/day/11

namespace {

/**
 * Blink the stone according to rules:
 * - 0 -> 1
 * - a number with an even number of digits -> split the number in half
 * - else *2024
 */
std::pair<int64_t, std::optional<int64_t>> blink_stone(int64_t stone) {
  if (stone == 0) return { 1, std::nullopt };

  if (std::string s = std::to_string(stone); s.size() % 2 == 0) {
    return {
      std::stoll(s.substr(0, s.size() / 2)),
      std::stoll(s.substr(s.size() / 2))
    };
  }

  return { stone * 2024, std::nullopt };
}

// PART A

/**
 * Blink all stones once
 */
void blink_stone_list_once(std::list<int64_t> & list) {
  auto it_stone = list.begin();
  while (it_stone != list.end()) {
    const auto [a, opt_b] = blink_stone(*it_stone);

    *it_stone = a;
    ++it_stone;

    if (opt_b) {
      list.insert(it_stone, *opt_b);
    }
  }
}

// PART B - first implemented method with recursion + memoization

/**
 * @brief Blink the stone n times and return the number of stones.
 * Stores the result in blink_map
 * 
 * @param stone The stone
 * @param n n times
 * @param blink_map A mapping (stone number, number of times) -> number of stones
 * @return size_t The number of produces stones
 */
[[maybe_unused]]
size_t blink_stone_nth_times(
  int64_t stone,
  size_t n,
  std::map<std::pair<int64_t, size_t>, size_t> & blink_map
) {
  if (n == 0) return 1;

  const auto it = blink_map.find(std::pair(stone, n));
  if (it != blink_map.end()) {
    return it->second;
  }

  size_t output = 0;

  const auto [a, opt_b] = blink_stone(stone);

  output += blink_stone_nth_times(a, n - 1, blink_map);
  if (opt_b) output += blink_stone_nth_times(*opt_b, n - 1, blink_map);

  blink_map[std::pair(stone, n)] = output;

  return output;
}

/**
 * Blink the stones n times and return the number of stones at the end
 */
[[maybe_unused]]
size_t blink_nth_times(std::span<const int64_t> stones, size_t n) {
  std::map<std::pair<int64_t, size_t>, size_t> blink_map;

  size_t total = 0;
  for (int64_t stone : stones) {
    total += blink_stone_nth_times(stone, n, blink_map);
  }

  return total;
}

// PART B - second implemented method with a map

size_t blink_nth_times_better(std::span<const int64_t> stones, size_t n) {
  // Order is not important, store the stones in a map (stone id) -> number
  std::map<int64_t, size_t> stones_map;
  for (const int64_t stone : stones) {
    stones_map[stone] += 1;
  }

  // Do the blink
  for (size_t i = 0; i != n; ++i) {
    std::map<int64_t, size_t> next;

    for (const auto & [value, qtt] : stones_map) {
      const auto [a, opt_b] = blink_stone(value);

      next[a] += qtt;
      if (opt_b) next[*opt_b] += qtt;
    }

    stones_map = std::move(next);
  }

  // return stones_map | std::views::values | std::views::reduce() ???????

  return std::ranges::fold_left_first(
    stones_map | std::views::values,
    std::plus{}
  ).value_or(0);
}


}

Output day_2024_11(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::vector<int64_t> stones_ = bj::extract_signed_i64_from_line(lines[0]);

  std::list<int64_t> stones(stones_.begin(), stones_.end());

  for (int i = 0; i != 25; ++i) {
    blink_stone_list_once(stones);
  }

  size_t part_a = stones.size();
  size_t part_b = 0;
  if (!dei.can_skip_part_B) {
    part_b = blink_nth_times_better(stones_, 75);
  }

  return Output(part_a, part_b); 
}
