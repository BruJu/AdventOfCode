#include "../advent_of_code.hpp"
#include "../util/instruction_reader.hpp"
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <optional>
#include <iostream>
#include <array>
#include <queue>

// https://adventofcode.com/2021/day/24


struct ProgramInput {
  std::array<int, 14> values;

  ProgramInput(int init = 9) {
    values.fill(init);
  }

  [[nodiscard]] int operator[](size_t i) const {
    return values[i];
  }

  ProgramInput & operator--() {
    for (int i = 14 - 1; i >= 0; ++i) {
      --values[i];

      if (values[i] == 0) {
        values[i] = 9;
      } else {
        break;
      }
    }

    return *this;
  }


  ProgramInput & operator++() {
    for (int i = 14 - 1; i >= 0; ++i) {
      ++values[i];

      if (values[i] == 10) {
        values[i] = 1;
      } else {
        break;
      }
    }

    return *this;
  }

  [[nodiscard]] long long int to_int() const {
    long long int v = 0;

    for (int x : values) {
      v = v * 10 + x;
    }

    return v;
  }


};


struct Memory {
  ProgramInput inputs;
  int i_input = 0;
  long long int w = 0;
  long long int x = 0;
  long long int y = 0;
  long long int z = 0;


  long long int & get(char c) {
    if (c == 'w') return w;
    else if (c == 'x') return x;
    else if (c == 'y') return y;
    else if (c == 'z') return z;
    std::cout << "error: invalid name " << c << '\n';
    return z;
  }

  int get_input() {
    if (i_input == 14) {
      std::cout << "no input but one was required\n";
      return 0;
    }

    return inputs[i_input++];
  }
};

enum class IType {
  imp, add, mul, div, mod, eql
};

IType to_type(const std::string & s) {
  if (s == "inp") return IType::imp;
  if (s == "add") return IType::add;
  if (s == "mul") return IType::mul;
  if (s == "div") return IType::div;
  if (s == "mod") return IType::mod;
  if (s == "eql") return IType::eql;

  std::cout << "unknown instruction " << s << '\n';
  exit(0);
}

struct Instruction {
  IType type;

  char variable = '0';

  bool is_int = false;
  long long int int_value = 0;
  char variable_2 = '0';

  void print_cpp(int & input) const {
    const auto & name_arg = [&]() {
      if (is_int) return std::to_string(int_value);
      else return std::string("") + variable_2;
    };

    switch (type) {
      case IType::imp:
        std::cout << variable << " = input[" << (input++) << "]; \n";
        break;
      case IType::add:
        std::cout << variable << " += " << name_arg() << "; \n";
        break;
      case IType::mul:
        std::cout << variable << " *= " << name_arg() << "; \n";
        break;
      case IType::div:
        std::cout << "if (" << name_arg() << " == 0) return false; \n";
        std::cout << variable << " /= " << name_arg() << "; \n";
        break;
      case IType::mod:
        std::cout << "if (" << variable << " < 0) return false; \n";
        std::cout << "if (" << name_arg() << " == 0) return false; \n";
        std::cout << variable << " %= " << name_arg() << "; \n";
        break;
      case IType::eql:
        std::cout << variable << " = ( " << variable << " == " << name_arg() << ") ? 1 : 0;\n";
        break;
    }
  }

  Instruction(std::string c) {
    type = IType::imp;
    variable = c[0];
  }

  Instruction(std::string type_, const std::string & dest, const std::string & src) {
    type = to_type(type_);
    variable = dest[0];
    is_int = false;
    variable_2 = src[0];
  }

  Instruction(std::string type_, const std::string & dest, int val) {
    type = to_type(type_);
    variable = dest[0];
    is_int = true;
    int_value = val;
  }

  bool modify(Memory & state) const {
    switch (type) {
      case IType::imp:
        state.get(variable) = state.get_input();
        break;
      case IType::add:
        state.get(variable) += value(state);
        break;
      case IType::mul:
        state.get(variable) *= value(state);
        break;
      case IType::div:
      {
        long long int b = value(state);
        if (b == 0) return false;
        state.get(variable) /= b;
      }
        break;
      case IType::mod:
      {
        if (state.get(variable) < 0) return false;
        long long int b = value(state);
        if (b == 0) return false;
        state.get(variable) %= b;
      }
        break;
      case IType::eql:
        state.get(variable) = state.get(variable) == value(state) ? 1 : 0;
        break;
    }

    return true;
  }

  long long int value(Memory & state) const {
    if (is_int) {
      return int_value;
    } else {
      return state.get(variable_2);
    }
  }
};

void into_cpp(const std::vector<Instruction> & instructioons) {
  // Tried to compile the code into C++ to benefit from gcc optimization
  // that's still too slow
  int input = 0;
  for (const auto & instruction : instructioons) {
    instruction.print_cpp(input);
  }
}


static std::vector<Instruction> read_input(const std::vector<std::string> & lines) {
  bj::InstructionReader<Instruction> ir;

  ir.add_handler(R"(inp ([a-z]))",
    [](const std::vector<std::string> & strs) {
      return Instruction(strs[0]);
    }
  );

  ir.add_handler(R"(([a-z]+) ([a-z]) (-?[0-9]+))",
    [](const std::vector<std::string> & strs) {
      return Instruction(strs[0], strs[1], std::stoi(strs[2]));
    }
  );

  ir.add_handler(
    R"(([a-z]+) ([a-z]) ([a-z]))",
    [](const std::vector<std::string> & strs) {
      return Instruction(strs[0], strs[1], strs[2]);
    }
  );

  return ir(lines).value();
}

Output day_2021_24(const std::vector<std::string> & lines, const DayExtraInfo &) {
  return Output("ok", "");

  // Input is too big
  // I don't want to use an external tool to solve this, it's plain boring.
  // If the purpose is to use SOTA tools for CSP solving, I'm out

  const auto instructions = read_input(lines);

  ProgramInput programInput(9);

  while (true) {
    Memory mem { programInput };

    bool in_the_end = true;

    int used_inputs = 0;

    for (size_t i = 0; i != instructions.size(); ++i) {
      const auto & instruction = instructions[i];

      if (!instruction.modify(mem)) {
        in_the_end = false;
        break;
      }

      if (instruction.type == IType::imp) {
        ++used_inputs;
      }
    }

    if (in_the_end && mem.z == 0) {
      std::cout << programInput.to_int() << '\n';
    }

    ++programInput;
  }

  return Output(programInput.to_int(), 0);
}
