#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <map>
#include <exception>
#include <bitset>
#include <set>

// https://adventofcode.com/2016/day/24

namespace {

struct Grid {
  std::vector<std::string> m_grid;

  explicit Grid(const std::vector<std::string> & lines) : m_grid(lines) {}

  [[nodiscard]] std::map<int, bj::Position> get_positions() const;
  [[nodiscard]] size_t get_distance(bj::Position from, bj::Position to) const;
};


std::map<int, bj::Position> Grid::get_positions() const {
  std::map<int, bj::Position> positions;

  for (size_t y = 0; y != m_grid.size(); ++y) {
    for (size_t x = 0; x != m_grid[0].size(); ++x) {
      const char c = m_grid[y][x];

      if (c < '0' || c > '9') continue;

      positions[c - '0'] = bj::Position{ static_cast<int>(x), static_cast<int>(y) };
    }
  }

  return positions;
}

size_t Grid::get_distance(bj::Position from, bj::Position to) const {
  if (from == to) return 0;

  std::set<bj::Position> visited;
  std::vector<bj::Position> current_positions;

  visited.emplace(from);
  current_positions.emplace_back(from);

  size_t steps = 0;

  std::vector<bj::Position> next;
  while (true) {
    next.clear();

    for (const bj::Position pos : current_positions) {
      pos.for_each_neighbour([&](bj::Position neighbour) {
        if (m_grid[neighbour.y][neighbour.x] == '#') return;
        if (visited.contains(neighbour)) return;
        visited.emplace(neighbour);
        next.emplace_back(neighbour);
      });
    }

    for (const auto & next_ : next) {
      if (next_ == to) {
        return steps + 1;
      }
    }

    ++steps;
    std::swap(next, current_positions);
  }
}


struct Result {
  size_t size;
  std::string path;
};

Result le_voyageur_du_commerce(
  const std::map<std::pair<int, int>, size_t> & distances,
  int from,
  int number_of_points,
  std::bitset<12> & visited_points,
  bool and_go_back
) {
  if (visited_points.count() == number_of_points) {
    if (and_go_back) {
      return Result { distances.find(std::pair<int, int>(from, 0))->second, std::to_string(from) };
    } else {
      return Result { 0, std::to_string(from) };
    }
  }

  std::optional<Result> min_total = std::nullopt;
  for (int i = 0; i != number_of_points; ++i) {
    if (visited_points[i]) continue;

    visited_points.set(i, true);
    size_t total = distances.find(std::pair<int, int>(from, i))->second;
    const auto this_res = le_voyageur_du_commerce(distances, i, number_of_points, visited_points, and_go_back);
    total += this_res.size;

    if (!min_total || min_total->size > total) {
      min_total = this_res;
      min_total->size = total;
      min_total->path = std::to_string(from) + min_total->path;
    }
    visited_points.set(i, false);
  }

  return min_total.value();
}

}

Output day_2016_24(const std::vector<std::string> & lines, const DayExtraInfo &) {

  Grid grid(lines);

  std::map<int, bj::Position> points_of_interest = grid.get_positions();

  const auto min = points_of_interest.begin()->first;
  const auto max = points_of_interest.rbegin()->first;

  std::map<std::pair<int, int>, size_t> distances;

  for (int i = min; i != max; ++i) {
    for (int j = i + 1; j <= max; ++j) {
      const size_t distance = grid.get_distance(points_of_interest[i], points_of_interest[j]);
      distances[std::pair<int, int>(i, j)] = distance;
      distances[std::pair<int, int>(j, i)] = distance;
    }
  }

  if (points_of_interest.size() >= 12) {
    std::cerr << "Too many points of interest, pressure too high\n";
  }

  std::bitset<12> visited;
  visited.set(0);
  const auto part_a = le_voyageur_du_commerce(distances, 0, points_of_interest.size(), visited, false);
  const auto part_b = le_voyageur_du_commerce(distances, 0, points_of_interest.size(), visited, true);

  return Output(part_a.size, part_b.size);
}
