#include "../../common/advent_of_code.hpp"
#include <algorithm>
#include <map>
#include <vector>

// https://adventofcode.com/2022/day/21

namespace {

struct Monkey {
  std::optional<long long int> value;

  // Relevant only if value.has_value() == false
  Monkey * lhs;
  Monkey * rhs;
  char operation;
  
  [[nodiscard]] long long int what_does_the_monkey_say() const;
  [[nodiscard]] size_t count(const Monkey * monkey) const;
};

long long int Monkey::what_does_the_monkey_say() const {
  if (value) return value.value();

  const long long int lhs_val = lhs->what_does_the_monkey_say();
  const long long int rhs_val = rhs->what_does_the_monkey_say();

  switch (operation) {
    case '+': return lhs_val + rhs_val;
    case '-': return lhs_val - rhs_val;
    case '*': return lhs_val * rhs_val;
    case '/': return lhs_val / rhs_val;
    default:  throw std::runtime_error("Invalid monkey");
  }
}

size_t Monkey::count(const Monkey * monkey) const {
  if (this == monkey) return 1;
  if (value) return 0;

  return lhs->count(monkey) + rhs->count(monkey);
}


[[nodiscard]] char sanitize_operation(char operation) {
  if (operation != '+' && operation != '-'
    && operation != '*' && operation != '/') {
    std::cerr << "Operation= " << operation << "\n";
    throw std::runtime_error("Invalid operation found");
  }

  return operation;
}

class Monkeys {
private:
  std::vector<Monkey> monkeys;
  std::map<std::string, Monkey *> names;

public:
  explicit Monkeys(const std::vector<std::string> & lines);

  [[nodiscard]] Monkey * get_monkey(const std::string & name) {
    const auto it = names.find(name);
    if (it != names.end()) return it->second;
    if (monkeys.size() == monkeys.capacity()) {
      throw std::runtime_error("More monkeys that lines");
    }
    Monkey * ptr = &monkeys.emplace_back();
    names.emplace(name, ptr);
    return ptr;
  }
};

Monkeys::Monkeys(const std::vector<std::string> & lines) {
  monkeys.reserve(lines.size());

  for (const std::string & line : lines) {
    const auto splitted = bj::string_split(line, " ");
    const auto name = splitted[0].substr(0, 4);
    Monkey * me = get_monkey(name);

    if (splitted.size() == 2) {
      me->value = std::stoi(splitted[1]);
    } else {
      me->value = std::nullopt;
      me->lhs = get_monkey(splitted[1]);
      me->rhs = get_monkey(splitted[3]);
      me->operation = sanitize_operation(splitted[2][0]);
    }
  }
}

struct PossibleInterval {
  long long int min;
  long long int max;

  [[nodiscard]] bool has_only_one_value() const {
    return max - min == 1;
  }

  [[nodiscard]] long long int get_next() {
    return min + (max - min) / 2;
  }
};

long long int compute_part_b(Monkeys & monkeys) {
  Monkey * root = monkeys.get_monkey("root");

  // Possibles values
  PossibleInterval interval { 0, 1000000000000000ll };

  // Check if the problem can be seen as solving an equation f(x) = y
  // where y is the right monkey and is stable, f(x) is left and x is humn.
  Monkey * human = monkeys.get_monkey("humn");

  if (root->count(human) != 1) {
    // We assume that f(x) is somewhat linear = human only appears once in the
    // formula. This is so we are ensured that the function is either growing
    // or ... the opposite of growing
    throw std::runtime_error("Monkeys are referring several time to human");
  }

  human->value = interval.min;
  const auto left_min = root->lhs->what_does_the_monkey_say();
  const auto right_min = root->rhs->what_does_the_monkey_say();

  human->value = interval.max;
  const auto left_max = root->lhs->what_does_the_monkey_say();
  const auto right_max = root->rhs->what_does_the_monkey_say();

  if (right_min != right_max) {
    throw std::runtime_error("Right monkey of root is expected to be fixed");
  }

  const auto target = right_max;

  const bool function_is_growing = left_min < left_max;

  if (function_is_growing) {
    if (target < left_min || target >= left_max)
      throw std::runtime_error("Target is out of range");
  } else {
    if (target < left_max || target >= left_min)
      throw std::runtime_error("Target is out of range");
  }

  // Find the right value to shoot
  while (!interval.has_only_one_value()) {
    const long long int next_value = interval.get_next();

    human->value = next_value;

    const auto result = root->lhs->what_does_the_monkey_say();

    if (result == target) {
      interval.min = next_value;
      interval.max = next_value + 1;
      break;      
    }

    if ((result > target) == function_is_growing) {
      interval.max = next_value;
    } else {
      interval.min = next_value;
    }
  }

  // Look for minimal value as it is the one expected by AOC
  while (true) {
    human->value = interval.min - 1;
    const auto result = root->lhs->what_does_the_monkey_say();

    if (result != target) break;

    --interval.min;
  }

  return interval.min;
}

}

Output day_2022_21(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Monkeys monkeys(lines);

  // Part A
  Monkey * root = monkeys.get_monkey("root");
  const auto root_base_value = root->what_does_the_monkey_say();

  // Part B
  const auto part_b_result = compute_part_b(monkeys);

  return Output(root_base_value, part_b_result);
}
