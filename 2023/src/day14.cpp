#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <vector>
#include <algorithm>
#include <set>
#include <map>

using i64 = std::int64_t;

using Grid = std::vector<std::string>;

// https://adventofcode.com/2023/day/14

namespace {

void tilt_boulder(Grid & grid, bj::UnsignedPosition from, bj::Direction direction) {
  bj::UnsignedPosition to = from;
  if (!to.move(direction, grid)) return;
  if (grid[from.y][from.x] != 'O') return;
  if (grid[to.y][to.x] != '.') return;

  std::swap(grid[from.y][from.x], grid[to.y][to.x]);
  tilt_boulder(grid, to, direction);
}

[[nodiscard]] i64 north_support_beams(const Grid & grid) {
  i64 answer = 0;
  for (size_t row = 0; row != grid.size(); ++row) {
    for (size_t col = 0; col != grid[0].size(); ++col) {
      if (grid[row][col] == 'O') {
        answer += (grid.size() - row);
      }
    }
  }
  return answer;
}

void tilt_NWSE(Grid & grid) {
  for (size_t row = 0; row != grid.size(); ++row) {
    for (size_t col = 0; col != grid[0].size(); ++col) {
      tilt_boulder(grid, bj::UnsignedPosition{ col, row }, bj::Direction::Top);
    }
  }
  
  for (size_t col = 0; col != grid[0].size(); ++col) {
    for (size_t row = 0; row != grid.size(); ++row) {
      tilt_boulder(grid, bj::UnsignedPosition{ col, row }, bj::Direction::Left);
    }
  }

  for (size_t row = grid.size(); row > 0; --row) {
    for (size_t col = 0; col != grid[0].size(); ++col) {
      tilt_boulder(grid, bj::UnsignedPosition{ col, row - 1 }, bj::Direction::Down);
    }
  }

  for (size_t col = grid[0].size(); col > 0; --col) {
    for (size_t row = 0; row != grid.size(); ++row) {
      tilt_boulder(grid, bj::UnsignedPosition{ col - 1, row }, bj::Direction::Right);
    }
  }
}

}

Output day_2023_14(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Grid grid = lines;

  // PART A
  for (size_t row = 0; row != grid.size(); ++row) {
    for (size_t col = 0; col != grid[0].size(); ++col) {
      tilt_boulder(grid, bj::UnsignedPosition{ col, row }, bj::Direction::Top);
    }
  }

  const i64 part_a = north_support_beams(grid);

  // PART B
  grid = lines;

  std::map<Grid, int> memory;

  int iterations = 0;
  while (iterations < 1000000000) {
    // Do normal tilt
    tilt_NWSE(grid);
    ++iterations;

    // Fast advance if in a situation that was already encountered
    const auto it = memory.find(grid);

    if (it != memory.end()) {
      // Known, fast forward
      const size_t delta = iterations - it->second;
      while (iterations + delta < 1000000000) {
        iterations += delta;
      }
    } else {
      // Unknown, register
      memory.emplace(grid, iterations);
    }
  }

  const i64 part_b = north_support_beams(grid);

  return Output(part_a, part_b);
}
