#include "../advent_of_code.hpp"
#include <algorithm>
#include <set>
#include <vector>
#include <map>
#include <array>
#include "../util/position.hpp"

// https://adventofcode.com/2021/day/15

struct Grid {
  std::vector<int> danger_level;
  int width;
  int height;

  Grid(const std::vector<std::string> & lines, int coef = 1) {
    danger_level.resize(lines.size() * lines[0].size() * coef * coef);
    const auto input_width = lines[0].size();
    width = lines[0].size() * coef;
    height = lines.size() * coef;

    for (int X = 0; X != coef; ++X) {
      for (int Y = 0; Y != coef; ++Y) {

        for (size_t y = 0; y != lines.size(); ++y) {
          for (size_t x = 0; x != input_width; ++x) {
            const auto danger_pos = (x + X * input_width) + (y + Y * input_width) * width;
            const auto raw_danger_value = lines[y][x] - '0';
            // Danger values in parallel universes is + X + Y
            // but are counted in 1..9 = there are 9 values and the value after 9 is 1.
            const auto danger_value_in_pu = (raw_danger_value - 1 + X + Y) % 9 + 1;
            danger_level[danger_pos] = danger_value_in_pu;
          }
        }

      }
    }
  }

  [[nodiscard]] std::optional<int> operator[](bj::Position position) const {
    if (position.x < 0) return std::nullopt;
    if (position.y < 0) return std::nullopt;
    if (position.x >= width) return std::nullopt;
    if (position.y >= height) return std::nullopt;

    return danger_level[position.x + position.y * width];
  }

  [[nodiscard]] long long int compute_danger() const;
};

static bool next_step(std::vector<std::optional<int>> & total_danger, const Grid & grid) {
  bool stable = true;

  for (int x = 0; x != grid.width; ++x) {
    for (int y = 0; y != grid.height; ++y) {
      if (!total_danger[x + y * grid.width].has_value()) continue;

      const auto my_score = total_danger[x + y * grid.width].value();

      bj::Position{ x, y }.for_each_neighbour([&](bj::Position neighbour) {
        const auto dangerosity_here = grid[neighbour];
        if (!dangerosity_here) return;

        const auto dangerosity_to_reach = my_score + dangerosity_here.value();

        auto & current_known = total_danger[neighbour.x + neighbour.y * grid.width];

        if (!current_known || current_known.value() > dangerosity_to_reach) {
          current_known = dangerosity_to_reach;
          stable = false;
        }
      });
    }
  }

  return stable;
}

long long int Grid::compute_danger() const {
  std::vector<std::optional<int>> total_danger(danger_level.size(), std::nullopt);
  total_danger[0] = 0;

  while (!next_step(total_danger, *this)) {}

  return total_danger[total_danger.size() - 1].value();
}

Output day_2021_15(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const Grid grid(lines, 1);
  long long int part_a = grid.compute_danger();

  const Grid bigger_grid(lines, 5);
  long long int part_b = bigger_grid.compute_danger();
  
  return Output(part_a, part_b);
}
