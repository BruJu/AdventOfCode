#include "../advent_of_code.hpp"

// https://adventofcode.com/2017/day/01

Output day_2017_01(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::string & line = lines[0];

  size_t part_a = 0;
  for (size_t i = 0; i != line.size(); ++i) {
    if (line[i] == line[(i + 1) % line.size()]) {
      part_a += (line[i] - '0');
    }
  }

  size_t part_b = 0;
  for (size_t i = 0; i != line.size(); ++i) {
    if (line[i] == line[(i + line.size() / 2) % line.size()]) {
      part_b += (line[i] - '0');
    }
  }

  return Output(part_a, part_b);
}
