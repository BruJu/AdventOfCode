#include "../advent_of_code.hpp"
#include <regex>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <tuple>
#include <iostream>

static constexpr size_t X = 0;
static constexpr size_t Y = 1;
static constexpr size_t Z = 2;

struct Segment {
  int min;
  int max;

  [[nodiscard]] bool is_init_protocole() const noexcept {
    return min >= -50 && max <= 50;
  }

  [[nodiscard]] long long int nb_of_points() const noexcept {
    return max - min + 1;
  }

  [[nodiscard]] bool operator==(const Segment other) const noexcept {
    return min == other.min && max == other.max;
  }

  [[nodiscard]] bool disjoint_to(const Segment other) const noexcept {
    return /* This is after */ min > other.max || /* Other is before */ max < other.min;
  }

  [[nodiscard]] bool operator<(const Segment other) const noexcept {
    if (min < other.min) return true;
    if (min > other.min) return false;
    if (max < other.max) return true;
    if (max > other.max) return false;
    return false;
  }

  friend std::ostream & operator<<(std::ostream & stream, Segment seg) {
    return stream << seg.min << "~" << seg.max;
  }
};

struct Cube {
  std::array<Segment, 3> xyz;

  Cube(int x_min, int x_max, int y_min, int y_max, int z_min, int z_max) {
    xyz[X] = Segment { x_min, x_max };
    xyz[Y] = Segment { y_min, y_max };
    xyz[Z] = Segment { z_min, z_max };
  }

  friend std::ostream & operator<<(std::ostream & stream, Cube cube) {
    return stream << "(" << cube.xyz[X] << " | " << cube.xyz[Y] << " | " << cube.xyz[Z] << ")";
  }

  [[nodiscard]] bool is_init_protocole() const {
    return xyz[X].is_init_protocole()
      && xyz[Y].is_init_protocole()
      && xyz[Z].is_init_protocole();
  }

  [[nodiscard]] long long int volume() const {
    return xyz[X].nb_of_points() * xyz[Y].nb_of_points() * xyz[Z].nb_of_points();
  }

  [[nodiscard]] bool operator==(const Cube & other) const {
    return xyz == other.xyz;
  }

  [[nodiscard]] bool operator<(const Cube & other) const {
    if (xyz[X] < other.xyz[X]) return true;
    if (other.xyz[X] < xyz[X]) return false;
    if (xyz[Y] < other.xyz[Y]) return true;
    if (other.xyz[Y] < xyz[Y]) return false;
    if (xyz[Z] < other.xyz[Z]) return true;
    if (other.xyz[Z] < xyz[Z]) return false;
    return false;
  }

  [[nodiscard]] bool disjoint_to(const Cube & other) const {
    return xyz[X].disjoint_to(other.xyz[X])
      || xyz[Y].disjoint_to(other.xyz[Y])
      || xyz[Z].disjoint_to(other.xyz[Z]);
  }

  [[nodiscard]] bool equals_or_disjoint_to(const Cube other) const {
    return *this == other || disjoint_to(other);
  }

  static std::vector<Cube> ensure_distinct(const Cube lhs, const Cube rhs) {
    if (lhs.equals_or_disjoint_to(rhs)) return {};

    std::vector<Cube> cut;

    std::vector<int> xs = { lhs.xyz[X].min, lhs.xyz[X].max, rhs.xyz[X].min, rhs.xyz[X].max };
    std::vector<int> ys = { lhs.xyz[Y].min, lhs.xyz[Y].max, rhs.xyz[Y].min, rhs.xyz[Y].max };
    std::vector<int> zs = { lhs.xyz[Z].min, lhs.xyz[Z].max, rhs.xyz[Z].min, rhs.xyz[Z].max };

    constexpr auto clear_vector = [](std::vector<int> & vec) {
      std::sort(vec.begin(), vec.end());
      const auto last = std::unique(vec.begin(), vec.end());
      vec.erase(last, vec.end());
    };
    
    clear_vector(xs);
    clear_vector(ys);
    clear_vector(zs);

    for (size_t xi = 0; xi + 1 != xs.size(); ++xi) {
      for (size_t yi = 0; yi + 1 != ys.size(); ++yi) {
        for (size_t zi = 0; zi + 1 != zs.size(); ++zi) {
          Cube cube(
            xs[xi], xs[xi + 1] - 1,
            ys[yi], ys[yi + 1] - 1,
            zs[zi], zs[zi + 1] - 1
          );

          if (!lhs.disjoint_to(cube) || !rhs.disjoint_to(cube)) {
            cut.push_back(cube);
          }
        }
      }
    }

    return cut;
  }
};

