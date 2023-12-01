#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <bitset>
#include <set>
#include <string>
#include <span>
#include <map>
#include <ranges>


// https://adventofcode.com/2018/day/06


bj::Rectangle get_boundaries(std::span<const bj::Position> positions) {
  if (positions.empty()) return bj::Rectangle(0, 0, 0, 0);

  bj::Rectangle bounds(
    positions[0].x,
    positions[0].y,
    positions[0].x,
    positions[0].y
  );

  for (const bj::Position & position : positions) {
    bounds.left   = std::min(bounds.left, position.x);
    bounds.top    = std::min(bounds.top, position.y);
    bounds.right  = std::max(bounds.right, position.x);
    bounds.bottom = std::max(bounds.bottom, position.y);
  }

  return bounds;
}

struct Near {
  int distance = 0;
  std::vector<bj::Position> points;

  long long int manhattan_sum = 0;

  void add(bj::Position new_point, bj::Position current_pos) {
    bj::Position diff = new_point;
    diff.x -= current_pos.x;
    diff.y -= current_pos.y;

    const int new_distance = diff.manhattan();

    manhattan_sum += new_distance;

    if (points.empty()) {
      distance = new_distance;
      points.emplace_back(new_point);
      return;
    }

    if (distance < new_distance) {
      return;
    } else if (distance > new_distance) {
      distance = new_distance;
      points.clear();
    }

    points.emplace_back(new_point);
  }
};

template<typename Range>
size_t range_count(Range && range) {
  size_t answer = 0;
  for (const auto & _ : range) {
    ++answer;
  }
  return answer;
}


Output day_2018_06(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::vector<bj::Position> points;

  for (const std::string & line : lines) {
    std::vector<int> coords = bj::extract_ints_from_line(line);
    if (coords.size() != 2) throw std::runtime_error("bad number of numbers");

    points.emplace_back(bj::Position{ coords[0], coords[1] });
  }

  // Bounds
  bj::Rectangle area = get_boundaries(points);
  --area.left;
  --area.top;
  ++area.right;
  ++area.bottom;

  // Compute influence
  std::map<bj::Position, Near> near_points;
  std::set<bj::Position> infinite_influence_points;
  std::map<bj::Position, size_t> main_points_number_of_occ;

  area.for_each_position([&](const bj::Position point_in_area) {
    for (const bj::Position & main_point : points) {
      near_points[point_in_area].add(main_point, point_in_area);
    }

    for (const bj::Position & main_point_near : near_points[point_in_area].points) {

      if (near_points[point_in_area].points.size() == 1) {
        ++main_points_number_of_occ[main_point_near];

        if (point_in_area.x == area.left
        || point_in_area.x == area.right
        || point_in_area.y == area.top
        || point_in_area.y == area.bottom
        ) {
          infinite_influence_points.emplace(main_point_near);
        }
      }
    }
  });

  for (int y = 0; y != 20; ++y) {
    for (int x = 0; x != 20; ++x) {
      bj::Position pos { x, y };
      const auto it = near_points.find(pos);
      if (it == near_points.end()) {
        std::cout << '?';
      } else {
        if (it->second.points.size() == 1) {
          std::cout << (char)('A' + (std::find(points.begin(), points.end(), it->second.points[0]) - points.begin()));
        } else{
          std::cout << '.';
        }
      }
    }
    std::cout << '\n';
  }

  std::cout << main_points_number_of_occ.size() << " " << near_points.size() << " " << infinite_influence_points.size() << " " << points.size() << " \n";


  size_t largest_area_size = 0;
  for (const auto & [main_point, size] : main_points_number_of_occ) {
    if (size > largest_area_size && !infinite_influence_points.contains(main_point)) {
      largest_area_size = size;
    }
  }

  // We assume that the borders are not that near
  size_t part_b = range_count(
    near_points
    | std::views::values
    | std::views::filter([&](const Near & near) { return near.manhattan_sum < dei.part_b_extra_param; })
  );

  return Output(largest_area_size, part_b);
}
