#include "../../common/advent_of_code.hpp"
#include <vector>
#include <span>
#include <utility>
#include <map>
#include <algorithm>
#include <numeric>
#include <cstring>

// https://adventofcode.com/2023/day/2

struct SetOfCubes {
  std::map<std::string, int> cubes;

  [[nodiscard]] int get_color_qtt(const std::string & name) const {
    const auto it = cubes.find(name);
    if (it == cubes.end()) return 0;
    return it->second;
  }

  [[nodiscard]] bool operator<=(const SetOfCubes & rhs) const noexcept {
    for (const auto & [color, qtt] : rhs.cubes) {
      if (get_color_qtt(color) > qtt) return false;
    }
    return true;
  }

  static SetOfCubes GetElfCubes() {
    SetOfCubes ret;
    ret.cubes["red"] = 12;
    ret.cubes["green"] = 13;
    ret.cubes["blue"] = 14;
    return ret;
  }

  SetOfCubes accumulate_max(const SetOfCubes & rhs) {
    for (const auto & [color, qtt] : rhs.cubes) {
      cubes[color] = std::max(cubes[color], qtt);
    }
    return *this;
  }

  int get_power() const {
    return get_color_qtt("red") * get_color_qtt("green") * get_color_qtt("blue");
  }
};


struct Game {
  int game_id;
  std::vector<SetOfCubes> bags;

  explicit Game(const std::string & line);
};


Game::Game(const std::string & line) {
  // Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green
  //      _
  //        _______________________ _______________________
  //        ______ _______ ________
  // Cut by ; then by ,
  size_t dotdot = line.find(':') - std::strlen("Game ");
  std::string game_id_str = line.substr(std::strlen("Game "), dotdot);
  game_id = std::stoi(game_id_str);

  std::string draws = line.substr(std::strlen("Game ") + dotdot + 1);

  for (const std::string & draw : bj::string_split(draws, ";")) {
    bags.emplace_back();

    for (const std::string & item : bj::string_split(draw, ",")) {
      size_t separator = item.find(' ', 1);

      std::string cube_color = item.substr(separator + 1);
      std::string cube_qtt = item.substr(1, separator - 1);

      bags.back().cubes[cube_color] += std::stoi(cube_qtt);
    }
  }
}


Output day_2023_02(const std::vector<std::string> & lines, const DayExtraInfo &) {
  SetOfCubes reference_cubes = SetOfCubes::GetElfCubes();

  int part_a = 0;
  int part_b = 0;

  for (const std::string & line : lines) {
    Game game { line };

    bool less_than_reference = std::all_of(game.bags.begin(), game.bags.end(),
      [&](const SetOfCubes & draw) { return draw <= reference_cubes; }
    );

    const SetOfCubes min_in_bag = std::accumulate(game.bags.begin(), game.bags.end(),
      SetOfCubes(),
      [](SetOfCubes lhs, const SetOfCubes & rhs) {
        lhs.accumulate_max(rhs);
        return lhs;
      }
    );

    if (less_than_reference) part_a += game.game_id;
    part_b += min_in_bag.get_power();
  }

  return Output(part_a, part_b);
}