struct Instruction {
  bool add;
  Cube cube;
};

struct Point3D : std::tuple<int, int, int> {};

static constexpr size_t part_a_max_distance = 50 + 50 + 1;// from -50 to 50

struct PartA {
  std::array<std::array<std::array<bool,
    part_a_max_distance>, part_a_max_distance>, part_a_max_distance
  > area;

  PartA() {
    for (auto & xs : area) {
      for (auto & ys : xs) {
        ys.fill(false);
      }
    }
  }

  void run(const Instruction & instruction) {
    if (instruction.cube.is_init_protocole()) {
      const auto & cube = instruction.cube.xyz;
      for (int x = cube[X].min; x <= cube[X].max; ++x) {
        for (int y = cube[Y].min; y <= cube[Y].max; ++y) {
          for (int z = cube[Z].min; z <= cube[Z].max; ++z) {
            area[x + 50][y + 50][z + 50] = instruction.add;
          }
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

struct PartB {
  std::map<Cube, std::vector<Cube>> managed_cubes = {};
  std::set<Cube> state = {};

  explicit PartB(std::vector<Cube> cubes) {
    const auto original = cubes;
    bool stable = false;
    while (!stable) {
      // std::cout << cubes.size() << '\n';
      stable = true;

      for (size_t i_lhs = 0; i_lhs + 1 < cubes.size(); ++i_lhs) {
        // std::cout << "lhs=" << i_lhs << '\n';

        size_t i_rhs = i_lhs + 1;

        while (i_rhs < cubes.size()) {
          // std::cout << "rhs=" << i_rhs << '\n';
          const auto cut = Cube::ensure_distinct(cubes[i_lhs], cubes[i_rhs]);

          if (cut.size() != 0) {
            if (cut.size() < 2)  {
              // std::cout << cubes[i_lhs] << " & " << cubes[i_rhs] << " made one \n";
            } else if (cut.size() == 2) {
              // std::cout << cubes[i_lhs] << " & " << cubes[i_rhs] << " not equaldistinct but two blocks\n";
            } else {
              // std::cout << cubes[i_lhs] << " & " << cubes[i_rhs] << " boom\n";
              cubes[i_lhs] = cut[0];
              cubes[i_rhs] = cut[1];
              for (size_t i = 2; i != cut.size(); ++i) {
                cubes.push_back(cut[i]);
              }
            }
          }

          ++i_rhs;
        }
      }
    }

    std::cout << "end\n";

    for (const auto cube : cubes) {
      for (const auto ori : original) {
        if (!ori.disjoint_to(cube)) {
          managed_cubes[ori].push_back(cube);
        }
      }
    }

    std::cout << "true end\n";
  }

  void run(const Instruction & instruction) {
    const auto & cubes = managed_cubes.find(instruction.cube)->second;

    for (const auto cube : cubes) {
      if (instruction.add) {
        state.emplace(cube);
      } else {
        const auto it = state.find(cube);
        if (it != state.end()) state.erase(it);
      }
    }
  }


  [[nodiscard]] long long int how_many_lit() const {
    long long int total = 0;

    for (const auto & cube : state) {
      total += cube.volume();
    }

    return total;
  }
};



Output day_2021_22(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::regex regex(R"((on|off) x=(-?[0-9]+)\.\.(-?[0-9]+),y=(-?[0-9]+)\.\.(-?[0-9]+),z=(-?[0-9]+)\.\.(-?[0-9]+))");
  std::smatch matches;

  std::vector<Instruction> instructions;
  for (const auto & line : lines) {
    if (line == "") continue;

    if (!std::regex_search(line, matches, regex)) {
      std::cerr << "Error on reading " << line << '\n';
      continue;
    }

    Cube cube = Cube(
      std::stoi(matches[2].str()),
      std::stoi(matches[3].str()),
      std::stoi(matches[4].str()),
      std::stoi(matches[5].str()),
      std::stoi(matches[6].str()),
      std::stoi(matches[7].str())
    );

    instructions.push_back(Instruction{ matches[1] == "on", { cube } });
  }

  // Slow Part A
  PartA slow_part_a;
  for (const auto & instruction : instructions) {
    slow_part_a.run(instruction);
  }

std::cout << "B\n";
  // Part B
  std::vector<Cube> cubes;
  for (const auto & instruction : instructions) {
    cubes.push_back(instruction.cube);
  }
std::cout << "ok\n";
  PartB part_b(cubes);
  for (const auto & instruction : instructions) {
    part_b.run(instruction);
  }

  return Output(slow_part_a.how_many_lit(), 0);
}
