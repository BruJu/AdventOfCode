#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <map>
#include <set>
#include <algorithm>
#include <queue>

// https://adventofcode.com/2024/day/16

namespace {

static constexpr std::array<bj::Direction, 4> all_dirs { bj::Direction::Left, bj::Direction::Down, bj::Direction::Right, bj::Direction::Top };

std::vector<bj::Kitten> get_next_position(bj::Position position, const std::vector<std::string> & grid) {
  std::vector<bj::Kitten> left_to_explore;

  for (const auto dir : all_dirs) {
    bj::Position pos = position; pos.move(dir);
    if (grid[pos.y][pos.x] == '.' || grid[pos.y][pos.x] == 'E') {
      left_to_explore.emplace_back(pos, dir);
    }
  }

  return left_to_explore;
}

struct Path {
  bj::Kitten start;
  bj::Kitten end;
  std::set<bj::Position> my_tiles;
  int nb_turn;
  bool is_final = false;

  Path(bj::Kitten initial) : start(initial), end(initial), nb_turn(0) {
  }

  void print() {
    static auto constexpr to_dir = [](bj::Direction dir) -> const char * {
      switch (dir) {
        case bj::Direction::Down: return "v";
        case bj::Direction::Top: return "^";
        case bj::Direction::Left: return "<";
        case bj::Direction::Right: return ">";
        default: return "?";
      }
    };

    printf("Path: (%d, %d, %s) -> (%d, %d, %s) %d\n",
      start.position.x, start.position.y, to_dir(start.direction),
      end.position.x, end.position.y, to_dir(end.direction),
      nb_turn
      );
  }

