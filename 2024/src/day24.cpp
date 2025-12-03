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

std::string operator_to_string(Operator op) {
  switch (op) {
    case Operator::Xor: return "XOR";
    case Operator::And: return "AND";
    case Operator::Or: return "OR";
  }
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

void unevaluate(std::map<std::string, Wire> & wires, char first_letter, std::int64_t value) {
    for (auto & [wire_name, wire] : wires) {
    if (wire_name[0] != first_letter) continue;

    int shift = (wire_name[1] - '0') * 10 + (wire_name[2] - '0');

    wire.value = static_cast<int>((value >> shift) & 1);
  }
}

std::int64_t solve_part_a(
  std::map<std::string, int> ram,
  std::vector<Calculus> calculus
) {
  std::map<std::string, Wire> wires = to_wires(ram, calculus);
  return evaluate(wires, 'z');
}


bool try_something(
  std::set<std::string> * bad_gates,
  std::map<std::string, Wire> wires,
  std::int64_t x, std::int64_t y
) {
  std::int64_t expected = x + y;

  unevaluate(wires, 'x', x);
  unevaluate(wires, 'y', y);

  std::int64_t actual_z = evaluate(wires, 'z');

  size_t number_of_lit = 0;

  for (const auto & [_, wire] : wires) {
    if (const int * v = std::get_if<int>(&wire.value)) {
      if (*v) ++number_of_lit;
    }
  }

  if (expected == actual_z) {
    std::cout << "ok ; nb_lit = " << number_of_lit << "\n";

    return true;
  } else {
    std::cout << "bad found " << actual_z << " instead of " << expected << " ; nb_lit = " << number_of_lit << "\n";

    if (bad_gates) {
      for (const auto & [wire_name, wire] : wires) {
        if (const int * v = std::get_if<int>(&wire.value)) {
          if (*v) {
            bad_gates->emplace(wire_name);
          }
        }
      }
    }

    return false;
  }
}

void fix(
  const std::set<std::string> & all_nodes,
  std::set<std::string> & bad_gates,
  std::map<std::string, Wire> & wires,
  std::int64_t x, std::int64_t y
) {
  std::set<std::string> locally_bad;
  const bool res = try_something(&locally_bad, wires, x, y);

  if (res) {
    return;
  }


  int nb_of_results = 0;

  for (const std::string & locally_bad_node : locally_bad) {

    for (const std::string & other_node : all_nodes) {
      if (locally_bad_node == other_node) continue;

    }




  }



  std::cout << "nb_of_results == " << nb_of_results << "\n";





  




}



int count_number_of_prefix(char prefix, const std::map<std::string, Wire> & wires) {
  int qtt = 0;
  for (const auto & [wire_name, _] : wires) {
    if (wire_name[0] == prefix) {
      ++qtt;
    }
  }
  return qtt;
}


auto build_calculus_2(int input_size) {
  int temporary = 0;

  const auto generate_name = [&]() {
    temporary += 1;
    return "temp" + std::to_string(temporary);
  };

  // initit:
  // x00 XOR y00 = z00
  // x00 AND y00 = carry1

  // recursion: 
  // S     = A XOR B XOR Cin
  // Cnext = (A AND B) + (Cin AND ((A XOR B)))

  std::vector<Calculus> wires;

  std::optional<std::string> carry_in;

  for (int i = 0; i <= input_size; ++i) {
    std::string x = std::format("x{:02}", i);
    std::string y = std::format("y{:02}", i);
    std::string z = std::format("z{:02}", i);

    std::string xor_output = generate_name();
    std::string and_output = generate_name();

    wires.emplace_back(Calculus{x, y, Operator::Xor, i == 0 ? z : xor_output });
    wires.emplace_back(Calculus{x, y, Operator::And, and_output });

    if (carry_in) {
      wires.emplace_back(Calculus{*carry_in, xor_output, Operator::Xor, z });

      std::string and_of_xor_output = generate_name();
      wires.emplace_back(Calculus{*carry_in, xor_output, Operator::And, and_of_xor_output });

      std::string next_carry = i == input_size ? std::format("z{:02}", i + 1) : generate_name();
      wires.emplace_back(Calculus{and_output, and_of_xor_output, Operator::Or, next_carry });

      carry_in = next_carry;
    } else {
      carry_in = and_output;
    }
  }

  return wires;
}


auto build_calculus(int input_size) {
  int temporary = 0;

  const auto generate_name = [&]() {
    temporary += 1;
    return "temp" + std::to_string(temporary);
  };

  // initit:
  // x00 XOR y00 = z00
  // x00 AND y00 = carry1

  // recursion: 
  // S     = A XOR B XOR Cin
  // Cnext = (A AND B) + (Cin AND ((A XOR B)))

  std::vector<Calculus> wires;

  std::optional<std::string> carry_in;

  for (int i = 0; i <= input_size; ++i) {
    std::string x = std::format("x{:02}", i);
    std::string y = std::format("y{:02}", i);
    std::string z = std::format("z{:02}", i);

    std::string xor_output = generate_name();
    std::string and_output = generate_name();

    wires.emplace_back(Calculus{x, y, Operator::Xor, i == 0 ? z : xor_output });
    wires.emplace_back(Calculus{x, y, Operator::And, and_output });

    if (carry_in) {
      wires.emplace_back(Calculus{*carry_in, xor_output, Operator::Xor, z });

      std::string and_of_xor_output = generate_name();
      wires.emplace_back(Calculus{*carry_in, xor_output, Operator::And, and_of_xor_output });

      std::string next_carry = i == input_size ? std::format("z{:02}", i + 1) : generate_name();
      wires.emplace_back(Calculus{and_output, and_of_xor_output, Operator::Or, next_carry });

      carry_in = next_carry;
    } else {
      carry_in = and_output;
    }
  }

  return wires;
}

void rename(
  const std::vector<Calculus> & original,
  std::vector<Calculus> & computed
) {
  std::set<std::string> valid_names;

  // All nodes x, y and z are correctly named
  for (const auto & calculus : computed) {
    for (const std::string & str : { calculus.lhs, calculus.rhs, calculus.out }) {
      if (str[0] == 'x' || str[0] == 'y' || str[0] == 'z') {
        valid_names.emplace(str);
      }
    }
  }

  while (true) {
    bool stable = true;

    // For each calculus:
    // - whose output is named differently
    // - but the inputs are named similarly and the operator is also the same
    // it means that the output is badly named



    if (stable) break;
  }


}

}

