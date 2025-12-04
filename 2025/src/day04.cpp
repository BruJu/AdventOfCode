#include "../../common/advent_of_code.hpp"
#include "./2025.hpp"
#include <vector>
#include <span>
#include <utility>
#include <regex>
#include <ranges>
#include <algorithm>
#include <map>
#include "../../util/position.hpp"

// https://adventofcode.com/2025/day/4

namespace {

std::pair<std::vector<std::string>, int>
forklifting(const std::vector<std::string> & grid) {
  int forklifts = 0;

  std::vector<std::string> cpy = grid;

  for (int y = 0; std::cmp_not_equal(y, grid.size()); ++y) {
    for (int x = 0; std::cmp_not_equal(x, grid[y].size()); ++x) {
      bj::Position pos { x, y };

      if (grid[y][x] != '@') continue; // Not on a roll

      int around_roll = 0;

      for (bj::Position neighbour : pos.get_8_neighbours()) {
        if (neighbour.is_out_of_bound(grid)) continue;
        if (grid[neighbour.y][neighbour.x] == '@') {
          around_roll++;
        }
      }

      if (around_roll < 4) {
        ++forklifts;
        cpy[y][x] = '.';
      }
    }
  }

  return { cpy, forklifts };
}

}

Output day_2025_04(const std::vector<std::string> & lines, const DayExtraInfo &) {
  auto [grid, forklifts] = forklifting(lines);
  int total_forklifts = forklifts;

  while (true) {
    auto [new_grid, new_forklifts] = forklifting(grid);
    if (new_forklifts == 0) break;
    total_forklifts += new_forklifts;
    grid = std::move(new_grid);
  }
  
  return Output(forklifts, total_forklifts);
}
