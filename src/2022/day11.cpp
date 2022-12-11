#include "../advent_of_code.hpp"
#include <span>
#include <algorithm>
#include <queue>
#include <exception>
#include <optional>
#include <ranges>
#include <cstring>

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

struct MonkeyBuilder;

class Monkey {
private:
  size_t id;
  std::queue<unsigned long long int> held_items;
  ItemTransformer transformer;
  Item dividable_by;
  size_t target_monkey_if_true;
  size_t target_monkey_if_false;
  size_t seen_items = 0;

  Monkey() = default;
public:
  static std::optional<Monkey> next_monkey(MonkeyBuilder & builder);

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

struct MonkeyBuilder {
  const std::vector<std::string> * lines;
  size_t i = 0;

  bool go_to_next_non_empty_line() {
    while (i < lines->size() && (*lines)[i] == "") ++i;

    return i != lines->size();
  }

  std::string check_and_trim(const char * prefix) {
    if (i == lines->size()) throw std::runtime_error("bad input (eof)");
    const std::string & line = (*lines)[i];
    ++i;
    const size_t expected_size = std::strlen(prefix);
    if (line.size() < expected_size) throw std::runtime_error("bad input");
    if (line.substr(0, expected_size) != prefix) {
      throw std::runtime_error("bad input");
    }
    return line.substr(expected_size);
  }
};

std::optional<Monkey> Monkey::next_monkey(MonkeyBuilder & builder) {
  // IF ONLY THERE WAS SOME KIND OF UNIVERSAL FORMAT THAT EVERY APIs
  // IN THIS WORLD WAS USING. MAYBE WE COULD USE IT INSTEAD OF HAVING PARSING
  // FUNCTIONS THAT ARE WAY DAMN TOO LONG.
  if (!builder.go_to_next_non_empty_line()) return std::nullopt;

  Monkey monkey;

  {
    const std::string monkey_id_line = builder.check_and_trim("Monkey ");
    monkey.id = std::stoul(monkey_id_line.substr(0, monkey_id_line.size() - 1));
  }

  {
    std::string items_line = builder.check_and_trim("  Starting items: ");
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
      monkey.held_items.emplace(item);
    }
  }

  {
    std::string operation = builder.check_and_trim("  Operation: new = old ");
    const Operator operator_ = operation[0] == '+' ? Operator::Plus : Operator::Times;
    std::optional<Item> value;
    
    if (operation.size() == 5 && operation.substr(2, 3) == "old") {
      value = std::nullopt; // I like writing useless lines so I can write useless comments
    } else{
      value = std::stoul(operation.substr(2));
    }

    monkey.transformer = ItemTransformer{ operator_, value };
  }

  {
    std::string line = builder.check_and_trim("  Test: divisible by ");
    monkey.dividable_by = std::stoul(line);
  }

  {
    std::string line = builder.check_and_trim("    If true: throw to monkey ");
    monkey.target_monkey_if_true = std::stoul(line);
  }

  {
    std::string line = builder.check_and_trim("    If false: throw to monkey ");
    monkey.target_monkey_if_false = std::stoul(line);
  }

  return monkey;
}

Output day_2022_11(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<Monkey> monkeys = [&]() {
    MonkeyBuilder monkey_builder{ &lines, 0 };

    std::vector<Monkey> monkeys;
    while (auto new_monkey = Monkey::next_monkey(monkey_builder)) {
      monkeys.emplace_back(std::move(new_monkey.value()));
    }
    return monkeys;
  }();

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
