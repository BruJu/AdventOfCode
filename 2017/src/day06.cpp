#include "../../common/advent_of_code.hpp"
#include <map>

// https://adventofcode.com/2017/day/06

Output day_2017_06(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<int> values = bj::extract_ints_from_line(lines[0]);

  int steps = 0;

  std::map<std::vector<int>, int> seen;
  seen.emplace(values, steps);

  while (true) {
    size_t max = 0;
    for (size_t i = 0; i != values.size(); ++i) {
      if (values[max] < values[i]) {
        max = i;
      }
    }

    int to_give = values[max];
    values[max] = 0;
    size_t i = (max + 1) % values.size();
    while (to_give > 0) {
      values[i] += 1;
      --to_give;
      i = (i + 1) % values.size();
    }

    ++steps;

    if (seen.contains(values)) {
      break;
    }

    seen.emplace(values, steps);
  }

  return Output(steps, steps - seen[values]);
}
