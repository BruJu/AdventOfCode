#include "../advent_of_code.hpp"
#include <array>
#include <compare>
#include <map>
#include <ranges>
#include <set>
#include <stack>

// https://adventofcode.com/2022/day/18

struct Cube {
  int x;
  int y;
  int z;
  
  friend auto operator<=>(const Cube &, const Cube &) = default;

  std::array<Cube, 6> adj_cubes() const {
    return std::array<Cube, 6>{
      Cube{ x - 1, y, z },
      Cube{ x + 1, y, z },
      Cube{ x, y - 1,z },
      Cube{ x, y + 1,z },
      Cube{ x, y, z - 1 },
      Cube{ x, y, z + 1 }
    };
  }
};

class Cartography {
public:
  enum class Content { Cube, AirFree };

private:

  Cube min_bound {0,0,0};
  Cube max_bound {0,0,0};

  std::map<Cube, Content> map_cubes;

  void fill_the_map();

public:
  explicit Cartography(const std::set<Cube> & cubes);

  [[nodiscard]] bool is_air_free(Cube cube) const;
  [[nodiscard]] bool is_out_of_bound(Cube cube) const;
};

Cartography::Cartography(const std::set<Cube> & cubes) {
  for (const Cube & cube : cubes) {
    min_bound.x = std::min(min_bound.x, cube.x);
    min_bound.y = std::min(min_bound.y, cube.z);
    min_bound.z = std::min(min_bound.y, cube.z);
    
    max_bound.x = std::max(max_bound.x, cube.x);
    max_bound.y = std::max(max_bound.y, cube.z);
    max_bound.z = std::max(max_bound.y, cube.z);

    map_cubes.emplace(cube, Content::Cube);
  }

  --min_bound.x;
  --min_bound.y;
  --min_bound.z;
  ++max_bound.x;
  ++max_bound.y;
  ++max_bound.z;

  fill_the_map();
}

void Cartography::fill_the_map() {
  std::stack<Cube> to_visit;

  const auto acc_cube = [&](const Cube cube) {
    map_cubes.emplace(cube, Content::AirFree);
    to_visit.emplace(cube);
  };

  for (const Cube adj : min_bound.adj_cubes()) {
    if (!is_out_of_bound(adj) && !map_cubes.contains(adj)) {
      acc_cube(adj);
    }
  }

  while (!to_visit.empty()) {
    const Cube next = to_visit.top();
    to_visit.pop();

    for (const Cube adj : next.adj_cubes()) {
      if (!is_out_of_bound(adj) && !map_cubes.contains(adj)) {
        acc_cube(adj);
      }
    }
  }
}

bool Cartography::is_air_free(const Cube cube) const{
  if (is_out_of_bound(cube)) return true;

  const auto it = map_cubes.find(cube);
  if (it == map_cubes.end()) return false;
  return it->second == Content::AirFree;
}

bool Cartography::is_out_of_bound(const Cube cube) const {
  if (cube.x < min_bound.x) return true;
  if (cube.y < min_bound.y) return true;
  if (cube.z < min_bound.z) return true;

  if (cube.x > max_bound.x) return true;
  if (cube.y > max_bound.y) return true;
  if (cube.z > max_bound.z) return true;

  return false;
}

Output day_2022_18(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::set<Cube> cubes;

  for (const std::string & line : lines) {
    const auto l = bj::string_split(line, ",");
    cubes.emplace(Cube { std::stoi(l[0]), std::stoi(l[1]), std::stoi(l[2]) });
  }

  const Cartography carto(cubes);

  int surface_of_structure = 0;
  int surface_to_air = 0;

  for (const Cube & cube : cubes) {
    for (const Cube & adj_cube : cube.adj_cubes()) {
      if (cubes.contains(adj_cube)) {
        ++surface_of_structure;
      }

      if (carto.is_air_free(adj_cube)) {
        ++surface_to_air;
      }
    }
  }

  return Output(surface_of_structure, surface_to_air);
}