  static std::multimap<bj::Position, Path> find_paths(const std::vector<std::string> & lines, bj::Position start) {
    std::multimap<bj::Position, Path> result;

    std::set<bj::Position> already_in_stack;
    std::vector<bj::Position> stack;

    already_in_stack.emplace(start);
    stack.emplace_back(start);

    while (!stack.empty()) {
      const bj::Position start = stack.back();
      stack.pop_back();

      for (const bj::Kitten kitty : get_next_position(start, lines)) {
        Path path(bj::Kitten{ start, kitty.direction });

        path.my_tiles.emplace(kitty.position);

        bj::Kitten current = kitty;

        if (lines[current.position.y][current.position.x] == 'E') {
          path.end = current;
          path.nb_turn = 0;
          path.is_final = true;
          result.emplace(path.start.position, path);
          continue;
        }

        while (true) {
          // Find next possibilities
          auto following = get_next_position(current.position, lines);

          // Remove moving backward
          auto it_previous = std::ranges::find_if(following, [&](const auto & pair) {
            return are_opposite_dirs(pair.direction, current.direction);
          });
          if (it_previous != following.end()) {
            following.erase(it_previous);
          }

          // Ok
          if (following.size() == 1) {
            // Only one next
            path.my_tiles.emplace(following[0].position);
            path.nb_turn += current.direction == following[0].direction ? 0 : 1;

            current = following[0];

            if (lines[current.position.y][current.position.x] == 'E') {
              // Reach goal
              path.is_final = true;
              path.end = current;
              result.emplace(path.start.position, path);
              break;
            } else {
              // Reach a .
              // continue
            }
          } else {
            // Multiple choices or none, this is the end of this path
            path.end = current;
            path.my_tiles.emplace(current.position);
            path.is_final = lines[current.position.y][current.position.x] == 'E';

            if (!path.is_final && following.size() == 0) {
              // cul-de-sac comme on dit en bon français formel, don't bother
            } else {
              result.emplace(path.start.position, path);
            }

            if (!path.is_final && !already_in_stack.contains(current.position) && following.size() > 0) {
              already_in_stack.emplace(current.position);
              stack.emplace_back(current.position);
            }
            break;
          }
        }
      }
    }

    return result;
  }

};

struct OutputPath {
  std::vector<const Path *> paths;
  // std::set<bj::Position> visited_intersections;
  long long int score = 0;
};



std::vector<OutputPath>
find_solutions(const std::multimap<bj::Position, Path> & paths, bj::Position starting_pos) {
  struct KittenComp {
    [[nodiscard]] bool operator()(bj::Kitten a, bj::Kitten b) const {
      if (a.position < b.position) return true;
      if (b.position < a.position) return false;
      if (int(a.direction) < int(b.direction)) return true;
      if (int(a.direction) > int(b.direction)) return false;
      return false;
    }
  };

  std::vector<OutputPath> known_solution;                  // Solutions
  std::map<bj::Kitten, int, KittenComp> smallest_to_reach; // Limit the number of explored branches

  struct ToExplore {
    OutputPath currently_explored;
    bj::Kitten my_situation;
  };
  std::queue<ToExplore> queue;
  queue.emplace(OutputPath(), bj::Kitten(starting_pos, bj::Direction::Right));
  smallest_to_reach[queue.front().my_situation] = 0;

  while (!queue.empty()) {
    auto [currently_explored, situation] = queue.front();
    queue.pop();

    // Explore?
    const auto it_str = smallest_to_reach.find(situation);
    if (it_str != smallest_to_reach.end()) {
      const int smallest_score = it_str->second;
      if (smallest_score < currently_explored.score) {
        continue;
      }
    }
    smallest_to_reach[situation] = currently_explored.score;

    // Ok, look at paths
    const auto it_next_paths = paths.equal_range(situation.position);
    
    for (auto it = it_next_paths.first; it != it_next_paths.second; ++it) {
      const Path & possible_path = it->second;

      if (possible_path.start.position != situation.position) continue;
      if (bj::are_opposite_dirs(possible_path.start.direction, situation.direction)) continue;
      
      OutputPath c_explored_it = currently_explored;
      if (possible_path.start.direction != situation.direction) {
        c_explored_it.score += 1000;
      }
      c_explored_it.paths.emplace_back(&possible_path);
      c_explored_it.score += possible_path.nb_turn * 1000 + possible_path.my_tiles.size();

      if (!known_solution.empty() && c_explored_it.score > known_solution[0].score) {
        continue;
      }

      if (possible_path.is_final) {
        if (!known_solution.empty() && c_explored_it.score < known_solution[0].score) {
          known_solution.clear();
        }

        known_solution.emplace_back(c_explored_it);
      } else {
        queue.emplace(ToExplore{ c_explored_it, possible_path.end });
      }
    }
  }
  
  return known_solution;
}

[[maybe_unused]]
void draw_solutions(const std::vector<std::string> & lines, std::span<const OutputPath> solutions) {
  int nb_crosses = 0;
  for (size_t y = 0; y != lines.size(); ++y) {
    for (size_t x = 0; x != lines[y].size(); ++x) {
      switch (lines[y][x]) {
        case '.':
          if (
            std::ranges::any_of(solutions, [&](const auto & solution) {
              return std::ranges::any_of(solution.paths, [&](const Path * path) {
                return path->my_tiles.contains(bj::Position::from(x, y));
                });
              })
          ) {
            printf("x");
            ++nb_crosses;
          } else {
            printf(".");
          }
          break;
        case 'S':
        case 'E':
        case '#':
        default:
          printf("%c", lines[y][x]);
      }
    }
    printf("\n");
  }
  printf("\n");

  printf("Nb of crosses = %d\n", nb_crosses);
  printf("Nb of solutions = %zu paths\n", solutions.size());
}

}

Output day_2024_16(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto opt_starting_pos = bj::Position::find(lines, 'S');
  const auto opt_starting_target = bj::Position::find(lines, 'E');
  if (!opt_starting_pos) return Output("invalid input", "invalid input");
  if (!opt_starting_target) return Output("invalid input", "invalid input");

  const bj::Position starting_pos = *opt_starting_pos;


  std::multimap<bj::Position, Path> paths = Path::find_paths(lines, starting_pos);

  const auto solutions = find_solutions(paths, starting_pos);

  std::set<bj::Position> place_a_seat_here;
  place_a_seat_here.emplace(starting_pos);
  for (const auto & solution : solutions) {
    for (const auto & path : solution.paths) {
      for (const auto & tile : path->my_tiles) {
        place_a_seat_here.emplace(tile);
      }
    }
  }

  return Output(place_a_seat_here.size() == 0 ? -1 : solutions[0].score, place_a_seat_here.size());
}
