#include "../../common/advent_of_code.hpp"
#include <string>
#include <numeric>
#include <set>

// https://adventofcode.com/2018/day/01


Output day_2018_01(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<int> changes = lines_transform::to_ints(lines);

  const auto sum = std::reduce(changes.begin(), changes.end());

  std::set<int> reached;

  int current_frequency = 0;
  reached.emplace(current_frequency);
  size_t i = 0;
  while (true) {
    current_frequency += changes[i];
    if (reached.contains(current_frequency)) {
      break;
    }
    reached.emplace(current_frequency);

    i = (i + 1) % changes.size();
  }

  return Output(sum, current_frequency);
}
