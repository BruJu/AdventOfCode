#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <map>
#include <utility>
#include <variant>
#include <set>

// https://adventofcode.com/2024/day/21

// Not finished

using i64 = std::int64_t;

namespace {


struct Robot {
  bj::Position pos;
  Robot * piloted_by;

  struct Pathway {
    std::vector<std::string> description_of_paths;
    std::optional<size_t> minimal_size_of_input; // for memoization
    bj::Position end_position;
  };

  // [starting position, destination character] -> [legal sequences, fianl position]
  // In legal sequences, we forbid alternating between inputs. i.e. ^<^< is not legit but ^^<< and <<^^ are
  std::map<std::pair<bj::Position, char>, Pathway> paths;

  Robot(std::vector<std::string> layout, Robot * piloted_by) : piloted_by(piloted_by) {
    const auto [symbols, forbidden_positions] = locate_all_symbols(layout);

    const auto it_pos_a = symbols.find('A');
    if (it_pos_a == symbols.end()) throw std::runtime_error("202421 no A in input");
    pos = it_pos_a->second;

    const auto bvp_result = build_valid_paths(symbols, forbidden_positions);

    for (const auto & [key, value] : bvp_result) {
      paths[key] = Pathway {
        .description_of_paths = value.first,
        .minimal_size_of_input = std::nullopt,
        .end_position = value.second
      };
    }
  }


  struct LocateAllSymbolsResult {
    std::map<char, bj::Position> symbols;
    std::set<bj::Position> forbidden_positions;
  };

  [[nodiscard]]
  static
  LocateAllSymbolsResult locate_all_symbols(const std::vector<std::string> & layout) {
    std::map<char, bj::Position> symbols;
    std::set<bj::Position> forbidden_positions;
    
    for (int y = 0; std::cmp_not_equal(y, layout.size()); ++y) {
      for (int x = 0; std::cmp_not_equal(x, layout[y].size()); ++x) {
        const char c = layout[y][x];

        if (c == ' ') {
          forbidden_positions.emplace(bj::Position{ x, y });
        } else if (!symbols.contains(c)) {
          symbols.emplace(c, bj::Position{ x, y });
        } else {
          throw std::runtime_error("202421: No duplicate keys are allowed on keyboards");
        }
      }
    }

    return { symbols, forbidden_positions };
  }


  [[nodiscard]]
  static
  bool is_valid(
    bj::Position start, std::string_view path,
    const std::set<bj::Position> & banned_positions
  ) {
    bj::Kitten hello_kitty(start, bj::Direction::Top);

    for (char c : path) {
      hello_kitty.direction = bj::to_direction_from_lrtd(c, "<>^v").value();
      hello_kitty.advance();
      if (banned_positions.contains(hello_kitty.position)) {
        return false;
      }
    }

    return true;
  }

  [[nodiscard]]
  static
  std::map<
    std::pair<bj::Position, char>,
    std::pair<std::vector<std::string>, bj::Position>
  >
  build_valid_paths(
    std::map<char, bj::Position> symbols,
    std::set<bj::Position> forbidden_positions
  ) {
    std::map<
      std::pair<bj::Position, char>,
      std::pair<std::vector<std::string>, bj::Position>
    > output;

    for (const auto & [starting_char, starting_pos] : symbols) {
      for (const auto & [ending_char, ending_pos] : symbols) {
        // Find key presses
        const int diff_x = ending_pos.x - starting_pos.x;
        const int diff_y = ending_pos.y - starting_pos.y;

        const char char_dx = diff_x > 0 ? '>' : '<';
        const char char_dy = diff_y > 0 ? 'v' : '^';
        
        const char int_dx = diff_x > 0 ? +1 : -1;
        const char int_dy = diff_y > 0 ? +1 : -1;

        std::string candidate_path_1;
        std::string candidate_path_2;
        for (int x = starting_pos.x; x != ending_pos.x; x += int_dx) {
          candidate_path_1 += char_dx;
        }
        for (int y = starting_pos.y; y != ending_pos.y; y += int_dy) {
          candidate_path_1 += char_dy;
          candidate_path_2 += char_dy;
        }
        for (int x = starting_pos.x; x != ending_pos.x; x += int_dx) {
          candidate_path_2 += char_dx;
        }

        std::vector<std::string> paths;

        if (candidate_path_1.size() == 0) {
          // what a convoluted way to tell that the starting_char and the ending_char are the same
          paths.emplace_back("");
        } else {
          if (is_valid(starting_pos, candidate_path_1, forbidden_positions)) {
            paths.emplace_back(candidate_path_1);
          }

          if (candidate_path_1 != candidate_path_2 && is_valid(starting_pos, candidate_path_2, forbidden_positions)) {
            paths.emplace_back(candidate_path_2);
          }
        }

        if (paths.size() == 0) {
          throw std::runtime_error("202421 A symbol is not accessible from another symbol");
        }

        // You must end the sequence with an A press
        for (auto & path : paths) {
          path += 'A';
        }

        output[std::pair(starting_pos, ending_char)] = std::pair(paths, ending_pos);
      }
    }


    return output;
  }
  
