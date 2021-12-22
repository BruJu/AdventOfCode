#include "../advent_of_code.hpp"
#include <regex>
#include <vector>
#include <unordered_map>
#include <tuple>

struct Cube {
  int x_min;
  int x_max;
  int y_min;
  int y_max;
  int z_min;
  int z_max;

  [[nodiscard]] bool is_init_protocole() const {
    return x_min >= -50 && x_max <= 50
      && y_min >= -50 && y_max <= 50
      && z_min >= -50 && z_max <= 50;
  }

  [[nodiscard]] long long int volume() const {
    const long long int width = x_max - x_min + 1;
    const long long int height = y_max - y_min + 1;
    const long long int depth = z_max - z_min + 1;
    return width * height * depth;
  }

  [[nodiscard]] bool equals_or_disjoint_to(const Cube other) const {
    if (x_min == other.x_min && x_max == other.x_max
      && y_min == other.y_min && y_max == other.y_max
      && z_min == other.z_min && z_max == other.z_max) {
      return true;
    }

    if (x_min > other.x_max) return true;
    if (other.x_min > x_max) return true;
    if (y_min > other.y_max) return true;
    if (other.y_min > y_max) return true;
    if (z_min > other.z_max) return true;
    if (other.z_min > z_max) return true;

    return false;
  }

  void split(const Cube & cube, std::vector<Cube> & destination) {
    // TODO
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
      run(instruction.add, instruction.cube);
    }
  }

  void run(bool new_state, Cube cube) {
    for (int x = cube.x_min; x <= cube.x_max; ++x) {
      for (int y = cube.y_min; y <= cube.y_max; ++y) {
        for (int z = cube.z_min; z <= cube.z_max; ++z) {
          area[x + 50][y + 50][z + 50] = new_state;
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
  std::vector<Cube> lit_cubes;

  void run(const Instruction & instruction) {

  }


  [[nodiscard]] long long int how_many_lit() const {
    long long int total = 0;

    for (const auto & cube : lit_cubes) {
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

    Cube cube {
      std::stoi(matches[2].str()),
      std::stoi(matches[3].str()),
      std::stoi(matches[4].str()),
      std::stoi(matches[5].str()),
      std::stoi(matches[6].str()),
      std::stoi(matches[7].str()),
    };

    instructions.push_back(Instruction{ matches[1] == "on", { cube } });
  }

  // Slow Part A
  PartA slow_part_a;
  for (const auto & instruction : instructions) {
    slow_part_a.run(instruction);
  }

  return Output(slow_part_a.how_many_lit(), 0);
}
