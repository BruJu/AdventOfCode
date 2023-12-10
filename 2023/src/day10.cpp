#include "../../common/advent_of_code.hpp"
#include <vector>
#include <span>
#include <algorithm>
#include <set>
#include <map>
#include <stack>
#include "../../util/position.hpp"

#pragma GCC diagnostic ignored "-Wsign-compare"

using i64 = std::int64_t;

// https://adventofcode.com/2023/day/10


using Maze = std::vector<std::string>;

struct SupportedTiles {
  std::map<char, std::pair<bj::Direction, bj::Direction>> tiles;

  SupportedTiles() {
    add_tile('|', bj::Direction::Top, bj::Direction::Down);
    add_tile('-', bj::Direction::Left, bj::Direction::Right);
    add_tile('L', bj::Direction::Top, bj::Direction::Right);
    add_tile('J', bj::Direction::Top, bj::Direction::Left);
    add_tile('7', bj::Direction::Down, bj::Direction::Left);
    add_tile('F', bj::Direction::Down, bj::Direction::Right);
  }

  void add_tile(char c, bj::Direction first, bj::Direction second) {
    tiles[c] = std::pair(first, second);
  }

  [[nodiscard]] std::optional<bj::Direction>
  get_next_direction(char symbol, bj::Direction current_direction) const {
    const auto it = tiles.find(symbol);
    if (it == tiles.end()) return std::nullopt;

    if (current_direction == it->second.first) {
      return it->second.second;
    } else if (current_direction == it->second.second) {
      return it->second.first;
    } else {
      return std::nullopt;
    }
  }

  [[nodiscard]] bool has_dir(char symbol, bj::Direction d) const {
    const auto it = tiles.find(symbol);
    if (it == tiles.end()) return false;
    return it->second.first == d || it->second.second == d;
  }
};

static SupportedTiles tiles;

bool advance_(bj::Kitten & kitten, char symbol) {
  kitten.turn_180();

  const auto next_dir = tiles.get_next_direction(symbol, kitten.direction);
  if (!next_dir) return false;

  kitten.direction = *next_dir;
  kitten.advance();

  return true;
}

bool in_bound(const Maze & maze, bj::Position pos) {
  if (pos.x < 0 || pos.y < 0) return false;
  if (pos.y >= maze.size()) return false;
  if (pos.x >= maze[0].size()) return false;
  return true;
}

struct PartA {
  bj::Position starting_position;
  bj::Direction starting_direction;
  Maze maze_with_valid_start_pos;
  i64 answer;

  static PartA solve(const Maze & lines) {
    std::vector<std::string> maze = lines;
    bj::Position start = PartA::find_start(maze);
    
    bj::Direction base_dir;

    int part_a = -1;
    for (const auto & [possible_pipe, dirs] : tiles.tiles) {
      maze[start.y][start.x] = possible_pipe;

      bj::Kitten k { start, dirs.first };
      base_dir = dirs.first;
      k.turn_180();

      int steps = 0;
      do {
        if (!in_bound(maze, k.position)) break;
        if (!advance_(k, maze[k.position.y][k.position.x])) {
          break;
        }
        ++steps;
      } while (k.position != start);

      if (k.position == start && advance_(k, maze[k.position.y][k.position.x])) {
        part_a = (steps + 1) / 2;
        break;
      }
    }
    
    return PartA {
      .starting_position = start,
      .starting_direction = base_dir,
      .maze_with_valid_start_pos = maze,
      .answer = i64(part_a)
    };
  }

  static bj::Position find_start(const Maze & maze) {
    for (int y = 0; y != maze.size(); ++y) {
      for (int x = 0; x != maze[0].size(); ++x) {
        if (maze[y][x] == 'S') {
          return bj::Position{ x, y };
        }
      }
    }

    throw std::runtime_error("Bad input in 2023 10");
  }
};

struct PartB {
  /// Produce maze without pipes not in the loop
  static Maze clean_maze(const Maze & maze, bj::Kitten kitten) {
    // Produce empty maze
    Maze cleaned = maze;

    for (int y = 0; y != cleaned.size(); ++y) {
      for (int x = 0; x != cleaned[0].size(); ++x) {
        cleaned[y][x] = '.';
      }
    }

    // Copy the maze
    bj::Position start = kitten.position;
    do {
      char c = maze[kitten.position.y][kitten.position.x];
      cleaned[kitten.position.y][kitten.position.x] = c;
      advance_(kitten, c);
    } while (kitten.position != start);

    return cleaned;
  }

  /// *2 the maze
  static Maze zoom(const Maze & maze) {
    Maze bigger_version;

    for (int y = 0; y != maze.size(); ++y) {
      bigger_version.emplace_back();
      std::string after;
      for (int x = 0; x != maze[0].size(); ++x) {
        char c = maze[y][x];
        bigger_version.back() += c;
        if (tiles.has_dir(c, bj::Direction::Right)) {
          bigger_version.back() += '-';
        } else {
          bigger_version.back() += '.';
        }

        if (tiles.has_dir(c, bj::Direction::Down)) {
          after += '|';
        } else {
          after += '.';
        }
        after += '.';
      }

      bigger_version.push_back(after);
    }

    return bigger_version;
  }

  /// Put # symbols outside of the maze
  static void fill(Maze & maze) {
    std::stack<bj::Position> to_evaluate;

    for (int y = 0; y != maze.size(); ++y) {
      to_evaluate.push(bj::Position{ 0, y });
      to_evaluate.push(bj::Position{ int(maze[0].size()) - 1, y });
    }

    for (int x = 0; x != maze[0].size(); ++x) {
      to_evaluate.push(bj::Position{ x, 0 });
      to_evaluate.push(bj::Position{ x, int(maze.size()) - 1 });
    }

    while (!to_evaluate.empty()) {
      bj::Position n = to_evaluate.top();
      to_evaluate.pop();

      if (maze[n.y][n.x] != '.') continue;
      
      maze[n.y][n.x] = '#';

      n.for_each_neighbour([&](bj::Position next) {
        if (!in_bound(maze, next)) return;

        if (maze[next.y][next.x] == '.') {
          to_evaluate.push(next);
        }
      });
    }
  }
};

Output day_2023_10(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  const PartA part_a_solution = PartA::solve(lines);
  
  if (dei.can_skip_part_B)
    return Output(part_a_solution.answer, 0);

  bj::Kitten k {
    part_a_solution.starting_position,
    part_a_solution.starting_direction
    };
  k.turn_180();

  const Maze cleaned_maze = PartB::clean_maze(part_a_solution.maze_with_valid_start_pos, k);

  Maze zoom = PartB::zoom(cleaned_maze);

  PartB::fill(zoom);

  // Count not filled tiles... in an unzoomed context
  i64 part_b = 0;
  for (int y = 0; y != zoom.size(); ++y) {
    if (y % 2 == 1) continue;
    for (int x = 0; x != zoom[0].size(); ++x) {
      if (x % 2 == 1) continue;

      if (zoom[y][x] == '.') {
        ++part_b;
      }
    }
  }

  return Output(part_a_solution.answer, part_b);
}