  size_t press(std::string sequence) {
    if (piloted_by == nullptr) {
      // Biological robot (aka me): just press the buttons
      return sequence.size();
    }

    // We assume we are on an A
    if (sequence.back() != 'A') throw std::runtime_error("Must finish with an A press");

    size_t full_sequence = 0;
    for (const char c : sequence) {
      const auto it_possible_paths = paths.find(std::pair(pos, c));
      if (it_possible_paths == paths.end()) throw std::runtime_error("No path found");

      bj::Position new_pos = it_possible_paths->second.end_position;

      std::optional<size_t> shortest_sequence;

      if (it_possible_paths->second.minimal_size_of_input) {
        // already known (memoization)
        shortest_sequence = *it_possible_paths->second.minimal_size_of_input;
      } else {
        // piloted_by is on A because all ends with A and we start at A
        for (const std::string & possible_path : it_possible_paths->second.description_of_paths) {
          size_t res = piloted_by->press(possible_path);
          if (!shortest_sequence || *shortest_sequence > res) {
            shortest_sequence = res;
          }
        }
        // piloted_by is on A because ends on an A

        it_possible_paths->second.minimal_size_of_input = *shortest_sequence;
      }

      if (!shortest_sequence) {
        throw std::runtime_error("No path found"); // how? bc no paths
      }

      full_sequence += *shortest_sequence;

      pos = new_pos;
    }
    return full_sequence;
  }
};


}

Output day_2024_21(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::vector<std::string> numpad;
  numpad.emplace_back("789");
  numpad.emplace_back("456");
  numpad.emplace_back("123");
  numpad.emplace_back(" 0A");

  std::vector<std::string> dirpad;
  dirpad.emplace_back(" ^A");
  dirpad.emplace_back("<v>");


  Robot me               (dirpad, nullptr);
  Robot robot_1_on_dirpad(dirpad, &me);
  Robot robot_2_on_dirpad(dirpad, &robot_1_on_dirpad);
  Robot robot_on_numpad  (numpad, &robot_2_on_dirpad);

  std::vector<std::unique_ptr<Robot>> the_26_robots_and_me;
  // One directional keypad that you are using.
  the_26_robots_and_me.emplace_back(new Robot(dirpad, nullptr));
  // 25 directional keypads that robots are using.
  for (int i = 0; i != 25; ++i) {
    Robot * previous = the_26_robots_and_me.back().get();
    the_26_robots_and_me.emplace_back(new Robot(dirpad, previous));
  }
  // One numeric keypad (on a door) that a robot is using.
  the_26_robots_and_me.emplace_back(new Robot(numpad, the_26_robots_and_me.back().get()));

  int sum = 0;
  std::int64_t part_b_sum = 0;
  for (const std::string & line : lines) {
    size_t pressed = robot_on_numpad.press(line);
    sum += pressed * std::stoi(line);

    if (!dei.can_skip_part_B) {
      size_t pressed_b = the_26_robots_and_me.back()->press(line);
      part_b_sum += pressed_b * std::stoi(line);
    }
  }

  return Output(sum, part_b_sum);
}
