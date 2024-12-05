#include "../../common/advent_of_code.hpp"
#include <utility>
#include "../../util/position.hpp"

// https://adventofcode.com/2024/day/4

enum class Direction {
  // Directions named according to num pad
  Dir8,
  Dir9,
  Dir6,
  Dir3,
  Dir2,
  Dir1,
  Dir4,
  Dir7
};

static
bj::Position move(bj::Position position, Direction direction) {
  using enum Direction;
  switch (direction) {
    case Dir8:
      position.y -= 1;
      break;
    case Dir9:
      position.y -= 1;
      position.x += 1;
      break;
    case Dir6:
      position.x += 1;
      break;
    case Dir3:
      position.x += 1;
      position.y += 1;
      break;
    case Dir2:
      position.y += 1;
      break;
    case Dir1:
      position.y += 1;
      position.x -= 1;
      break;
    case Dir4:
      position.x -= 1;
      break;
    case Dir7:
      position.x -= 1;
      position.y -= 1;
      break;
  }
  return position;
}

static
std::optional<char> get_letter(const std::vector<std::string> & grid, bj::Position position) {
  if (position.x < 0) return std::nullopt;
  if (position.y < 0) return std::nullopt;
  if (std::cmp_greater_equal(position.y, grid.size())) return std::nullopt;
  if (std::cmp_greater_equal(position.x, grid[position.y].size())) return std::nullopt;
  return grid[position.y][position.x];
}

static
bool is_xmas(const std::vector<std::string> & grid, bj::Position position, Direction direction) {
  static constexpr char XMAS[] = {'X','M','A','S'};

  for (const char letter : XMAS) {
    const auto letter_on_grid = get_letter(grid, position);
    if (!letter_on_grid) return false;
    if (*letter_on_grid != letter) return false;
    position = move(position, direction);
  }

  return true;
}

static
bool is_masmas(const std::vector<std::string> & grid, bj::Position position) {
  if (grid[position.y][position.x] != 'A') {
    return false;
  }

  static constexpr auto get_letter_in_dir = [](const std::vector<std::string> & grid, bj::Position position, Direction direction) {
    return get_letter(grid, move(position, direction));
  };

  static constexpr auto get_expected_other = [](std::optional<char> letter) -> std::optional<char> {
    if (!letter) return std::nullopt;

    switch (letter.value()) {
      case 'M': return 'S';
      case 'S': return 'M';
      default: return std::nullopt;
    }
  };

  const auto optTopLeft  = get_expected_other(get_letter_in_dir(grid, position, Direction::Dir7));
  const auto optTopRight = get_expected_other(get_letter_in_dir(grid, position, Direction::Dir9));

  if (!optTopLeft || !optTopRight) return false;

  const auto bottomLeft  = get_letter_in_dir(grid, position, Direction::Dir1);
  const auto bottomRight = get_letter_in_dir(grid, position, Direction::Dir3);

  return bottomLeft == *optTopRight && bottomRight == *optTopLeft;
}


Output day_2024_04(const std::vector<std::string> & lines, const DayExtraInfo &) {
  long long int part_a = 0;
  long long int part_b = 0;

  for (size_t y = 0; y != lines.size(); ++y) {
    for (size_t x = 0; x != lines[y].size(); ++x) {
      bj::Position position { .x = static_cast<int>(x), .y = static_cast<int>(y) };

      for (const Direction direction : {
        Direction::Dir8,
        Direction::Dir9,
        Direction::Dir6,
        Direction::Dir3,
        Direction::Dir2,
        Direction::Dir1,
        Direction::Dir4,
        Direction::Dir7
      }) {
        if (is_xmas(lines, position, direction)) {
          ++part_a;
        }
      }

      if (is_masmas(lines, position)) {
        ++part_b;
      }
    }
  }

  return Output(part_a, part_b);
}
