#include "../advent_of_code.hpp"
#include "../util/instruction_reader.hpp"
#include <cstring>
#include <algorithm>
#include <array>
#include <map>
#include <queue>
#include <variant>

// https://adventofcode.com/2016/day/12

enum class InstructionType {
  cpy, inc, jnz, dec
};

using Thing = std::variant<int, std::string>;

struct Instruction {
  InstructionType type;

  Thing x;
  Thing y;
};


int get(const std::map<std::string, int> & memory, const Thing & value) {
  if (const int * value_ = std::get_if<int>(&value)) {
    return *value_;
  } else {
    const std::string * str = std::get_if<std::string>(&value);
    const auto it = memory.find(*str);
    if (it == memory.end()) return 0;
    return it->second;
  }
}

static int run(const std::vector<Instruction> & instructions, std::map<std::string, int> memory) {
  int pc = 0;
  
  while (pc >= 0 && pc < static_cast<int>(instructions.size())) {
    const auto & instruction = instructions[pc];
    // std::cout << pc << '\n';

    switch (instruction.type) {
      case InstructionType::cpy: {
        const std::string * y = std::get_if<std::string>(&instruction.y);
        if (y) {
          memory[*y] = get(memory, instruction.x);
        }
        ++pc;
        break;
      }
      case InstructionType::inc: {
        const std::string * x = std::get_if<std::string>(&instruction.x);
        if (x) {
          ++memory[*x];
        }
        ++pc;
        break;
      }
      case InstructionType::dec: {
        const std::string * x = std::get_if<std::string>(&instruction.x);
        if (x) {
          --memory[*x];
        }
        ++pc;
        break;
      }
      case InstructionType::jnz: {
        const int x = get(memory, instruction.x);
        if (x != 0) {
          const int y = get(memory, instruction.y);
          pc += y;
        } else {
          ++pc;
        }
        break;
      }
    }
  }

  return memory["a"];
}


Output day_2016_12(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<Instruction> instructions;

  for (const std::string & line : lines) {
    const auto split = bj::string_split(line, " ");

    constexpr auto to_thing = [](std::string s) -> Thing {
      if (s[0] >= 'a' && s[0] <= 'z') return s;
      return std::stoi(s);
    };

    if (split[0] == "cpy") {
      instructions.emplace_back(Instruction{ InstructionType::cpy, to_thing(split[1]), to_thing(split[2]) });
    } else if (split[0] == "inc") {
      instructions.emplace_back(Instruction{ InstructionType::inc, to_thing(split[1]), Thing(0) });
    } else if (split[0] == "dec") {
      instructions.emplace_back(Instruction{ InstructionType::dec, to_thing(split[1]), Thing(0) });
    } else if (split[0] == "jnz") {
      instructions.emplace_back(Instruction{ InstructionType::jnz, to_thing(split[1]), to_thing(split[2]) });
    } else {
      throw std::runtime_error("Unknown instruction");
    }
  }

  const int part_a = run(instructions, {});
  const int part_b = run(instructions, { { "c", 1 } });

  return Output(part_a, part_b);
}
