#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <map>
#include <utility>
#include <variant>
#include <set>
#include <ranges>
#include <algorithm>
#include <span>

#include <ctime>
#include <random>

#include <map>

// https://adventofcode.com/2024/day/24

using i64 = std::int64_t;

namespace {


enum class Operator {
  Xor, And, Or
};

Operator to_operator(std::string str) {
  if (str == "XOR") return Operator::Xor;
  if (str == "AND") return Operator::And;
  if (str == "OR") return Operator::Or;
  printf("str=  %s\n", str.c_str());
  throw std::runtime_error("not an operator");
}

struct Wire;

struct Calculus {
  std::string lhs;
  std::string rhs;
  Operator op_;
  std::string out;

  int solve(std::map<std::string, Wire> & wires);
};

struct Wire {
  std::string name;
  std::variant<int, Calculus> value;

  int get_value(std::map<std::string, Wire> & wires) {
    if (Calculus * calc = std::get_if<Calculus>(&value)) {
      int v = calc->solve(wires);
      value = v;
      return v;
    }

    return std::get<int>(value);
  }
};


  int Calculus::solve(std::map<std::string, Wire> & wires) {
    const int left = wires[lhs].get_value(wires);
    const int right = wires[rhs].get_value(wires);

    if (op_ == Operator::And) {
      return left && right ? 1 : 0;
    } else if (op_ == Operator::Or) {
      return left || wires[rhs].get_value(wires) ? 1 : 0;
    } else {
    const int right = wires[rhs].get_value(wires);
      return left == right ? 0 : 1;
    }
  }

std::pair<
  std::map<std::string, int>,
  std::vector<Calculus>
> read_input(const std::vector<std::string> & lines) {
  // RAM
  // empty line
  // Maths

  // 1- RAM
  std::map<std::string, int> ram;
  
  auto it_line = lines.begin();
  while (*it_line != "") {
    // Format is as followes:
    // x23: 0
    ram[it_line->substr(0, 3)] = std::stoi(it_line->substr(it_line->find(":") + 1));
    ++it_line;
  }

  // 2- Jump empty line
  ++it_line;

  // 3- Maths
  std::vector<Calculus> maths;
  while (it_line != lines.end()) {
    std::vector<std::string> strs = bj::string_split(*it_line, " ");

    maths.emplace_back(Calculus{
      strs[0],
      strs[2],
      to_operator(strs[1]),
      strs[4],
    });

    ++it_line;
  }

  return std::pair(ram, maths);
}


std::map<std::string, Wire> to_wires(
  const std::map<std::string, int> & ram,
  const std::vector<Calculus> & maths
) {
  std::map<std::string, Wire> wires;

  for (const auto & [name, value] : ram) {
    wires[name] = Wire{ name, value };
  }

  for (const Calculus & calc : maths) {
    wires[calc.out] = Wire { calc.out, calc };
  }

  return wires;
}


std::int64_t evaluate(std::map<std::string, Wire> & wires, char first_letter) {
  std::int64_t final_value = 0;

  for (auto & [wire_name, wire] : wires) {
    if (wire_name[0] != first_letter) continue;

    const std::int64_t v = wire.get_value(wires);
    int shift = (wire_name[1] - '0') * 10 + (wire_name[2] - '0');

    final_value += v << shift;
  }

  return final_value;
}

std::int64_t solve_part_a(
  std::map<std::string, int> ram,
  std::vector<Calculus> calculus
) {
  std::map<std::string, Wire> wires = to_wires(ram, calculus);
  return evaluate(wires, 'z');
}



}

Output day_2024_24(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  const auto & [ram, maths] = read_input(lines);


  std::int64_t part_a_answer = solve_part_a(ram, maths);


  return Output(
    part_a_answer, 0
    );
}
