#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include "../../util/instruction_reader.hpp"
#include <algorithm>
#include <optional>
#include <map>
#include <array>
#include <numeric>

// https://adventofcode.com/2021/day/6


// From day 4
std::vector<int> string_to_ints(const std::string & s, char separator);


[[maybe_unused]] static long long int simulation(std::vector<int> lanterns, int until) {
    for (int d = 0 ; d < until; ++d) {
    int add_how_many = 0;

    for (auto & lanternfish : lanterns) {
      if (lanternfish == 0) {
        lanternfish = 6;
        ++add_how_many;
      } else {
        --lanternfish;
      }
    }

    while (add_how_many != 0) {
      lanterns.push_back(8);
      --add_how_many;
    }
  }

  return lanterns.size();
}


static long long int smart_way(const std::vector<int> & init, int until) {
  // It's probably easier and good enough to go for a middly stupid solution
  // so I did a middly stupid solution instead

  std::vector<long long int> lanterns; // lanterns[x] -> nb of lanterns with a counter of x

  for (const auto & lantern : init) {
    while (lanterns.size() <= size_t(lantern)) {
      lanterns.push_back(0);
    }

    ++lanterns[lantern];
  }

  while (lanterns.size() <= 8) {
    lanterns.push_back(0);
  }

  for (int day = 0; day < until; ++day) {
    const auto x = lanterns[0];
    lanterns.erase(lanterns.begin());
    lanterns.push_back(0);
    lanterns[6] += x;
    lanterns[8] += x;
  }

  return std::reduce(lanterns.begin(), lanterns.end());
}

Output day_2021_06(const std::vector<std::string> & lines, const DayExtraInfo &) {
  auto lanterns = string_to_ints(lines[0], ',');

  // const auto part_a = simulation(lanterns, std::stoi(lines[1]));
  const auto part_a = smart_way(lanterns, std::stoi(lines[1]));
  const auto part_b = smart_way(lanterns, 256);

  return Output(part_a, part_b);
}
