#include "../../common/advent_of_code.hpp"
#include <algorithm>
#include <optional>
#include <numeric>

// https://adventofcode.com/2021/day/7

// At this point, string_to_ints should come with the framework
std::vector<int> string_to_ints(const std::string & s, char separator);

constexpr static long long int compute_for_b(long long int value) {
  long long int sum = 0;
  while (value != 0) {
    sum += value;
    --value;
  }
  return sum;
}

Output day_2021_07(const std::vector<std::string> & lines, const DayExtraInfo &) {
  auto crabs = string_to_ints(lines[0], ',');
  // std::sort(crabs.begin(), crabs.end()); // Not sure if sorting is efficient or not

  std::optional<long long int> part_a = std::nullopt;
  std::optional<long long int> part_b = std::nullopt;

  const auto double_it = std::minmax_element(crabs.begin(), crabs.end());

  const auto min = *(double_it.first);
  const auto max = *(double_it.second);

  // Prepare cached values for part B
  // This decrease the runtime from 482ms to 3ms
  std::vector<long long int> cache_for_b(max - min + 1);
  for (int i = 0; i <= (max - min); ++i) {
    cache_for_b[i] = compute_for_b(i);
  }

  // Ok let's go. As C++ is a fast language, and the input is
  // very small we can just bruteforce this problem.
  for (auto i = min; i <= max; ++i) {
    long long int local_answer = 0;
    long long int local_answer_b = 0;

    for (const auto crab : crabs) {
      auto diff = crab - i;
      if (diff < 0) diff = -diff;
      local_answer += diff;
      local_answer_b += cache_for_b[diff];
    }

    part_a = part_a ? std::min(*part_a, local_answer) : local_answer;
    part_b = part_b ? std::min(*part_b, local_answer_b) : local_answer_b;
  }

  return Output(part_a.value(), part_b.value());
}
