#include "../advent_of_code.hpp"
#include <algorithm>
#include <cstring>
#include <exception>
#include <optional>
#include <queue>
#include <regex>
#include <span>

// https://adventofcode.com/2022/day/11

using Item = unsigned long long int;

enum class Operator { Plus, Times };

struct ItemTransformer {
private:
  Operator operator_;
  std::optional<Item> rhs;

public:
  ItemTransformer() = default;
  ItemTransformer(Operator operator_, std::optional<Item> other)
    : operator_(operator_), rhs(other) {}

  [[nodiscard]] Item apply(const Item item) const noexcept {
    if (operator_ == Operator::Plus) {
      if (rhs.has_value()) {
        return item + rhs.value();
      } else {
        return item + item;
      }
    } else {
      if (rhs.has_value()) {
        return item * rhs.value();
      } else {
        return item * item;
      }
    }
  }

  friend std::ostream & operator<<(std::ostream & stream, ItemTransformer self) {
    if (self.operator_ == Operator::Plus) {
      stream << '+';
    } else {
      stream << '*';
    }
    if (self.rhs.has_value()) {
      stream << self.rhs.value();
    } else{
      stream << "self";
    }
    return stream;
  }
};

class Monkey {
private:
  size_t id;
  std::queue<unsigned long long int> held_items;
  ItemTransformer transformer;
  Item dividable_by;
  size_t target_monkey_if_true;
  size_t target_monkey_if_false;
  size_t seen_items = 0;
  
  explicit Monkey(std::smatch regex_match);

public:
  [[nodiscard]] static std::vector<Monkey> read_monkeys(const std::vector<std::string> & lines);

  static unsigned long long int get_monkey_business(std::span<const Monkey> monkeys) {
    std::vector<unsigned long long int> values;
    for (const Monkey & monkey : monkeys) {
      values.emplace_back(monkey.seen_items);
    }

    std::partial_sort(
      values.begin(), values.begin() + 2, values.end(),
      std::greater<unsigned long long int>{}
    );

    return values[0] * values[1];
  }

  void run_round(std::span<Monkey> monkeys, std::optional<Item> worry_strategy) {
    while (!held_items.empty()) {
      ++seen_items;

      Item item = held_items.front();
      held_items.pop();

      item = transformer.apply(item);
      if (!worry_strategy) { // Part A
        item /= 3;
      } else { // Part B
        item %= worry_strategy.value();
      }

      Monkey & target = monkeys[
        ((item % dividable_by) == 0) ?
        target_monkey_if_true :
        target_monkey_if_false
      ];

      target.held_items.emplace(item);
    }
  }

  static Item compute_worry_strategy(std::span<const Monkey> monkeys) {
    Item worry_divider = 1;
    for (const Monkey & monkey : monkeys) {
      worry_divider *= monkey.dividable_by;
    }

    return worry_divider;
  }

  friend std::ostream & operator<<(std::ostream & stream, const Monkey & self) {
    stream << "Monkey(id=" << self.id
    << ", op=" << self.transformer
    << ", div(" << self.dividable_by << ", " << self.target_monkey_if_true << ", " << self.target_monkey_if_false << ")"
    << ", items=";

    auto copy = self.held_items;
    while (!copy.empty()) {
      stream << ' ' << copy.front();
      copy.pop();
    }

    return stream << ")";
  }
};

std::vector<Monkey> Monkey::read_monkeys(const std::vector<std::string> & lines) {
  const std::regex regex_object = std::regex(
    R"(Monkey (\d+):\n  Starting items: (.*)\n  Operation: new = old (\*|\+) (\d+|old)\n  Test: divisible by (\d+)\n    If true: throw to monkey (\d+)\n    If false: throw to monkey (\d+))"
  );
  std::smatch smatch_;

  std::vector<Monkey> retval;

  size_t i = 0;

  while (true) {
    while (i < lines.size() && lines[i] == "") ++i;

    if (i == lines.size()) break;

    const std::string to_parse =
      lines[i] + '\n' + lines[i + 1] + '\n' +
      lines[i + 2] + '\n' + lines[i + 3] + '\n' +
      lines[i + 4] + '\n' + lines[i + 5];
    
    i += 6;

    if (!std::regex_search(to_parse, smatch_, regex_object)) {
      std::cerr << to_parse << '\n';
      throw std::runtime_error("Bad input");
    }

    retval.push_back(Monkey(smatch_));
  }

  return retval;
}

Monkey::Monkey(std::smatch regex_match)
  : id(std::stoul(regex_match[1].str()))
  , dividable_by(std::stoul(regex_match[5].str()))
  , target_monkey_if_true(std::stoul(regex_match[6].str()))
  , target_monkey_if_false(std::stoul(regex_match[7].str()))
{

  {
    std::string items_line = regex_match[2].str();
    auto it = items_line.begin();
    while (it != items_line.end()) {
      if (*it == ',') it = items_line.erase(it);
      else ++it;
    }

    const auto items_int = StringSplitter::line_to_vector<Item>(items_line,
      [](std::string s) { return std::stoul(s); },
      ' '
    );

    for (const Item item : items_int) {
      held_items.emplace(item);
    }
  }
  
  {
    const Operator operator_ = regex_match[3].str()[0] == '+' ? Operator::Plus : Operator::Times;
    std::optional<Item> value;

    std::string value_str = regex_match[4].str();
    
    if (value_str == "old") {
      value = std::nullopt; // I like writing useless lines so I can write useless comments
    } else{
      value = std::stoul(value_str);
    }

    transformer = ItemTransformer{ operator_, value };
  }
}

Output day_2022_11(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::vector<Monkey> monkeys = Monkey::read_monkeys(lines);

  auto part_a = monkeys;
  for (size_t round = 0; round != 20; ++round) {
    for (Monkey & monkey : part_a) {
      monkey.run_round(part_a, std::nullopt);
    }
  }
  
  auto part_b = monkeys;
  const Item worry_strategy = Monkey::compute_worry_strategy(part_b);
  for (size_t round = 0; round != 10000; ++round) {
    for (Monkey & monkey : part_b) {
      monkey.run_round(part_b, worry_strategy);
    }
  }

  return Output(
    Monkey::get_monkey_business(part_a),
    Monkey::get_monkey_business(part_b)
  );
}
