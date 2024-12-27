#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <map>
#include <utility>
#include <variant>
#include <set>
#include <ranges>
#include <algorithm>

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




}

Output day_2024_24(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::map<std::string, int> ram;

  std::map<std::string, Wire> wires;

  auto it_line = lines.begin();
  while (*it_line != "") {
    // x23: 0
    ram[it_line->substr(0, 3)] = std::stoi(it_line->substr(it_line->find(":") + 1));
    ++it_line;
  }

  std::vector<Calculus> maths;
  ++it_line;
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

  for (const auto & [name, value] : ram) {
    wires[name] = Wire{ name, value };
  }

  for (const Calculus & calc : maths) {
    wires[calc.out] = Wire { calc.out, calc };
  }

  std::map<std::string, int> out_a;

  for (auto & [wire_name, wire] :wires ) {
    if (wire_name[0] == 'z') {
      out_a[wire_name] = wire.get_value(wires);
    }
  }

  std::int64_t out = 0;
  for (const auto & [wire_name, vlaue] : out_a| std::views::reverse) {
    out = out * 2 + vlaue;
  }


  return Output(
    out, 0
    );
}
