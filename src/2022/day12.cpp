#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include <concepts>
#include <exception>
#include <optional>
#include <queue>
#include <set>


// https://adventofcode.com/2022/day/12

namespace {

struct Grid {
  const std::vector<std::string> * lines;

  [[nodiscard]] char elevation_at(bj::Position position) const {
    return (*lines)[position.y][position.x];
  }

  [[nodiscard]] bj::Position find_pos_of(char c) const {
    for (size_t y = 0; y != lines->size(); ++y) {
      for (size_t x = 0; x != (*lines)[y].length(); ++x) {
        bj::Position position{ int(x), int(y) };

        if (elevation_at(position) == c) {
          return position;
        }
      }
    }

    throw std::runtime_error("Position not found");
  }

  template<std::predicate<char> is_final_position, std::relation<char, char> can_go_on>
  [[nodiscard]] std::optional<std::uint64_t> go_to(
    const bj::Position from,
    is_final_position is_final_position_,
    can_go_on can_go_on_
    ) const;

  [[nodiscard]] bool is_out_of_bound(const bj::Position & position) const {
    if (position.y < 0 || std::cmp_greater_equal(position.y, lines->size())) return true;
    if (position.x < 0 || std::cmp_greater_equal(position.x, (*lines)[position.y].size())) return true;
    return false;
  }
  
  [[nodiscard]] static char normalize(const char elevation) {
    if (elevation == 'S') return 'a';
    if (elevation == 'E') return 'z';
    return elevation;
  }
};

template<std::predicate<char> is_final_position, std::relation<char, char> can_go_on>
std::optional<std::uint64_t> Grid::go_to(
  const bj::Position from,
  is_final_position is_final_position_,
  can_go_on can_go_on_
) const {
  std::set<bj::Position> visited;
  std::queue<std::pair<bj::Position, std::uint64_t>> next_values;

  next_values.emplace(from, 0);
  visited.emplace(from);

  std::optional<std::uint64_t> result = std::nullopt;

  while (!result && !next_values.empty()) {
    const auto [pos, steps] = next_values.front();
    next_values.pop();

    const char my_elevation = elevation_at(pos);

    pos.for_each_neighbour([&](bj::Position neighbour) {
      if (result) return;
      if (is_out_of_bound(neighbour)) return;
      if (visited.contains(neighbour)) return;
      
      const char neighbour_elevation = elevation_at(neighbour);

      if (can_go_on_(normalize(my_elevation), normalize(neighbour_elevation))) {
        if (is_final_position_(neighbour_elevation)) {
          result = steps + 1;
          return;
        }

        next_values.emplace(neighbour, steps + 1);
        visited.emplace(neighbour);
      }
    });
  }

  return result;
}

}


Output day_2022_12(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const Grid grid = Grid{ &lines };

  const bj::Position position = grid.find_pos_of('S');
  const auto steps_a = grid.go_to(position,
    [](const char elevation) { return elevation == 'E'; },
    [](const char from, const char to) { return from >= to - 1; }
  );

  const bj::Position target = grid.find_pos_of('E');
  const auto steps_b = grid.go_to(target,
    [](const char elevation) { return Grid::normalize(elevation) == 'a'; },
    [](const char from, const char to) { return from <= to + 1; }
  );

  return Output(steps_a.value(), steps_b.value());
}
