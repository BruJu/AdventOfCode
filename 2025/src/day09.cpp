#include "../../common/advent_of_code.hpp"
#include <vector>
#include <algorithm>
#include <numeric>
#include "../../util/position.hpp"
#include <set>
#include <map>
#include <stack>
#include <climits>

// https://adventofcode.com/2025/day/9

namespace {

std::int64_t rect_area(bj::Position a, bj::Position b) {
  std::int64_t x_min = std::min(a.x, b.x);
  std::int64_t x_max = std::max(a.x, b.x);
  std::int64_t y_min = std::min(a.y, b.y);
  std::int64_t y_max = std::max(a.y, b.y);

  return (y_max - y_min + 1) * (x_max - x_min + 1);
}

struct VirtualCoordinate {
  int start;
  int end;
};

struct VirtualAxis {
  std::map<int, int> from_normal_to_virtual;
  std::vector<VirtualCoordinate> from_virtual_to_normal;

  explicit VirtualAxis(const std::set<int> & values) {
    int next_expected_real_coordinate = INT_MIN;

    for (const int real_coordinate : values) {
      if (real_coordinate != next_expected_real_coordinate) {
        // add padding
        from_virtual_to_normal.emplace_back(VirtualCoordinate{ next_expected_real_coordinate, real_coordinate - 1 });
      }

      // Add this
      from_normal_to_virtual[real_coordinate] = from_virtual_to_normal.size();
      from_virtual_to_normal.emplace_back(VirtualCoordinate{ real_coordinate, real_coordinate });

      next_expected_real_coordinate = real_coordinate + 1;
    }

    // final padding
    from_virtual_to_normal.emplace_back(VirtualCoordinate{ next_expected_real_coordinate, INT_MAX });
  }

  [[nodiscard]] int get(int normal_coordinate) const {
    const auto it = from_normal_to_virtual.find(normal_coordinate);
    if (it == from_normal_to_virtual.end()) throw std::runtime_error("202509 Virtual Axis: unexpected real coordinate");
    return it->second;
  }

  int length(int start, int end) {
    if (end >= from_virtual_to_normal.size()) throw std::runtime_error("202509 Virtual Axis: length from out of bound");
    if (start == 0) throw std::runtime_error("202509 Virtual Axis : length from infinite left");
    if (end + 1 == from_virtual_to_normal.size()) throw std::runtime_error("202509 Virtual Axis : length from infinite right");
    
    if (end < start) std::swap(start, end);

    int l = 0;
    for (int x = start; x <= end; ++x) {
      l += from_virtual_to_normal[x].end - from_virtual_to_normal[x].start + 1;
    }
    return l;
  }

  int get_upper_bound() { return from_virtual_to_normal.size(); }

};

struct VirtualAxisBuilder {
  std::set<int> coord;

  void add(int val) {
    coord.emplace(val);
  }

