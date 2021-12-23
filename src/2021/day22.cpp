#include "../advent_of_code.hpp"
#include <regex>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <optional>
#include <tuple>
#include <iostream>

// https://adventofcode.com/2021/day/22

// For part B, I tried to split intersected cuboids but I couldn't get the maths
// right. I found of Reddit another approach which relies on compression so
// I've implemented instead.
//
// The idea is to have a grid of point, but each point in the virtual grid can
// represent several points. 
//
// For example let us consider that we have those cuboids:
//
// 3
// 2   bbbb
// 1 aaxbbb
// 0 aaa
// . 0123456
//
// We build this virtual layout:
//
// 2   bbbb
// 1 aaxbbb
// 0 aaa
// . 0 1  2
//
// A position on an axis in the virtual layout is named 


struct VirtualCoordinate {
  int position;
  long long int absorbed;

  VirtualCoordinate(int min, int max) : position(min), absorbed(max - min) {}
};

struct VirtualAxis {
  std::vector<VirtualCoordinate> coordinates;

  VirtualAxis(std::vector<int> values) {
    // Sort + remove duplicates
    std::sort(values.begin(), values.end());
    const auto it = std::unique(values.begin(), values.end());
    if (it != values.end()) {
      values.erase(it);
    }

    // Build list of virtual coordinates
    for (size_t i = 0; i + 1 < values.size(); ++i) {
      coordinates.emplace_back(values[i], values[i + 1]);
    }
  }
  
  [[nodiscard]] auto size() const noexcept { return coordinates.size(); }

  std::optional<std::pair<int, int>> find_virtual_segment(int min, int max) const {
    int it = 0;

    while (it != coordinates.size()) {
      if (coordinates[it].position == min) break;
      ++it;
    }

    if (it == coordinates.size()) {
      return std::nullopt;
    }

    const int black_hole_starts_at = it;
    
    int quantity_to_absorb = max - min;
    quantity_to_absorb -= coordinates[it].absorbed;

    while (quantity_to_absorb > 0) {
      ++it;
      if (it == coordinates.size()) {
        return std::nullopt;
      }
      quantity_to_absorb -= coordinates[it].absorbed;
    }

    if (quantity_to_absorb == 0) {
      ++it;
      return std::pair<int, int>(black_hole_starts_at, it);
    } else {
      return std::nullopt;
    }
  }

  [[nodiscard]] long long int operator[](size_t i) const {
    return coordinates[i].absorbed;
  }
};

/** A 3D position */
struct Point3D {
  int x;
  int y;
  int z;
};

/** A 3D area */
struct Cuboid {
  Point3D min;
  Point3D max;

  template<typename Consumer>
  void for_each_point(Consumer consumer) const {
    for (int x = min.x; x < max.x; ++x) {
      for (int y = min.y; y < max.y; ++y) {
        for (int z = min.z; z < max.z; ++z) {
          consumer(Point3D{ x, y, z });
        }
      }
    }
  }
};

/** An instruction to turn on or off the lights in an area*/
struct Instruction {
  bool add;
  Cuboid cube;
};

/** The entire universe, with a new compressed graduation for our use case */
struct Universe {
  VirtualAxis x;
  VirtualAxis y;
  VirtualAxis z;

  static Universe build_from_instructions(
    const std::vector<Instruction> & instructions
  ) {
    std::vector<int> xs;
    std::vector<int> ys;
    std::vector<int> zs;

    for (const auto & instruction : instructions) {
      xs.push_back(instruction.cube.min.x);
      ys.push_back(instruction.cube.min.y);
      zs.push_back(instruction.cube.min.z);
      xs.push_back(instruction.cube.max.x);
      ys.push_back(instruction.cube.max.y);
      zs.push_back(instruction.cube.max.z);
    }

    return Universe(xs, ys, zs);
  }

  Universe(std::vector<int> xs, std::vector<int> ys, std::vector<int> zs)
  : x(xs), y(ys), z(zs) {}

  /** Remap an absolute cuboid to a virtual cuboid in this universe */
  [[nodiscard]] std::optional<Cuboid> transform(Cuboid cube) const {
    const auto xs = x.find_virtual_segment(cube.min.x, cube.max.x);
    const auto ys = y.find_virtual_segment(cube.min.y, cube.max.y);
    const auto zs = z.find_virtual_segment(cube.min.z, cube.max.z);

    if (xs && ys && zs) {
      return Cuboid {
        Point3D { xs->first, ys->first, zs->first },
        Point3D { xs->second, ys->second, zs->second }
      };
    } else {
      return std::nullopt;
    }
  }

