#include "../advent_of_code.hpp"
#include "../util/md5.h"

// https://adventofcode.com/2016/day/5

static bool leads_with_zeros(const std::string & s) {
  for (size_t i = 0; i != 5; ++i) {
    if (s[i] != '0') return false;
  }

  return true;
}

Output day_2016_05(const std::vector<std::string> & lines, const DayExtraInfo &) {
  MD5 md5;
  std::string part_a = "";
  std::string part_b = "$$$$$$$$";

  for (long long int i = 0; true; ++i) {
    const std::string content = lines[0] + std::to_string(i);
    const auto hash = md5(content);

    if (!leads_with_zeros(hash)) {
      continue;
    }

    const auto sixth = hash[5];

    if (part_a.size() < 8) part_a += sixth;

    if (sixth >= '0' && sixth <= '7') {
      const auto pos = sixth - '0';

      if (part_b[pos] == '$') {
        part_b[pos] = hash[6];

        if (std::find(part_b.begin(), part_b.end(), '$') == part_b.end()) {
          return Output(part_a, part_b);
        }
      }
    }
  }
}
