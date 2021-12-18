#include "../advent_of_code.hpp"
#include <vector>
#include <optional>
#include <stack>

// https://adventofcode.com/2021/day/18

enum class SnailfishType {
  SingleValue, Embedded
};

struct Snailfish {
  struct Stacked {
    Snailfish * snail;
    Snailfish * parent;
    size_t level;
  };

  SnailfishType type;
  /* Single value */ long long int value = 0;
  /* Embedded */ std::vector<Snailfish> members;

  Snailfish() = default;

  Snailfish(const std::string & line, size_t & cursor) {
    if (line[cursor] == '[') {
      type = SnailfishType::Embedded;
      ++cursor;
      members.push_back(Snailfish(line, cursor));
      if (line[cursor] != ',') std::cout << "Error on processing: expected ,\n";

      ++cursor;
      members.push_back(Snailfish(line, cursor));

      if (line[cursor] != ']') std::cout << "Error on processing: expected ]\n";
      ++cursor;
    } else {
      type = SnailfishType::SingleValue;
      value = 0;

      while (line[cursor] >= '0' && line[cursor] <= '9') {
        value = value * 10 + (line[cursor] - '0');
        ++cursor;
      }
    }
  }

  Snailfish(const Snailfish & lhs, const Snailfish & rhs) {
    // TODO: use move semantic if it is too slow
    type = SnailfishType::Embedded;
    members.emplace_back(lhs);
    members.emplace_back(rhs);

    while (true) {
      // explode
      std::vector<Stacked> stacked;
      push(stacked, this);

      auto it = std::find_if(
        stacked.begin(), stacked.end(),
        [](const Stacked & s) {
          return s.level >= 4;
        }
      );

      if (it == stacked.end()) return;

      if (it != stacked.begin()) {
        (it - 1)->snail->value += it->snail->value;
      }

      if ((it + 2) != stacked.end()) {
        (it + 2)->snail->value += (it + 1)->snail->value;
      }

      it->snail->value = 0;
      (it + 1)->snail->value = 0;

      it->parent->type = SnailfishType::SingleValue;
      it->parent->value = 0;
      it->parent->members.clear();

      // Split

      split();
    }
  }

  void split() {
    if (type == SnailfishType::Embedded) {
      members[0].split();
      members[1].split();
    } else {
      if (value >= 10) {
        members.emplace_back();
        members[0].type = SnailfishType::SingleValue;
        members[0].value = value / 2;

        members.emplace_back();
        members[0].type = SnailfishType::SingleValue;
        members[0].value = (value / 2) + (value % 2);
      }
    }
  }

  void push(std::vector<Stacked> & s, Snailfish * parent, size_t level = 0) {
    if (type == SnailfishType::SingleValue) {
      s.push_back(Stacked{ this, parent, level });
    } else {
      members[0].push(s, this, level + 1);
      members[1].push(s, this, level + 1);
    }
  }


  long long int & get(const std::string & pos, size_t i = 0) {
    if (type == SnailfishType::SingleValue) return value;

    if (pos[i] == 'L') return members[0].get(pos, i + 1);
    return members[1].get(pos, i + 1);
  }




  [[nodiscard]] long long int magnitude() const {
    if (type == SnailfishType::SingleValue) {
      return value;
    } else {
      return members[0].magnitude() * 3 + members[1].magnitude() * 2;
    }
  }


};

Output day_2021_18(const std::vector<std::string> & lines, const DayExtraInfo &) {

  std::vector<Snailfish> snails;
  for (const auto & line : lines) {
    size_t x = 0;
    snails.emplace_back(line, x);
  }

  while (snails.size() != 1) {
    const auto new_snail = Snailfish(snails[0], snails[1]);
    snails.erase(snails.begin() + 1);
    snails[0] = new_snail;
  }

  return Output(snails[0].magnitude(), 0);
}
