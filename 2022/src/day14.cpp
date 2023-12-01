#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <ranges>
#include <map>

// https://adventofcode.com/2022/day/14

class Grid {
private:
  std::map<bj::Position, char> m_grid;
  int m_ceiling = 2;
  bool ceiling_is_hard = false;

public:
  void add_brick(const bj::Position position);
  bool one_sand_drop();

  void set_hard_ceiling() { ceiling_is_hard = true; }
  [[nodiscard]] bool sand_is_blocked() const noexcept { return m_grid.contains(bj::Position{ 500, 0 }); }

  unsigned int drop_until_stability();
};

void Grid::add_brick(const bj::Position position) {
  m_grid[position] = '#';
  m_ceiling = std::max(m_ceiling, position.y + 2);
}

bool Grid::one_sand_drop() {
  if (sand_is_blocked()) return false;

  bj::Position sand_position { 500, 0 };

  while (true) {
    bj::Position future_pos = sand_position;
    future_pos.y += 1;

    if (future_pos.y == m_ceiling) {
      if (ceiling_is_hard) {
        break;
      } else {
        return false;
      }
    }

    if (!m_grid.contains(future_pos)) {
      sand_position = future_pos;
      continue;
    }

    future_pos.x -= 1;
    if (!m_grid.contains(future_pos)) {
      sand_position = future_pos;
      continue;
    }

    future_pos.x += 2;
    if (!m_grid.contains(future_pos)) {
      sand_position = future_pos;
      continue;
    }

    break;
  }

  m_grid[sand_position] = 'O';
  return true;
}

unsigned int Grid::drop_until_stability() {
  unsigned int retval = 0;

  while (one_sand_drop()) {
    ++retval;
  }

  return retval;
}

[[nodiscard]] bj::Position str_to_position(const std::string & str) {
  const size_t pos = str.find(',');
  return bj::Position{ std::stoi(str.substr(0, pos)), std::stoi(str.substr(pos + 1)) };
}

Output day_2022_14(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Grid grid;

  for (const std::string & line : lines) {
    const std::vector<std::string> positions_str = bj::string_split(line, " -> ");

    std::vector<bj::Position> positions;
    for (const std::string & position_str : positions_str) {
      positions.emplace_back(str_to_position(position_str));
    }

    for (size_t i = 0; i + 1 < positions.size(); ++i) {
      bj::Position::for_each_point_between(positions[i], positions[i + 1],
        [&](const bj::Position point) { grid.add_brick(point); }
      );
    }
  }

  Grid ceiled_grid = grid;
  ceiled_grid.set_hard_ceiling();

  return Output(grid.drop_until_stability(), ceiled_grid.drop_until_stability());
}
