#include "../../common/advent_of_code.hpp"
#include <vector>
#include <utility>
#include <map>

// https://adventofcode.com/2023/day/3

using Input = const std::vector<std::string> &;

struct PartNumber {
  long long int value;
  size_t xleft;
  size_t xright;
};

static
std::optional<PartNumber>
getPartNumber(Input lines, size_t x, size_t y) {
  char c = lines[y][x];
  if (c < '0' || c > '9') return std::nullopt;

  size_t xright = x;
  while (xright < lines[y].size() && lines[y][xright] >= '0' && lines[y][xright] <= '9') {
    ++xright;
  }

  long long int raw_value = std::stoi(lines[y].substr(x, xright - x));

  --xright;

  return PartNumber { raw_value, x, xright };
}

struct SymbolSearcher {
  bool foundSymbol = false;
  std::vector<std::pair<size_t, size_t>> foundTimes;

  void operator()(Input lines, size_t x, size_t y) {
    char c = lines[y][x];
    if (c == '*') {
      foundTimes.emplace_back(x, y);
    }
    foundSymbol = foundSymbol || c != '.';
  }
};

static
SymbolSearcher
isSurrounded(Input lines, size_t xleft, size_t xright, size_t y) {
  SymbolSearcher ss;
  
  for (int dy : { -1, 0, 1 }) {
    if (dy == -1 && y == 0) continue;
    if (dy == 1 && y + 1 == lines.size()) continue;

    if (xleft != 0) {
      ss(lines, xleft - 1, y + dy);
    }

    if (dy != 0) {
      for (size_t x = xleft; x <= xright; ++x) {
        ss(lines, x, y + dy);
      }
    }

    if (xright + 1 != lines[0].size()) {
      ss(lines, xright + 1, y + dy);
    }
  }

  return ss;
}

Output day_2023_03(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const size_t ymax = lines.size();
  const size_t xmax = lines[0].size();

  int sum_of_part_numbers = 0;

  std::map<std::pair<size_t, size_t>, std::vector<long long int>> gears_to_near_part_numbers;

  for (size_t y = 0; y != ymax; ++y) {
    for (size_t x = 0; x != xmax; ++x) {
      std::optional<PartNumber> number = getPartNumber(lines, x, y);
      if (!number) continue;

      x = number->xright;

      const auto surrounded_result = isSurrounded(lines, number->xleft, number->xright, y);
      if (!surrounded_result.foundSymbol) continue;

      sum_of_part_numbers += number->value;

      for (const auto & gear_pos : surrounded_result.foundTimes) {
        gears_to_near_part_numbers[gear_pos].emplace_back(number->value);
      }
    }
  }

  long long int gear_ratio = 0;
  for (const auto & [gearPos, part_values] : gears_to_near_part_numbers) {
    if (part_values.size() == 2) {
      gear_ratio += part_values[0] * part_values[1];
    }
  }

  return Output(sum_of_part_numbers, gear_ratio);
}
