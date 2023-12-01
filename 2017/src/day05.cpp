#include "../../common/advent_of_code.hpp"

// https://adventofcode.com/2017/day/05

static int part_a(std::vector<int> values) {
  int pc = 0;
  int steps = 0;

  while (pc >= 0 && std::cmp_less(pc, values.size())) {
    int & jump = values[pc];
    pc += jump;
    ++jump;
    ++steps;
  }

  return steps;
}

static int part_b(std::vector<int> values) {
  int pc = 0;
  int steps = 0;

  while (pc >= 0 && std::cmp_less(pc, values.size())) {
    int & jump = values[pc];
    pc += jump;
    if (jump >= 3) --jump;
    else ++jump;
    ++steps;
  }

  return steps;
}

Output day_2017_05(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::vector<int> values = lines_transform::to_ints(lines);

  return Output(part_a(values), part_b(values));
}
