#include "../../common/advent_of_code.hpp"
#include <vector>
#include <algorithm>
#include <set>

using i64 = std::int64_t;

// https://adventofcode.com/2023/day/11


namespace {

using Grid = std::vector<std::string>;
using Galaxy = std::pair<size_t, size_t>;

class Universe {
public:
  Grid _grid;
  std::set<size_t> _empty_lines;
  std::set<size_t> _empty_columns;

public:
  explicit Universe(Grid lines) : _grid(lines) {
    for (size_t y = 0; y != _grid.size(); ++y) {
      if (std::ranges::all_of(_grid[y], [](char c) { return c == '.'; })) {
        _empty_columns.emplace(y);
      }
    }

    for (size_t x = 0; x != _grid[0].size(); ++x) {
      if (is_empty_column(_grid, x)) {
        _empty_lines.emplace(x);
      }
    }
  }

  
  [[nodiscard]] std::vector<Galaxy> find_galaxies() const {
    std::vector<Galaxy> retval;

    for (size_t y = 0; y != _grid.size(); ++y) {
      for (size_t x = 0; x != _grid[0].size(); ++x) {
        if (_grid[y][x] != '.') {
          retval.emplace_back(x, y);
        }
      }
    }

    return retval;
  }

  

  [[nodiscard]] i64 compute_distance(Galaxy star1, Galaxy star2, i64 cosmic_expansion) const {
    const auto [min_x, max_x] = std::minmax({ star1.first, star2.first });
    const auto [min_y, max_y] = std::minmax({ star1.second, star2.second });

    i64 distance = 0;

    for (size_t x = min_x; x < max_x; ++x) {
      if (_empty_lines.contains(x)) {
        distance += cosmic_expansion;
      } else {
        distance += 1;
      }
    }

    for (size_t y = min_y; y < max_y; ++y) {
      if (_empty_columns.contains(y)) {
        distance += cosmic_expansion;
      } else {
        distance += 1;
      }
    }

    return distance;
  }


private:
  [[nodiscard]] static bool is_empty_column(const Grid & grid, size_t x) {
    for (size_t y = 0; y != grid.size(); ++y) {
      if (grid[y][x] != '.') {
        return false;
      }
    }

    return true;
  }
};

}


Output day_2023_11(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  Universe universe(lines);

  const std::vector<Galaxy> galaxies = universe.find_galaxies();

  const i64 cosmic_expansion = dei.part_b_extra_param == 0 ? 1000000 : dei.part_b_extra_param;

  i64 part_a = 0;
  i64 part_b = 0;

  for (size_t i = 0; i < galaxies.size(); ++i) {
    for (size_t j = i + 1; j < galaxies.size(); ++j) {
      part_a += universe.compute_distance(galaxies[i], galaxies[j], 2);
      part_b += universe.compute_distance(galaxies[i], galaxies[j], cosmic_expansion);
    }
  }

  return Output(part_a, part_b);
}
