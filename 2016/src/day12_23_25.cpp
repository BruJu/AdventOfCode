#include "../../common/advent_of_code.hpp"
#include "../../util/instruction_reader.hpp"
#include <cstring>
#include <algorithm>
#include <array>
#include <map>
#include <queue>
#include <variant>

// https://adventofcode.com/2016/day/12

enum class InstructionType {
  cpy, inc, jnz, dec, tgl, out
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

static void run_instruction(
  int & pc,
  std::vector<Instruction> & instructions,
  std::map<std::string, int> & memory,
  std::optional<int> & out
) {
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
    case InstructionType::tgl: {
      const int tgl_offset = get(memory, instruction.x);
      const int tgl_line = pc + tgl_offset;
      if (tgl_line < 0 || std::cmp_greater_equal(tgl_line, instructions.size())) {
        ++pc;
        break;
      }

      Instruction & modified = instructions[tgl_line];

      switch (modified.type) {
        case InstructionType::cpy: modified.type = InstructionType::jnz; break;
        case InstructionType::inc: modified.type = InstructionType::dec; break;
        case InstructionType::jnz: modified.type = InstructionType::cpy; break;
        case InstructionType::dec: modified.type = InstructionType::inc; break;
        case InstructionType::tgl: modified.type = InstructionType::inc; break;
        case InstructionType::out: modified.type = InstructionType::inc; break;
      }

      ++pc;

      break;
    }
    case InstructionType::out: {
      out = get(memory, instruction.x);
      ++pc;
      break;
    }
  }
}

static int run(std::vector<Instruction> instructions, std::map<std::string, int> memory) {
  int pc = 0;
  
  std::optional<int> useless;
  while (pc >= 0 && pc < static_cast<int>(instructions.size())) {
    run_instruction(pc, instructions, memory, useless);
  }

  return memory["a"];
}


std::vector<Instruction> read_instructions(const std::vector<std::string> & lines) {
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
    } else if (split[0] == "tgl") {
      instructions.emplace_back(Instruction{ InstructionType::tgl, to_thing(split[1]), Thing(0) });
    } else if (split[0] == "out") {
      instructions.emplace_back(Instruction{ InstructionType::out, to_thing(split[1]), Thing(0) });
    } else {
      throw std::runtime_error("Unknown instruction");
    }
  }

  return instructions;
}

Output day_2016_12(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto instructions = read_instructions(lines);

  const int part_a = run(instructions, {});
  const int part_b = run(instructions, { { "c", 1 } });

  return Output(part_a, part_b);
}

Output day_2016_23(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto instructions = read_instructions(lines);

  const int part_a = run(instructions, { { "a", 7 } });
  const int part_b = run(instructions, { { "a", 12 } });

  return Output(part_a, part_b);
}

static bool is_01(std::vector<Instruction> instructions, int a) {
  std::map<std::string, int> memory;
  memory["a"] = a;

  int pc = 0;
  std::optional<int> out;

  int chars_size = 0;

  while (true) {
    run_instruction(pc, instructions, memory, out);

    if (out) {
      if (chars_size % 2 != *out) {
        return false;
      }

      ++chars_size;
      out = std::nullopt;

      if (chars_size >= 1000) return true;
    }
  }
}

Output day_2016_25(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto instructions = read_instructions(lines);

  int lowest_value = 0;

  while (true) {
    if (is_01(instructions, lowest_value)) {
      break;
    }

    ++lowest_value;
  }

  return Output(lowest_value, "Oui");
}
