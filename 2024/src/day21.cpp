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

template<typename T>
T unwrap(std::optional<T> value, std::string message) {
  if (!value) throw std::runtime_error(message);
  return *value;
}

// This class is a robot that moves on a keypad.
// It knows where it is, how it has to move to reach each key.
// What do we want to ask to the robot?
// -> I have a keyboard with <^v>A, what do I press if i want to input this?
//    So I have to press some <^v> to reach my letter, then press A
//
// Ok, now if I stack robots, why are some orders better?
// Is <A<A^A better than <A^A<A? What does it mean?
struct Robot {
  bj::Position pos;
  Robot * piloted_by;

  // [starting position, destination character] -> [legal sequences, fianl position]
  // In legal sequences, we forbid alternating between inputs. i.e. ^<^< is not legit but ^^<< and <<^^ are
  std::map<std::pair<bj::Position, char>, std::pair<std::vector<std::string>, bj::Position>> paths;

  Robot(std::vector<std::string> layout, Robot * piloted_by) : piloted_by(piloted_by) {
    const auto [symbols, forbidden_positions] = locate_all_symbols(layout);

    const auto it_pos_a = symbols.find('A');
    if (it_pos_a == symbols.end()) throw std::runtime_error("202421 no A in input");
    pos = it_pos_a->second;

    paths = build_valid_paths(symbols, forbidden_positions);
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
  
  std::vector<const std::vector<std::string> *> do_sequence(std::string_view s) {
    std::vector<const std::vector<std::string> *> valid_sequences;
    valid_sequences.reserve(s.size());

    for (char c : s) {
      auto it = paths.find({ this->pos, c });
      if (it == paths.end()) throw std::runtime_error("paniku");

      valid_sequences.emplace_back(&it->second.first);
      this->pos = it->second.second;

      // Must press A?
    }

    return valid_sequences;
  }

  std::string go_to_symbol(char target) {
    std::map<char, int> required_presses;
    // const bj::Position target_pos = find_symbol(target);

    std::string sequence;



    return sequence;
  }

  std::string press(std::string sequence) {
    if (piloted_by == nullptr) {
      // Yeah I can do it. No need to look for complicated things
      return sequence;
    }

    // HOW I DO DIS???
    if (sequence.back() != 'A') throw std::runtime_error("Must finish with an A press");

    std::string full_sequence;
    for (const char c : sequence) {
      const auto it_possible_paths = paths.find(std::pair(pos, c));
      if (it_possible_paths == paths.end()) throw std::runtime_error("No path found");

      bj::Position new_pos = it_possible_paths->second.second;

      std::optional<std::string> shortest_sequence;
      for (const std::string & possible_path : it_possible_paths->second.first) {
        std::string res = piloted_by->press(possible_path);
        if (!shortest_sequence || shortest_sequence->size() > res.size()) {
          shortest_sequence = res;
        }
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
  
  
  
  
  

  int sum = 0;
  for (const std::string & line : lines) {
    std::string pressed = robot_on_numpad.press(line);

    // auto a = robot_on_numpad.do_sequence(line);
    // auto b = robot_on_dirpad.do_sequence(a);
    // std::string d = me.do_sequence(b);
    // std::string d;

    // v<<A^>>AvA^A v<<A^>>AAv<A<A^>>AA<Av>AA^Av<A^>AA<A>Av<A<A^>>AAA<Av>A^A
    // <v<A>>^AvA^A <vA<AA>>^AAvA<^A>AAvA^A<vA>^AA<A>A<v<A>A>^    AAAvA<^A>A

    // printf("%s\n", d.c_str());
    // printf("%zu ; %d, %ld\n", d.size(), std::stoi(line), d.size() * std::stoi(line));
    sum += pressed.size() * std::stoi(line);
  }

  return Output(
    sum, 0
    );
}
