#include "../advent_of_code.hpp"
#include <vector>
#include <optional>
#include <regex>

// https://adventofcode.com/2021/day/17

static bool is_in(int point1d, int min, int max) {
  return point1d >= min && point1d <= max;
}

struct TargetArea {
  int top;
  int bottom;
  int left;
  int right;

  bool is_in(int x, int y) const noexcept {
    return ::is_in(x, left, right) && ::is_in(y, bottom, top);
  }

  bool kill(int x, int y) const noexcept {
    return x > right || y < bottom;
  }
};

static std::optional<long long int> throw_ball(int x_velocity, int y_velocity, TargetArea target) {
  int x = 0;
  int y = 0;
  int highest = 0;
  bool missed = true;

  while (true) {
    x += x_velocity;
    y += y_velocity;
    if (target.kill(x, y)) break;
    
    if (highest < y) highest = y;

    if (target.is_in(x, y)){
      missed = false;
    }

    if (x_velocity != 0) --x_velocity;
    --y_velocity;
  }

  return missed ? std::nullopt : std::optional(highest);
}


Output day_2021_17(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::regex regex { R"(target area: x=([0-9]+)\.\.([0-9]+), y=-([0-9]+)\.\.-([0-9]+))" };
  std::smatch smatch;
  std::regex_search(lines[0], smatch, regex);

  //  x=20..30, y=-10..-5
  TargetArea target {
    -std::stoi(smatch[4].str()), // top    = -5
    -std::stoi(smatch[3].str()), // bottom = -10
    std::stoi(smatch[1].str()), // left    = 20
    std::stoi(smatch[2].str()), // right   = 30
  };

  std::optional<long long int> part_a = std::nullopt;
  long long int hitted = 0;

  for (int x_speed = target.right; x_speed > 0; --x_speed) {
    for (int y_speed = target.bottom; y_speed <= 50000; ++y_speed) {
      auto max_height = throw_ball(x_speed, y_speed, target);

      if (!max_height) continue;

      ++hitted;

      if (!part_a || *part_a < *max_height) {
        part_a = *max_height;
      }
    }
  }

  return Output(part_a ? part_a.value() : -1, hitted);
}
