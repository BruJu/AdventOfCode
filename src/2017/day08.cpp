#include "../advent_of_code.hpp"
#include <map>
#include <string>

// https://adventofcode.com/2017/day/08

bool is_true(
  std::map<std::string, int> & registers,
  std::string checked_register,
  std::string operation,
  int wanted_value
  ) {
  const int value = registers[checked_register];

  if (operation == "==") return value == wanted_value;
  if (operation == "!=") return value != wanted_value;
  if (operation == "<") return value < wanted_value;
  if (operation == ">") return value > wanted_value;
  if (operation == "<=") return value <= wanted_value;
  if (operation == ">=") return value >= wanted_value;
  return false;
}


Output day_2017_08(const std::vector<std::string> & lines, const DayExtraInfo &) {
  int max_ever_held = 0;
  std::map<std::string, int> registers;
  for (const std::string & line : lines) {
    const std::vector<std::string> tokens = bj::string_split(line, " ");

    if (is_true(registers, tokens[4], tokens[5], std::stoi(tokens[6]))) {
      int modification = std::stoi(tokens[2]);
      if (tokens[1] == "dec") modification = -modification;

      registers[tokens[0]] += modification;
      max_ever_held = std::max(max_ever_held, registers[tokens[0]]);
    }
  }

  int m = registers.begin()->second;

  for (const auto & [_, value] : registers) {
    m = std::max(m, value);
  }

  return Output(m, max_ever_held);
}
