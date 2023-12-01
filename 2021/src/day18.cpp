#include "../../common/advent_of_code.hpp"
#include <vector>
#include <optional>
#include <stack>
#include <ostream>

// https://adventofcode.com/2021/day/18

enum class SnailfishType { SingleValue, Embedded };

struct Snailfish;

/** A snail with its level in the tree and is parent */
struct LinedSnail {
  Snailfish * snail; /* Must be SingleValue */
  Snailfish * parent; /* Must be embedded */
  size_t level;
};

/** The list of all snails with a single value */
struct LinedSnailCombiner {
  std::vector<LinedSnail> m_values;

  explicit LinedSnailCombiner(Snailfish & snail);

  void add_value(Snailfish * self, Snailfish * parent, size_t level) {
    m_values.emplace_back(LinedSnail{ self, parent, level });
  }

  bool explode();
  bool split();
};

struct Snailfish {
  SnailfishType type;
  /* Single value */ long long int value = 0;
  /* Embedded */ std::vector<Snailfish> members;

  explicit Snailfish(long long int value)
  : type(SnailfishType::SingleValue), value(value) {}

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

private:
  Snailfish(const Snailfish & lhs, const Snailfish & rhs) {
    type = SnailfishType::Embedded;
    members.emplace_back(lhs);
    members.emplace_back(rhs);

    while (true) {
      LinedSnailCombiner combiner(*this);
      if (combiner.explode()) continue;
      if (combiner.split()) continue;
      break;
    }
  }

public:
  Snailfish operator+(const Snailfish & other) const {
    return Snailfish(*this, other);
  }


  void push(LinedSnailCombiner & s, Snailfish * parent, size_t level = 0) {
    if (type == SnailfishType::SingleValue) {
      s.add_value(this, parent, level);
    } else {
      members[0].push(s, this, level + 1);
      members[1].push(s, this, level + 1);
    }
  }

  [[nodiscard]] long long int magnitude() const {
    if (type == SnailfishType::SingleValue) {
      return value;
    } else {
      return members[0].magnitude() * 3 + members[1].magnitude() * 2;
    }
  }
};


////////////////////////////////////////////////////////////////////////////////
// === COMBINE TWO SNAILS

LinedSnailCombiner::LinedSnailCombiner(Snailfish & snail) {
  snail.push(*this, nullptr, 0);
}

bool LinedSnailCombiner::explode() {
  auto it = std::find_if(
    m_values.begin(), m_values.end(),
    [](const LinedSnail & s) { return s.level >= 5; }
  );

  if (it == m_values.end()) return false;

  // Stack values
  if (it != m_values.begin()) (it - 1)->snail->value += it->snail->value;
  if ((it + 2) != m_values.end()) (it + 2)->snail->value += (it + 1)->snail->value;

  // Set to zero
  it->parent->type = SnailfishType::SingleValue;
  it->parent->value = 0;
  it->parent->members.clear();

  return true;
}

bool LinedSnailCombiner::split() {
  for (auto & value : m_values) {
    if (value.snail->value >= 10) {
      value.snail->type = SnailfishType::Embedded;
      value.snail->members.clear();
      value.snail->members.emplace_back(value.snail->value / 2);
      value.snail->members.emplace_back(value.snail->value / 2 + value.snail->value % 2);
      return true;
    }
  }

  return false;
}


////////////////////////////////////////////////////////////////////////////////

Output day_2021_18(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::vector<Snailfish> snails;
  for (const auto & line : lines) {
    size_t x = 0;
    snails.emplace_back(line, x);
  }

  const long long int part_a = [](std::vector<Snailfish> snails) {
    while (snails.size() != 1) {
      const auto new_snail = snails[0] + snails[1];
      snails.erase(snails.begin() + 1);
      snails[0] = new_snail;
    }

    return snails[0].magnitude();
  }(snails);

  const long long int part_b = dei.can_skip_part_B ? 0 : [](const std::vector<Snailfish> & snails) {
    long long int max = 0;

    for (size_t i = 0; i != snails.size(); ++i) {
      for (size_t j = 0; j != snails.size(); ++j) {
        if (i == j) continue;

        const auto new_snail = snails[i] + snails[j];

        const auto magnitude = new_snail.magnitude();
        if (max < magnitude) max = magnitude;
      }
    }

    return max;
  }(snails);

  return Output(part_a, part_b);
}