Output day_2024_24(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  const auto & [ram, maths] = read_input(lines);


  std::int64_t part_a_answer = solve_part_a(ram, maths);

  if (dei.can_skip_part_B) return Output(part_a_answer, 0);

  if (true) {
    const std::map<std::string, Wire> original = to_wires(ram, maths);
    
    int highest_bit = count_number_of_prefix('x', original);
    int highest_bity = count_number_of_prefix('y', original);
    int highest_bitz = count_number_of_prefix('z', original);
    auto precalced = build_calculus(highest_bit);

    std::cout << highest_bit << " " << highest_bity << " " << highest_bitz << "\n";
    
    const std::map<std::string, Wire> precalced2 = to_wires(ram, precalced);


    try_something(nullptr, precalced2, 0, 0);

    for (int i = 0; i < highest_bit; ++i) {
      std::int64_t me = (1ll << i);
      std::int64_t before = i == 0 ? 0 : (1ll << (i - 1));
      try_something(nullptr, precalced2, me, 0);
      try_something(nullptr, precalced2, 0, me);
      try_something(nullptr, precalced2, me, me);
      try_something(nullptr, precalced2, before + me, before + me);
    }

  }


  if (false)
  {
    const std::map<std::string, Wire> original = to_wires(ram, maths);

    for (const auto & [target, wire] : original) {
      
      if (const Calculus * calc = std::get_if<Calculus>(&wire.value)) {
        std::cout << std::format("{} [label=\"{}\\n{}\"]", target, operator_to_string(calc->op_), target) << "\n";
        // std::cout << std::format("towards{} [label=\"{}\"]", target, operator_to_string(calc->op_)) << "\n";
        std::cout << std::format("{} -> {}", calc->lhs, target) << "\n";
        std::cout << std::format("{} -> {}", calc->rhs, target) << "\n";
        // std::cout << std::format("towards{} -> {}", target, target) << "\n";
      }
    }

    // Observation and hypothesis:
    // if x = y = 0, because we do not have the not operator, all wires are 0
    // x and y should be symmetric
    // If using a power of 2 thati s not 1
    //  3 lit when 0 + power of 2
    //  5 lit when power of 2 + power of 2

    int highest_bit = 0;

    std::set<std::string> bad_gates;

    try_something(nullptr, original, 0, 0);

    for (int i = 0; i <= highest_bit; ++i) {
      std::int64_t me = (1ll << i);
      std::int64_t before = i == 0 ? 0 : (1ll << (i - 1));
      try_something(&bad_gates, original, me, 0);
      try_something(&bad_gates, original, 0, me);
      try_something(&bad_gates, original, me, me);
      try_something(&bad_gates, original, before + me, before + me);
    }

    // Maybe we can run the loop, for all xAA / yBB, report the wires that are lit on error, and randomly try to swap them

    std::cout << "Nb of bad gates = " << bad_gates.size() << "\n";

    // 75 bad gates
    // (75 * 74) * (73 * 72) * (71 * 70) * (69 * 68) = 6 849 647 812 350 000
    // But we know it's a calculator, maybe we should try to fix the bad gates for values up to k.
    // Then commit the fact that the found swaps are correct.

    std::map<std::string, std::set<std::string>> possible_swaps;
    for (const std::string & bad_gate : bad_gates) {
      possible_swaps[bad_gate] = bad_gates;
      possible_swaps[bad_gate].erase(bad_gate);
    }

    for (int i = 0; i <= highest_bit; ++i) {
      std::int64_t me = (1ll << i);
      std::int64_t before = i == 0 ? 0 : (1ll << (i - 1));
//      fix(possible_swaps, bad_gates, original, me, 0);
//      fix(possible_swaps, bad_gates, original, 0, me);
//      fix(possible_swaps, bad_gates, original, me, me);
//      fix(possible_swaps, bad_gates, original, before + me, before + me);
    }



  }




  return Output(
    part_a_answer, 0
    );
}
