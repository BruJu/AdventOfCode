#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <map>
#include <utility>
#include <variant>

// https://adventofcode.com/2024/day/21

// Not finished

using i64 = std::int64_t;

namespace {

struct Robot {
  bj::Position pos;
  std::vector<std::string> layout;

  Robot(std::vector<std::string> layout) {
    this->layout = layout;
    pos = find_symbol('A');
  }

  
  std::string do_sequence(std::string s) {
    std::string result;
    for (char c : s) {
      result += go_to_symbol(c);
      result += 'A';
    }
    return result;
  }

  std::string go_to_symbol(char target) {
    std::map<char, int> required_presses;
    const bj::Position target_pos = find_symbol(target);

    std::string sequence;



    while (pos.y > target_pos.y) {
      --pos.y;
      sequence += '^';
    }

    while (pos.x < target_pos.x) {
      ++pos.x;
      sequence += '>';
    }

    while (pos.y < target_pos.y) {
      ++pos.y;
      sequence += 'v';
    }

    while (pos.x > target_pos.x) {
      --pos.x;
      sequence += '<';
    }
    return sequence;
  }

  [[nodiscard]] bj::Position find_symbol(char c) const {
    for (size_t y = 0; y != layout.size(); ++y) {
      for (size_t x = 0; x != layout[y].size(); ++x) {
        if (layout[y][x] == c) {
          return bj::Position::from(x, y);
        }
      }
    }

    throw std::runtime_error("???");
  }
};


}

Output day_2024_21(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::vector<std::string> numpad;
  numpad.emplace_back("789");
  numpad.emplace_back("456");
  numpad.emplace_back("123");
  numpad.emplace_back(".0A");

  std::vector<std::string> dirpad;
  dirpad.emplace_back(" ^A");
  dirpad.emplace_back("<v>");


  Robot robot_on_numpad(numpad);
  Robot robot_on_dirpad(dirpad);
  Robot robot_2_on_dirpad(dirpad);
  Robot me(dirpad);

  int sum = 0;
  for (const std::string & line : lines) {

    std::string a = robot_on_numpad.do_sequence(line);
    std::string b = robot_on_dirpad.do_sequence(a);
    std::string d = me.do_sequence(b);

    // v<<A^>>AvA^A v<<A^>>AAv<A<A^>>AA<Av>AA^Av<A^>AA<A>Av<A<A^>>AAA<Av>A^A
    // <v<A>>^AvA^A <vA<AA>>^AAvA<^A>AAvA^A<vA>^AA<A>A<v<A>A>^    AAAvA<^A>A

    // printf("%s\n", d.c_str());
    // printf("%zu ; %d, %ld\n", d.size(), std::stoi(line), d.size() * std::stoi(line));
    sum += d.size() * std::stoi(line);
  }

  return Output(
    sum, 0
    );
}
