#include "../../common/advent_of_code.hpp"
#include <regex>

// https://adventofcode.com/2024/day/3

Output day_2024_03(const std::vector<std::string> & lines, const DayExtraInfo &) {
  long long int part_a = 0;
  long long int part_b = 0;
  
  std::regex regex { R"((mul\((-?\d+),(-?\d+)\)|do\(\)|don't\(\)))" } ;

  bool part_b_do = true;

  for (const auto & line : lines) {
    auto itMulBegin = std::sregex_iterator(line.begin(), line.end(), regex);
    auto itMulEnd = std::sregex_iterator();

    for (auto it = std::sregex_iterator(line.begin(), line.end(), regex);
      it != std::sregex_iterator();
      ++it  
    ) {
      std::smatch match = *it;
      std::string whole_instruction = match[0].str();

      if (whole_instruction == "do()") {
        part_b_do = true;
      } else if (whole_instruction == "don't()") {
        part_b_do = false;
      } else /* mul */ {
        const long long int a = std::stoll(match[2].str());
        const long long int b = std::stoll(match[3].str());
        const auto product = a * b;
        part_a += product;
        if (part_b_do) part_b += product;
      }
    }
  }

  return Output(part_a, part_b);
}
