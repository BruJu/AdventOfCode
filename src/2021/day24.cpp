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


////////////////////////////////////////////////////////////////////////////////

enum class IType { imp, add, mul, div, mod, eql };

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


////////////////////////////////////////////////////////////////////////////////

using ProgramInput = long long int;

static std::array<ProgramInput, 9> derivate(ProgramInput self) {
  std::array<ProgramInput, 9> result;
  for (int i = 1; i <= 9; ++i) {
    result[i - 1] = self * 10 + i;
  }
  return result;
}


////////////////////////////////////////////////////////////////////////////////

struct RAMState {
  int w = 0;
  int x = 0;
  int y = 0;
  int z = 0;
  bool bad_state = false;
  
  int & get(char c) {
    if (c == 'w') return w;
    else if (c == 'x') return x;
    else if (c == 'y') return y;
    else if (c == 'z') return z;
    std::cout << "error: invalid name " << c << '\n';
    return z;
  }

  [[nodiscard]] bool operator==(const RAMState & other) const {
    if (bad_state != other.bad_state) return false;
    if (bad_state) return true;

    return w == other.w && x == other.x && y == other.y && z == other.z;
  }
};


namespace std {
  template <> struct hash<RAMState> {
    size_t operator()(const RAMState & r) const {
      if (r.bad_state) return 0;
      return (((((r.w << 4) + r.x) << 4) + r.y) << 4) + r.z + 1;
    }
  };
}

////////////////////////////////////////////////////////////////////////////////

struct Instruction {
  IType type;

  char variable = '0';

  bool is_int = false;
  long long int int_value = 0;
  char variable_2 = '0';

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

  bool modify(RAMState & state, ProgramInput input) const {
    switch (type) {
      case IType::imp:
        state.get(variable) = (input % 10);
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

  long long int value(RAMState & state) const {
    if (is_int) {
      return int_value;
    } else {
      return state.get(variable_2);
    }
  }
};


struct OneUniverse {
  RAMState ram;
  ProgramInput input = 0;
};



struct Memory {
  std::vector<OneUniverse> universes;

  Memory() {
    universes.push_back(OneUniverse{});
  }

  void run(const Instruction & instruction) {
    switch (instruction.type) {
      case IType::imp: {
        reduce_universes();
        expand_universes();
        break;
      }
    }

    for (auto it = universes.begin(); it != universes.end();) {
      if (!instruction.modify(it->ram, it->input)) {
        it->ram.bad_state = true;
        ++it;
      } else {
        ++it;
      }
    }

    if (instruction.type == IType::mul && instruction.type == IType::mod && instruction.type == IType::eql) {
      reduce_universes();
    }
  }

  void reduce_universes() {
    // Find higher
    std::unordered_map<RAMState, ProgramInput> to_the_sky;
    std::unordered_map<RAMState, ProgramInput> to_the_ground;
    for (const auto & universe : universes) {
      auto sky_it = to_the_sky.find(universe.ram);

      if (sky_it == to_the_sky.end()) {
        to_the_sky.emplace(universe.ram, universe.input);
        to_the_ground.emplace(universe.ram, universe.input);
      } else {
        if (sky_it->second < universe.input) {
          sky_it->second = universe.input;
        }

        auto ground_it = to_the_ground.find(universe.ram);
        if (universe.input < ground_it->second) {
          ground_it->second = universe.input;
        }
      }
    }

    // Change universes
    universes.clear();
    for (const auto & [ram, input] : to_the_sky) {
      universes.push_back(OneUniverse{ ram, input });
    }
    for (const auto & [ram, input] : to_the_ground) {
      universes.push_back(OneUniverse{ ram, input });
    }
  }

  void expand_universes() {
    std::vector<OneUniverse> next;

    for (const auto & universe : universes) {
      for (const auto & expansion : derivate(universe.input)) {
        next.push_back(OneUniverse { universe.ram, expansion });
      }
    }
    
    universes = next;
  }

  void remove_not_final() {
    universes.erase(
      std::remove_if(
        universes.begin(),
        universes.end(),
        [](const OneUniverse & universe) {
          return universe.ram.bad_state || universe.ram.z != 0;
        }
      ),
      universes.end()
    );
    
    reduce_universes();
  }

  Output get_min_max() {
    const auto min = std::min_element(universes.begin(), universes.end(),
      [](const OneUniverse & lhs, const OneUniverse & rhs) {
        return lhs.input < rhs.input;
      }
    );
    
    const auto max = std::min_element(universes.begin(), universes.end(),
      [](const OneUniverse & lhs, const OneUniverse & rhs) {
        return lhs.input > rhs.input;
      }
    );

    return Output(max->input, min->input);
  }
};

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
  const auto instructions = read_input(lines);

  Memory memory;
  size_t line = 0;
  for (const auto & instruction : instructions) {
    std::cout << (++line) << " " << memory.universes.size() << '\n';
    memory.run(instruction);
  }

  memory.remove_not_final();

  return memory.get_min_max();
}

