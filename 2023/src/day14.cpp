#include "../../common/advent_of_code.hpp"
#include <vector>
#include <algorithm>
#include <set>
#include <map>

using i64 = std::int64_t;

using Grid = std::vector<std::string>;

// https://adventofcode.com/2023/day/14


void move_up(size_t row, size_t col, Grid & g) {
  if (row == 0) return;
  if (g[row][col] != 'O') return;

  if (g[row - 1][col] == '.') {
    g[row - 1][col] = 'O';
    g[row][col] = '.';
    move_up(row - 1, col, g);
  }
}


void move_down(size_t row, size_t col, Grid & g) {
  if (row + 1 == g.size()) return;
  if (g[row][col] != 'O') return;

  if (g[row + 1][col] == '.') {
    g[row + 1][col] = 'O';
    g[row][col] = '.';
    move_down(row + 1, col, g);
  }
}

void move_left(size_t row, size_t col, Grid & g) {
  if (col == 0) return;
  if (g[row][col] != 'O') return;

  if (g[row][col - 1] == '.') {
    g[row][col - 1] = 'O';
    g[row][col] = '.';
    move_left(row, col - 1, g);
  }
}

void move_right(size_t row, size_t col, Grid & g) {
  if (col  + 1 == g[0].size()) return;
  if (g[row][col] != 'O') return;

  if (g[row][col + 1] == '.') {
    g[row][col + 1] = 'O';
    g[row][col] = '.';
    move_right(row, col + 1, g);
  }
}


Output day_2023_14(const std::vector<std::string> & lines, const DayExtraInfo &) {
  i64 part_a = 0;
  i64 part_b = 0;

  Grid grid = lines;

  for (size_t row = 0; row != grid.size(); ++row) {
    for (size_t col = 0; col != grid[0].size(); ++col) {
      move_up(row, col, grid);
    }
  }

  for (size_t row = 0; row != grid.size(); ++row) {
    for (size_t col = 0; col != grid[0].size(); ++col) {
      if (grid[row][col] == 'O') {
        part_a += (grid.size() - row);
      }
    }
  }

  grid = lines;

  std::map<Grid, size_t> memory;

  for (size_t i = 0; i != 1000000000; ++i) {
    for (size_t row = 0; row != grid.size(); ++row) {
      for (size_t col = 0; col != grid[0].size(); ++col) {
        move_up(row, col, grid);
      }
    }

    
    for (size_t col = 0; col != grid[0].size(); ++col) {
      for (size_t row = 0; row != grid.size(); ++row) {
        move_left(row, col, grid);
      }
    }

    for (int row = grid.size() - 1; row >= 0; --row) {
      for (size_t col = 0; col != grid[0].size(); ++col) {
        move_down(row, col, grid);
      }
    }

    for (int col = grid[0].size() - 1; col >= 0; --col) {
      for (size_t row = 0; row != grid.size(); ++row) {
        move_right(row, col, grid);
      }
    }

    auto it = memory.find(grid);

    if (it != memory.end()) {
      size_t delta = i - it->second;

      while (i + delta - 1 < 1000000000) {
        i += delta;
      }

    } else {
      memory.emplace(grid, i);
    }
  }


  for (size_t row = 0; row != grid.size(); ++row) {
    for (size_t col = 0; col != grid[0].size(); ++col) {
      if (grid[row][col] == 'O') {
        part_b += (grid.size() - row);
      }
    }
  }

  
  return Output(part_a, part_b);
}
