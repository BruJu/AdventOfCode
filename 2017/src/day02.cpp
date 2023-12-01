#include "../../common/advent_of_code.hpp"
#include <algorithm>

// https://adventofcode.com/2017/day/02

static std::pair<int, int> get_evenly_divider(const std::vector<int> & values) {
  for (size_t i = 0; i != values.size(); ++i) {
    for (size_t j = 0; j != values.size(); ++j) {
      if (i == j) continue;

      const auto big = values[i];
      const auto small = values[j];
      if (small > big) continue;

      if (big % small == 0) return std::pair<int, int>(big, small);
    }
  }

  return std::pair<int, int>(0, 1);
}

Output day_2017_02(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  int checksum = 0;
  int heaven = 0;

  for (const std::string & line : lines) {
    std::vector<int> values_int = bj::extract_ints_from_line(line);

    const auto [min, max] = std::minmax_element(values_int.begin(), values_int.end());

    checksum += *max - *min;

    if (!dei.can_skip_part_B) {
      const auto [big_heaven, small_heaven] = get_evenly_divider(values_int);
      heaven += big_heaven / small_heaven;
    }
  }

  return Output(checksum, heaven);
}
