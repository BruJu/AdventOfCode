#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include "../util/instruction_reader.hpp"
#include <algorithm>
#include <optional>
#include <map>
#include <array>
#include <numeric>

// https://adventofcode.com/2021/day/7

// From day 4
std::vector<int> string_to_ints(const std::string & s, char separator);


Output day_2021_07(const std::vector<std::string> & lines, const DayExtraInfo &) {
  auto crabs = string_to_ints(lines[0], ',');

  long long int part_a = 100000;

  const auto double_it = std::minmax_element(crabs.begin(), crabs.end());

  const auto min = *(double_it.first);
  const auto max = *(double_it.second);

  for (auto i = min; i <= max; ++i) {

    
  }

  


//
//  const auto sum = std::reduce(crabs.begin(), crabs.end());
//  auto avg = sum / (int) crabs.size();
//  if (sum % crabs.size() > crabs.size() / 2) ++avg;
//  std::cout << avg << '\n';
//
//  long long int part_a = 0;
//  for (const auto crab : crabs) {
//    auto diff = avg - crab;
//    if (diff < 0) diff = -diff;
//    part_a += diff;
//  }

  return Output(part_a, 0);
}
