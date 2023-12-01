#include "../../common/advent_of_code.hpp"
#include <algorithm>
#include <optional>
#include <map>
#include <vector>
#include <set>
#include "../../util/position.hpp"

// https://adventofcode.com/2021/day/9


long long int bassin_size(const std::vector<std::string> & lines, int col, int row) {
  const auto get = [&](int r, int c) -> int {
    if (r < 0) return 9;
    if (c < 0) return 9;
    if (r >= int(lines.size())) return 9;
    if (c >= int(lines[0].size())) return 9;
    return lines[r][c] - '0';
  };

  std::set<bj::Position> positions;

  positions.emplace(bj::Position{ int(row), int(col) });

  std::map<int, std::vector<bj::Position>> priority_queue;
  priority_queue[get(row, col)].push_back(bj::Position{ col, row });

  for (int height = 0; height < 9; ++height) {
    auto & stack = priority_queue[height];

    while (!stack.empty()) {
      const auto pos = stack.back();
      stack.pop_back();

      pos.for_each_neighbour([&](bj::Position neighbour) {
        const auto new_height = get(neighbour.y, neighbour.x);
        if (height >= new_height) return;
        if (new_height == 9) return;
        if (positions.find(neighbour) != positions.end()) return;

        positions.emplace(neighbour);
        priority_queue[new_height].push_back(neighbour);
      });
    }
  }

  return positions.size();
}

Output day_2021_09(const std::vector<std::string> & lines, const DayExtraInfo &) {
  long long int part_a = 0;

  std::vector<long long int> bassins;

  for (size_t y = 0; y != lines.size(); ++y) {
    const auto & line = lines[y];
    for (size_t x = 0; x != line.size(); ++x) {
      if (x > 0 && (lines[y][x] >= lines[y][x - 1])) continue;
      if (x < line.size() - 1 && (lines[y][x] >= lines[y][x + 1])) continue;
      if (y > 0 && (lines[y][x] >= lines[y - 1][x])) continue;
      if (y < lines.size() - 1 && (lines[y][x] >= lines[y + 1][x])) continue;

      part_a += (lines[y][x] - '0') + 1;

      bassins.push_back(bassin_size(lines, x, y));
    }
  }

  std::sort(bassins.rbegin(), bassins.rend());

  // Done
  return Output(part_a, bassins[0] * bassins[1] * bassins[2]);
}
