#include "../advent_of_code.hpp"
#include <array>
#include <compare>
#include <map>
#include <ranges>
#include <set>
#include <stack>

// https://adventofcode.com/2022/day/18

namespace {

struct Cube {
  int x;
  int y;
  int z;
  
  friend auto operator<=>(const Cube &, const Cube &) = default;

  [[nodiscard]] std::array<Cube, 6> adj_cubes() const {
    return std::array<Cube, 6>{
      Cube{ x - 1, y, z },
      Cube{ x + 1, y, z },
      Cube{ x, y - 1, z },
      Cube{ x, y + 1, z },
      Cube{ x, y, z - 1 },
      Cube{ x, y, z + 1 }
    };
  }
};

std::pair<Cube, Cube> compute_bounds(const std::set<Cube> & cubes) {
  Cube min_bound {0,0,0};
  Cube max_bound {0,0,0};

  for (const Cube & cube : cubes) {
    min_bound.x = std::min(min_bound.x, cube.x);
    min_bound.y = std::min(min_bound.y, cube.z);
    min_bound.z = std::min(min_bound.y, cube.z);
    
    max_bound.x = std::max(max_bound.x, cube.x);
    max_bound.y = std::max(max_bound.y, cube.z);
    max_bound.z = std::max(max_bound.y, cube.z);
  }

  --min_bound.x;
  --min_bound.y;
  --min_bound.z;
  ++max_bound.x;
  ++max_bound.y;
  ++max_bound.z;

  return { min_bound, max_bound };
}

bool is_out_of_bound(const Cube & cube, const Cube & min_bound, const Cube & max_bound) {
  if (cube.x < min_bound.x) return true;
  if (cube.y < min_bound.y) return true;
  if (cube.z < min_bound.z) return true;

  if (cube.x > max_bound.x) return true;
  if (cube.y > max_bound.y) return true;
  if (cube.z > max_bound.z) return true;

  return false;
}

struct CubeVisitor {
  std::set<Cube> visited;
  std::stack<Cube> to_visit;

  void add_cube(const Cube & cube) {
    if (visited.contains(cube)) return;
    visited.emplace(cube);
    to_visit.push(cube);
  }

  [[nodiscard]] std::optional<Cube> next() {
    if (to_visit.empty()) return std::nullopt;
    const Cube res = to_visit.top();
    to_visit.pop();
    return res;
  }
};

}


Output day_2022_18(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::set<Cube> cubes;

  for (const std::string & line : lines) {
    const auto l = bj::string_split(line, ",");
    cubes.emplace(Cube { std::stoi(l[0]), std::stoi(l[1]), std::stoi(l[2]) });
  }

  int surface_of_structure = 0;
  for (const Cube & cube : cubes) {
    for (const Cube & adj_cube : cube.adj_cubes()) {
      if (!cubes.contains(adj_cube)) {
        ++surface_of_structure;
      }
    }
  }

  const auto [min_bound, max_bound] = compute_bounds(cubes);

  CubeVisitor visitor;
  visitor.add_cube(min_bound);

  int surface_to_air = 0;

  while (const auto opt_cube = visitor.next()) {
    for (const Cube & neighbour : opt_cube->adj_cubes()) {
      if (is_out_of_bound(neighbour, min_bound, max_bound)) continue;
      if (visitor.visited.contains(neighbour)) continue;

      if (cubes.contains(neighbour)) {
        ++surface_to_air;
      } else {
        visitor.add_cube(neighbour);
      }
    }
  }

  return Output(surface_of_structure, surface_to_air);
}
