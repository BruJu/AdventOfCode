#include "../../common/advent_of_code.hpp"
#include "../../util/md5.h"
#include <optional>
#include <array>
#include <queue>


static bool is_open(char c) {
  return (c >= 'b' && c <= 'f') || (c >= 'B' && c <= 'F');
}

struct GridPosition {
  std::string path = "";
  int x = 0;
  int y = 0;

  GridPosition() = default;
  GridPosition(const GridPosition & parent, char direction) {
    path = parent.path + direction;
    x = parent.x;
    y = parent.y;
    if (direction == 'U') --y;
    if (direction == 'D') ++y;
    if (direction == 'L') --x;
    if (direction == 'R') ++x;
  }

  std::array<std::optional<GridPosition>, 4> get_next(const std::string & hash) const {
    // Not vector to an allocation (even though we use std::string)
    // std::vector               -> 95ms on examples / 46 ms on real input
    // std::array<std::optional> -> 99ms on examples / 32 ms on real input
    std::array<std::optional<GridPosition>, 4> result;
    result.fill(std::nullopt);

    if (y != 0 && is_open(hash[0])) result[0] = GridPosition(*this, 'U');
    if (y != 3 && is_open(hash[1])) result[1] = GridPosition(*this, 'D');
    if (x != 0 && is_open(hash[2])) result[2] = GridPosition(*this, 'L');
    if (x != 3 && is_open(hash[3])) result[3] = GridPosition(*this, 'R');

    return result;
  }

  [[nodiscard]] bool is_end() const noexcept {
    return x == 3 && y == 3;
  }
};

static std::pair<std::string, long long int> find_part_a(const std::string & input) {
  MD5 md5;

  std::queue<GridPosition> next_paths;
  next_paths.push(GridPosition());

  std::string first_solution;
  long long int last_length = 0;

  while (!next_paths.empty()) {
    const auto current = next_paths.front();
    next_paths.pop();

    for (const auto & next : current.get_next(md5(input + current.path))) {
      if (!next) continue;

      if (next->is_end()) {
        if (first_solution == "") {
          first_solution = next->path;
        }

        last_length = next->path.size();
      } else {
        next_paths.push(next.value());
      }
    }
  }

  return std::pair(first_solution, last_length);
}

Output day_2016_17(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto [a, b] = find_part_a(lines[0]);
  return Output(a, b);
}