  VirtualAxis build() {
    return VirtualAxis(coord);
  }
};

template<typename Item, typename Accessor>
VirtualAxis build_virtual_axis(const std::vector<Item> & items, Accessor accessor) {
  VirtualAxisBuilder builder;

  for (const Item & item : items) {
    builder.add(accessor(item));
  }

  return builder.build();
}


std::vector<std::string> draw_filled(std::span<const bj::Position> points) {
  constexpr auto construct_zoomed_grid = [](int width, int height) -> std::vector<std::string> {
    std::vector<std::string> drawn_grid;
    std::string base_line = "";
    for (int x = 0; x != width; ++x) {
      base_line += "??";
    }

    for (int y = 0; y != height; ++y) {
      drawn_grid.emplace_back(base_line);
      drawn_grid.emplace_back(base_line);
    }
    return drawn_grid;
  };

  constexpr auto find_bound = [](std::span<const bj::Position> points) -> bj::Position {
    bj::Position max { 0, 0 };

    for (const bj::Position & point : points) {
      if (point.x <= 0) throw std::runtime_error("draw_filled can not draw if points are <= 0. consider rebasing");
      if (point.y <= 0) throw std::runtime_error("draw_filled can not draw if points are <= 0. consider rebasing");
      max.x = std::max(point.x + 2, max.x);
      max.y = std::max(point.y + 2, max.y);
    }

    return max;
  };


  constexpr auto draw_bound = [](std::vector<std::string> & grid, std::span<const bj::Position> points) {
    for (size_t i = 0; i != points.size(); ++i) {
      bj::Position from = points[i];
      bj::Position to = points[(i + 1) % points.size()];

      if (from.x != to.x && from.y != to.y) throw std::runtime_error("not aligned");

      int minx = std::min(from.x, to.x) * 2;
      int miny = std::min(from.y, to.y) * 2;
      int maxx = std::max(from.x, to.x) * 2;
      int maxy = std::max(from.y, to.y) * 2;

      for (int j = miny; j <= maxy; ++j) {
        for (int i = minx; i <= maxx; ++i) {
          grid[j][i] = '+';
        }
      }
    }
  };

  constexpr auto fill_outside = [](std::vector<std::string> & grid) {
    std::stack<bj::Position> todo;

    todo.push(bj::Position{ 0, 0 });

    while (!todo.empty()) {
      bj::Position pos = todo.top();
      todo.pop();

      // OOB?
      if (pos.x < 0 || pos.y < 0) continue;
      if (pos.x >= grid[0].size() || pos.y >= grid.size()) continue;

      // Change it?
      char & c = grid[pos.y][pos.x];

      if (c != '?') {
        continue;
      }

      c = '.';
      
      // explore neighbour
      todo.push(pos + bj::Position{  0, +1 });
      todo.push(pos + bj::Position{  0, -1 });
      todo.push(pos + bj::Position{ +1,  0 });
      todo.push(pos + bj::Position{ -1,  0 });
    }
  };

  constexpr auto replace_question_marks = [](std::vector<std::string> & grid) {
    for (std::string & line : grid) {
      for (char & c : line) {
        if (c == '?') {
          c = 'x';
        }
      }
    }
  };

  constexpr auto unzoom = [](const std::vector<std::string> & zoomed) {
    std::vector<std::string> unzoomed;
    for (size_t row = 0; row < zoomed.size(); row += 2) {
      unzoomed.emplace_back();
      for (size_t col = 0; col < zoomed[row].size(); col +=2 ) {
        unzoomed.back() += zoomed[row][col];
      }
    }
    return unzoomed;
  };

  const auto [width, height] = find_bound(points);
  std::vector<std::string> zoomed = construct_zoomed_grid(width, height);
  draw_bound(zoomed, points);
  fill_outside(zoomed);
  replace_question_marks(zoomed);
  return unzoom(zoomed);
}

std::pair<bj::Position, bj::Position> reorder_coord(bj::Position pa, bj::Position ch) {
  bj::Position top_left { std::min(pa.x, ch.x), std::min(pa.y, ch.y) };
  bj::Position bottom_right { std::max(pa.x, ch.x), std::max(pa.y, ch.y) };
  return { top_left, bottom_right };
}

bool check_rect(const std::vector<std::string> & grid, bj::Position pa, bj::Position ch) {
  const auto [top_left, bottom_right] = reorder_coord(pa, ch);

  for (int y = top_left.y; y <= bottom_right.y; ++y) {
    for (int x = top_left.x; x <= bottom_right.x; ++x) {
      if (grid[y][x] == '.') return false;
    }
  }
  
  return true;
}

}

Output day_2025_09(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::vector<bj::Position> points;
  for (const std::string & line : lines) {
    std::vector<int> coords = bj::extract_ints_from_line(line);
    points.emplace_back(bj::Position{ coords[0], coords[1] });
  }


  std::int64_t part_a = 0, part_b = 0;
  for (const bj::Position & point_a : points) {
    for (const bj::Position & point_b : points) {
      part_a = std::max(part_a, rect_area(point_a, point_b));
    }
  }

  // Part B: requires space compression
  VirtualAxis virtual_x = build_virtual_axis(points, [](const bj::Position & point) { return point.x; });
  VirtualAxis virtual_y = build_virtual_axis(points, [](const bj::Position & point) { return point.y; });

  std::vector<bj::Position> virtual_points;
  for (const bj::Position & real_point : points) {
    virtual_points.emplace_back(bj::Position{
      virtual_x.get(real_point.x),
      virtual_y.get(real_point.y)
    });
  }

  constexpr auto construct_grid = [](int width, int height) -> std::vector<std::string> {
    std::vector<std::string> drawn_grid;
    std::string base_line = "";
    for (int x = 0; x != width; ++x) {
      base_line += '.';
    }

    for (int y = 0; y != height; ++y) {
      drawn_grid.emplace_back(base_line);      
    }
    return drawn_grid;
  };

  std::vector<std::string> grid = draw_filled(virtual_points);

  for (const bj::Position & point_a : virtual_points) {
    for (const bj::Position & point_b : virtual_points) {
      if (!check_rect(grid, point_a, point_b)) continue;

      part_b = std::max(part_b, 
        static_cast<std::int64_t>( virtual_x.length(point_a.x, point_b.x) )
        * static_cast<std::int64_t>( virtual_y.length(point_a.y, point_b.y) )
      );
    }
  }

  // 1288306560 = too low

  // Return result
  return Output(part_a, part_b);
}