  template <typename Consumer>
  void travel_the_universe(Consumer consumer) const {
    const Cuboid whole_area {
      Point3D{ 0, 0, 0 },
      Point3D{ x.size(), y.size(), z.size() }
    };

    whole_area.for_each_point(consumer);
  }

  [[nodiscard]] long long int operator[](Point3D point) const {
    return x[point.x] * y[point.y] * z[point.z];
  }
};

struct HitTheLights {
  static long long int let_the_music_move_you(
    const std::vector<Instruction> & instructions
  ) {
    HitTheLights light(Universe::build_from_instructions(instructions));
    light.run(instructions);
    return light.the_answer_is();
  }

  Universe universe;
  std::vector<bool> blackstar;

  HitTheLights(Universe universe)
  : universe(universe),
    blackstar(universe.x.size() * universe.y.size() * universe.z.size(), false)
  {}

  void run(const std::vector<Instruction> & instructions) {
    for (Instruction instruction : instructions) {
      const auto new_cube = universe.transform(instruction.cube);
      if (!new_cube) {
        std::cout << "Error on cube virtualization\n";
        return;
      }

      new_cube->for_each_point([&](Point3D point) {
        blackstar[to_1D_position(point)] = instruction.add;
      });
    }
  }

  [[nodiscard]] size_t to_1D_position(Point3D point) const {
    return point.x
      + (point.y) * universe.x.size()
      + (point.z) * universe.x.size() * universe.y.size();
  }

  [[nodiscard]] long long int the_answer_is() const {
    long long int c = 0;

    universe.travel_the_universe([&](Point3D point) {
      if (blackstar[to_1D_position(point)]) {
        c += universe[point];
      }
    });

    return c;
  }
};

static constexpr size_t part_a_max_distance = 50 + 50 + 1;// from -50 to 50

struct HelloDarkness {
  [[nodiscard]] static long long int my_old_friend(
    const std::vector<Instruction> & instructions
  ) {
    HelloDarkness instance;
    for (const auto & instruction : instructions) {
      instance.run(instruction);
    }
    return instance.how_many_lit();
  }

  std::array<std::array<std::array<bool,
    part_a_max_distance>, part_a_max_distance>, part_a_max_distance
  > area;

  HelloDarkness() {
    for (auto & xs : area) {
      for (auto & ys : xs) {
        ys.fill(false);
      }
    }
  }

  void run(const Instruction & instruction) {
    const auto & cube = instruction.cube;

    if (cube.min.x < -50 || cube.min.y < -50 || cube.min.z < -50) return;
    if (cube.min.x > 50 || cube.min.y > 50 || cube.min.z > 50) return;

    for (int x = cube.min.x; x < cube.max.x; ++x) {
      for (int y = cube.min.y; y < cube.max.y; ++y) {
        for (int z = cube.min.z; z < cube.max.z; ++z) {
          area[x + 50][y + 50][z + 50] = instruction.add;
        }
      }
    }
  }

  [[nodiscard]] size_t how_many_lit() const {
    size_t total = 0;
    for (auto & xs : area) {
      for (auto & ys : xs) {
        total += std::count(ys.begin(), ys.end(), true);
      }
    }

    return total;
  }
};



Output day_2021_22(const std::vector<std::string> & lines, const DayExtraInfo &) {
  // ==== Parsing
  const std::regex regex(R"((on|off) x=(-?[0-9]+)\.\.(-?[0-9]+),y=(-?[0-9]+)\.\.(-?[0-9]+),z=(-?[0-9]+)\.\.(-?[0-9]+))");
  std::smatch matches;

  std::vector<Instruction> instructions;
  for (const auto & line : lines) {
    if (line == "") continue;

    if (!std::regex_search(line, matches, regex)) {
      std::cerr << "Error on reading " << line << '\n';
      continue;
    }

    Cuboid cube = Cuboid{
      Point3D {
        std::stoi(matches[2].str()),
        std::stoi(matches[4].str()),
        std::stoi(matches[6].str()),
      },
      
      Point3D {
        std::stoi(matches[3].str()) + 1,
        std::stoi(matches[5].str()) + 1,
        std::stoi(matches[7].str()) + 1,
      }
    };

    instructions.push_back(Instruction{ matches[1] == "on", { cube } });
  }

  // ==== Run
  const long long int init = HelloDarkness::my_old_friend(instructions);
  const long long int reload = HitTheLights::let_the_music_move_you(instructions);

  return Output(init, reload);
}
