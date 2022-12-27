#include "../advent_of_code.hpp"
#include <string>

// https://adventofcode.com/2019/day/01

using Int = std::int64_t;

static int get_fuel_for(int mass) {
  return std::max((mass / 3) - 2, 0);
}

Output day_2019_01(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Int sum = 0;

  Int recursive_sum = 0;

  for (const std::string & line : lines) {
    const int mass = std::stoi(line);
    const int fuel = std::max((mass / 3) - 2, 0);
    sum += fuel;

    int rec_mass = mass;
    while (rec_mass != 0) {
      rec_mass = get_fuel_for(rec_mass);
      recursive_sum += rec_mass;
    }
  }

  return Output(sum, recursive_sum);
}
