#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <set>

// https://adventofcode.com/2024/day/15

namespace {

  std::vector<std::string> extract_grid(const std::vector<std::string> & lines) {
    const auto it = std::find(lines.begin(), lines.end(), "");
    return std::vector<std::string>(lines.begin(), it);
  }

  std::vector<bj::Direction> extract_directions(const std::vector<std::string> & lines) {
    auto it = lines.begin();
    while (*it != "") {
      ++it;
    }

    ++it;

    std::vector<bj::Direction> directions;
    while (it != lines.end()) {
      for (const char c : *it) {
        directions.emplace_back(bj::to_direction_from_lrtd(c, "<>^v").value());
      }
      ++it;
    }
    return directions;
  }

  bj::Position find_robot(std::vector<std::string> & grid) {
    for (size_t y = 0; y != grid.size(); ++y) {
      for (size_t x = 0; x != grid[0].size(); ++x) {
        if (grid[y][x] == '@') {
          grid[y][x] = '.';
          return bj::Position::from(x, y);
        }
      }
    }

    return { -1, -1 };
  }
  
  
  void move_robot_a(bj::Position & robot, bj::Direction direction, std::vector<std::string> & grid) {
    bj::Position current_position = robot;
    while (true) {
      current_position.move(direction);

      char current_symbol = grid[current_position.y][current_position.x];
      if (current_symbol == '.') {
        // legal move
        break;
      } else if (current_symbol == '#') {
        // Illegal move
        return;
      } else {
        // A box
      }
    }

    // Put a box here in case we encountered some
    grid[current_position.y][current_position.x] = 'O';

    // The robot current position is now free of any box because it was pushed
    // If no box was encoutnered, robot == current_position so our code is valid
    robot.move(direction);
    grid[robot.y][robot.x] = '.';
  }


  std::vector<std::string> do_part_a(std::span<const bj::Direction> directions, std::vector<std::string> grid) {
    bj::Position robot = find_robot(grid);
    for (bj::Direction direction : directions) {
      move_robot_a(robot, direction, grid);
    }
    return grid;
  }

  long long int score_grid(const std::vector<std::string> & grid) {
    long long int score = 0;
    for (size_t y = 0; y != grid.size(); ++y) {
      for (size_t x = 0; x != grid[0].size(); ++x) {
        if (grid[y][x] == 'O' || grid[y][x] == '[') {
          score += 100 * y + x;
        }
      }
    }
    return score;
  }

  std::vector<std::string> scale_up(const std::vector<std::string> & old_grid) {
    std::vector<std::string> new_grid;

    for (const std::string & line : old_grid) {
      new_grid.emplace_back();
      for (const char c : line) {
        switch (c) {
          case '#': new_grid.back() += "##"; break;
          case 'O': new_grid.back() += "[]"; break;
          case '@': new_grid.back() += "@."; break;
          case '.': 
          default:  new_grid.back() += ".."; break;
        }
      }
    }

    return new_grid;
  }

  void move_robot_b(bj::Position & robot, bj::Direction direction, std::vector<std::string> & grid) {

    static constexpr auto move_robot_left_right = [](bj::Position & robot, int dx, std::vector<std::string> & grid) {
      static constexpr auto is_allowed  = [](const bj::Position & robot, int dx, const std::vector<std::string> & grid) -> bool {
        bj::Position current_position = robot;
        while (true) {
          current_position.x += dx;

          char current_symbol = grid[current_position.y][current_position.x];
          if (current_symbol == '.') {
            // legal move
            return true;
          } else if (current_symbol == '#') {
            // Illegal move
            return false;
          } else {
            // A box
          }
        }
      };

      static constexpr auto push = [](const bj::Position & robot, int dx, std::vector<std::string> & grid) {
        char last_seen = '.';

        bj::Position current_position = robot;
        while (true) {
          current_position.x += dx;
          std::swap(last_seen, grid[current_position.y][current_position.x]);
          if (last_seen == '.') return;
        }
      };

      if (is_allowed(robot, dx, grid)) {
        push(robot, dx, grid);
        robot.x += dx;
      }
    };

    static constexpr auto move_robot_top_down = [](bj::Position & robot, int dy, std::vector<std::string> & grid) {

      static constexpr auto is_allowed = [](
        const bj::Position & robot, int dy, const std::vector<std::string> & grid,
        std::set<bj::Position> & pushed_boxes
      ) -> bool {
        std::set<bj::Position> current_positions;
        current_positions.emplace(robot);
        std::set<bj::Position> new_to_check;

        while (!current_positions.empty()) {
          for (bj::Position current_position : current_positions) {
            current_position.y += dy;

            char current_symbol = grid[current_position.y][current_position.x];
            if (current_symbol == '.') {
              // legal move, ok
            } else if (current_symbol == '#') {
              // Illegal move
              return false;
            } else if (current_symbol == '[') {
              // left side of a box
              new_to_check.emplace(current_position);
              pushed_boxes.emplace(current_position);
              current_position.x += 1;
              new_to_check.emplace(current_position);
            } else if (current_symbol == ']') {
              // right side of a box
              new_to_check.emplace(current_position);
              current_position.x -= 1;
              new_to_check.emplace(current_position);
              pushed_boxes.emplace(current_position);
            }
          }

          std::swap(current_positions, new_to_check);
          new_to_check.clear();
        }

        return true;
      };

      static constexpr auto push = [](const std::set<bj::Position> & pushed_boxes, int dy, std::vector<std::string> & grid) {
        for (const auto & box : pushed_boxes) {
          grid[box.y][box.x] = '.';
          grid[box.y][box.x + 1] = '.';
        }

        for (const auto & box : pushed_boxes) {
          grid[box.y + dy][box.x    ] = '[';
          grid[box.y + dy][box.x + 1] = ']';
        }

        return true;
      };

      std::set<bj::Position> pushed_boxes;
      if (is_allowed(robot, dy, grid, pushed_boxes)) {
        push(pushed_boxes, dy, grid);
        robot.y += dy;
      }
    };

    switch (direction) {
      case bj::Direction::Left:  move_robot_left_right(robot, -1, grid); return;
      case bj::Direction::Right: move_robot_left_right(robot, +1, grid); return;
      case bj::Direction::Top:   move_robot_top_down(robot, -1, grid); return;
      case bj::Direction::Down:  move_robot_top_down(robot, +1, grid); return;
    }
  }

  std::vector<std::string> do_part_b(std::span<const bj::Direction> directions, std::vector<std::string> grid) {
    grid = scale_up(grid);

    bj::Position robot = find_robot(grid);
    for (bj::Direction direction : directions) {
      move_robot_b(robot, direction, grid);
    }

    return grid;
  }

}

Output day_2024_15(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<std::string> grid = extract_grid(lines);
  std::vector<bj::Direction> directions = extract_directions(lines);
  
  long long int part_a = score_grid(do_part_a(directions, grid));

  long long int part_b = score_grid(do_part_b(directions, grid));

  return Output(part_a, part_b);
}
