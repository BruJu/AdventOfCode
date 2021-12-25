#include "../advent_of_code.hpp"
#include "../util/instruction_reader.hpp"
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <optional>
#include <iostream>
#include <array>
#include <queue>

// https://adventofcode.com/2021/day/25


enum class Cucumber { None, Down, Right };


static Cucumber make_cucumber(char c) {
  if (c == '>') return Cucumber::Right;
  if (c == 'v') return Cucumber::Down;
  return Cucumber::None;
}

std::ostream & operator<<(std::ostream & stream, Cucumber c) {
  if (c == Cucumber::Down) return stream << 'v';
  else if (c == Cucumber::Right) return stream << '>';
  else return stream << '.';
}

struct Grid {
  std::vector<std::vector<Cucumber>> layout;
  size_t width;
  size_t height;

  explicit Grid(const std::vector<std::string> & lines) {
    width = lines[0].size();
    height = lines.size();

    for (size_t x = 0; x != width; ++x) {
      layout.emplace_back();

      for (size_t y = 0; y != height; ++y) {
        layout.back().emplace_back(make_cucumber(lines[y][x]));
      }
    }
  }

  Cucumber get(size_t x, size_t y) const {
    return layout[x % width][y % height];
  }

  Cucumber & get(size_t x, size_t y) {
    return layout[x % width][y % height];
  }

  bool next() {
    bool stable = east();
    stable = stable & south();
    return stable;
  }

  bool east() {
    std::vector<std::pair<size_t, size_t>> x = find_all_movable(Cucumber::Right, 1, 0);

    for (const auto cucumber_pos : x) {
      get(cucumber_pos.first, cucumber_pos.second) = Cucumber::None;
      get(cucumber_pos.first + 1, cucumber_pos.second) = Cucumber::Right;
    }

    return x.empty();
  }

  bool south() {
    std::vector<std::pair<size_t, size_t>> x = find_all_movable(Cucumber::Down, 0, 1);

    for (const auto cucumber_pos : x) {
      get(cucumber_pos.first, cucumber_pos.second) = Cucumber::None;
      get(cucumber_pos.first, cucumber_pos.second + 1) = Cucumber::Down;
    }

    return x.empty();
  }

  std::vector<std::pair<size_t, size_t>> find_all_movable(Cucumber type, size_t dx, size_t dy) {
    std::vector<std::pair<size_t, size_t>> positions;

    for (size_t x = 0; x != width; ++x) {
      for (size_t y = 0; y != height; ++y) {
        if (get(x, y) == type && get(x + dx, y + dy) == Cucumber::None) {
          positions.push_back(std::pair(x, y));
        }
      }
    }

    return positions;
  }

  friend std::ostream & operator<<(std::ostream & stream, const Grid & self) {
    for (size_t y = 0; y != self.height; ++y) {
      for (size_t x = 0; x != self.width; ++x) {
        stream << self.get(x, y);
      }
      stream << '\n';
    }
    return stream;
  }
};

Output day_2021_25(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Grid grid = Grid(lines);

  int step = 1;

  while (!grid.next()) {
    ++step;
  }

  return Output(step, 0);
}

