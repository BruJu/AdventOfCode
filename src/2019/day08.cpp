#include "../advent_of_code.hpp"
#include <string>
#include <array>

// https://adventofcode.com/2019/day/08

Output day_2019_08(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::string & line = lines[0];

  constexpr size_t wide = 25;
  constexpr size_t tall = 6;
  size_t number_of_layers = line.size() / (wide * tall);

  // Part A
  size_t lowest_0 = wide * tall + 1;
  size_t part_a_result = 0;

  for (size_t i_layer = 0; i_layer != number_of_layers; ++i_layer) {
    std::array<size_t, 3> digits { 0, 0, 0 };

    for (size_t i = 0; i != wide * tall; ++i) {
      size_t I = i_layer * (wide * tall) + i;

      const char c = line[I];
      ++digits[c - '0'];
    }

    if (digits[0] < lowest_0) {
      lowest_0 = digits[0];
      part_a_result = digits[1] * digits[2];
    }
  }

  // Part B
  for (int y = 0; y != tall; ++y) {
    for (int x = 0; x != wide; ++x) {
      size_t i_layer;
      for (i_layer = 0; i_layer != number_of_layers; ++i_layer) {
        size_t I = i_layer * (wide * tall) + y * wide + x;
        const char c = line[I];
        if (c == '0') {
          std::cout << ' ';
          break;
        } else if (c == '1') {
          std::cout << '#';
          break;
        }
      }

      if (i_layer == number_of_layers) {
        std::cout << '?';
      }
    }
    std::cout << "\n";
  }
  
  return Output(part_a_result, "-");
}

