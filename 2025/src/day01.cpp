#include "../../common/advent_of_code.hpp"
#include <vector>
#include <span>
#include <utility>
#include <regex>
#include <ranges>
#include <algorithm>
#include <map>

// https://adventofcode.com/2025/day/1

Output day_2025_01(const std::vector<std::string> & lines, const DayExtraInfo &) {
  int dial = 50;

  int stopped_at_0 = 0;
  int passed_by_0 = 0;

  for (const std::string & line : lines) {
    if (line.empty()) break;

    int raw_value = std::stoi(line.substr(1));
    if (line[0] == 'L') {
      raw_value = -raw_value;
    }

    bool isOn0 = dial == 0;

    dial += raw_value;

    while (dial < 0) {
      if (!isOn0) {
        passed_by_0++;
      } else {
        isOn0 = false;
      }

      dial += 100;
    }
    while (dial > 100) {
      dial -= 100;
      passed_by_0++;
    }

    if (dial == 100) {
      dial -= 100;
    }

    if (dial == 0) {
      ++stopped_at_0;
    }
  }

  return Output(stopped_at_0, stopped_at_0 + passed_by_0);
}
