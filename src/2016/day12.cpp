#include "../advent_of_code.hpp"
#include "../util/instruction_reader.hpp"
#include <cstring>
#include <algorithm>
#include <array>
#include <map>
#include <queue>

// https://adventofcode.com/2016/day/12

enum class InstructionType {
  cpyvalue, cpyregister, inc, jnz, dec, jnz_constant
};

struct Instruction {
  InstructionType type;
  int value;
  bool valuebool;
  std::string symbol1;
  std::string symbol2;
};

static int run(const std::vector<Instruction> & instructions, std::map<std::string, int> memory) {
  int pc = 0;
  
  while (pc >= 0 && pc < static_cast<int>(instructions.size())) {
    const auto & instruction = instructions[pc];
    // std::cout << pc << '\n';

    switch (instruction.type) {
      case InstructionType::cpyvalue:
        memory[instruction.symbol2] = instruction.value;
        ++pc;
        break;
      case InstructionType::cpyregister:
        memory[instruction.symbol2] = memory[instruction.symbol1];
        ++pc;
        break;
      case InstructionType::inc:
        ++memory[instruction.symbol2];
        ++pc;
        break;
      case InstructionType::dec:
        --memory[instruction.symbol2];
        ++pc;
        break;
      case InstructionType::jnz:
        if (memory[instruction.symbol2] != 0) {
          pc += instruction.value;
        } else {
          ++pc;
        }
        break;
      case InstructionType::jnz_constant:
        if (instruction.valuebool) {
          pc += instruction.value;
        } else {
          ++pc;
        }
        break;
    }
  }

  return memory["a"];
}


Output day_2016_12(const std::vector<std::string> & lines, const DayExtraInfo &) {
  bj::InstructionReader<Instruction> reader;

  reader.add_handler(
    R"(cpy (-?[0-9]+) ([a-z]+))",
    [](const std::vector<std::string> & values) {
      return Instruction { InstructionType::cpyvalue, std::stoi(values[0]), false, "", values[1] };
    }
  );

  reader.add_handler(
    R"(cpy ([a-z]+) ([a-z]+))",
    [](const std::vector<std::string> & values) {
      return Instruction { InstructionType::cpyregister, 0, false, values[0], values[1] };
    }
  );

  reader.add_handler(
    R"(inc ([a-z]+))",
    [](const std::vector<std::string> & values) {
      return Instruction { InstructionType::inc, 0, false, "", values[0] };
    }
  );

  reader.add_handler(
    R"(dec ([a-z]+))",
    [](const std::vector<std::string> & values) {
      return Instruction { InstructionType::dec, 0, false, "", values[0] };
    }
  );

  reader.add_handler(
    R"(jnz ([a-z]+) (-?[0-9]+))",
    [](const std::vector<std::string> & values) {
      return Instruction { InstructionType::jnz, std::stoi(values[1]), false, "", values[0] };
    }
  );

  reader.add_handler(
    R"(jnz (-?[0-9]+) (-?[0-9]+))",
    [](const std::vector<std::string> & values) {
      return Instruction { InstructionType::jnz_constant, std::stoi(values[1]), std::stoi(values[0]) != 0, "", ""} ;
    }
  );

  const auto instructions = reader(lines).value();

  const int part_a = run(instructions, {});
  const int part_b = run(instructions, { { "c", 1 } });

  return Output(part_a, part_b);
}
