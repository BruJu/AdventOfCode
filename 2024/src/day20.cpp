#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <map>
#include <utility>

// https://adventofcode.com/2024/day/20

using i64 = std::int64_t;

namespace {

bj::Position find_in_grid(const std::vector<std::string> & lines, char c) {
  for (size_t y = 0; y != lines.size(); ++y) {
    for (size_t x = 0; x != lines[y].size(); ++x) {
      if (lines[y][x] == c) return bj::Position::from(x, y);
    }
  }
  return bj::Position{ -1, -1 };
}

template<typename T>
std::vector<std::vector<T>> replicate(const std::vector<std::string> & lines, T default_value) {
  std::vector<std::vector<T>> output;
  output.reserve(lines.size());
  for (const std::string & line : lines) {
    output.emplace_back();
    output.reserve(line.size());
    for (const char _ : line) {
      output.back().emplace_back(default_value);
    }
  }
  return output;
}


std::pair<
  std::vector<std::vector<std::optional<i64>>>,
  std::vector<bj::Position>
> compute_regular_path(const std::vector<std::string> & lines) {
  std::vector<std::vector<std::optional<i64>>> distance_from_start = replicate<std::optional<i64>>(lines, std::nullopt);
  std::vector<bj::Position> path;

  bj::Position car = find_in_grid(lines, 'S');

  int current_distance = 0;
  distance_from_start[car.y][car.x] = 0;
  path.emplace_back(car);


  while (true) {
    for (bj::Direction dir : { bj::Direction::Left, bj::Direction::Right, bj::Direction::Top, bj::Direction::Down }) {
      bj::Position copy = car;
      copy.move(dir);

      if (copy.is_out_of_bound(lines)) continue;
      if (distance_from_start[copy.y][copy.x]) continue;
      if (lines[copy.y][copy.x] == '.' || lines[copy.y][copy.x] == 'E') {
        car = copy;
        break;
      }

      if (dir == bj::Direction::Down) {
        throw std::runtime_error("not found");
      }
    }

    distance_from_start[car.y][car.x] = current_distance + 1;
    current_distance += 1;
    path.emplace_back(car);
    if (lines[car.y][car.x] == 'E') {
      break;
    }
  }

  return { distance_from_start, path };
}


struct PressingTheCtrlButton {
  std::vector<std::vector<std::optional<i64>>> reference_times;
  std::map<i64, i64> time_save; // An array would be better

  void consider(bj::Position pos, int at_with_cheating) {
    if (pos.x < 0 || pos.y < 0) return;
    if (std::cmp_greater_equal(pos.y, reference_times.size())) return;
    if (std::cmp_greater_equal(pos.x, reference_times[pos.y].size())) return;

    if (!reference_times[pos.y][pos.x]) return;
    
    const int expected_time = *reference_times[pos.y][pos.x];
    const int delta = expected_time - at_with_cheating;

    if (delta > 0) {
      time_save[delta] += 1;
    }
  }

  i64 sum_at_time_save_of_at_least(i64 min_time_save) {
    i64 acc = 0;
    for (const auto & [delta, times] : time_save) {
      if (delta >= min_time_save) {
        acc += times;
      }
    }
    return acc;
  }
};


}

Output day_2024_20(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  const auto [distance_from_start, path] = compute_regular_path(lines);

  PressingTheCtrlButton part_a { distance_from_start, {} };
  PressingTheCtrlButton part_b { distance_from_start, {} };

  for (bj::Position car : path) {
    const int step_at_cat = *distance_from_start[car.y][car.x];

    // Part A
    for (bj::Direction dir : { bj::Direction::Left, bj::Direction::Right, bj::Direction::Top, bj::Direction::Down }) {
      bj::Position copy = car;
      copy.move(dir);
      copy.move(dir);

      part_a.consider(copy, step_at_cat + 2);
    }

    // Part B
    static constexpr int max_time_stop = 20;
    for (int dy = -max_time_stop; dy <= max_time_stop; ++dy) {
      for (int dx = -max_time_stop; dx <= max_time_stop; ++dx) {
        const int manhattan = bj::Position(dx, dy).manhattan();
        if (manhattan > max_time_stop) {
          continue;
        }

        bj::Position pos = car;
        pos.x += dx;
        pos.y += dy;


        part_b.consider(pos, step_at_cat + manhattan);
      }
    }
  }

  return Output(
    part_a.sum_at_time_save_of_at_least(dei.part_a_extra_param ? dei.part_a_extra_param : 100),
    part_b.sum_at_time_save_of_at_least(dei.part_b_extra_param ? dei.part_b_extra_param : 100)
    );
}
