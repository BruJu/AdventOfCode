#include "../advent_of_code.hpp"
#include <vector>
#include <map>
#include <cstring>
#include <array>
#include <variant>
#include <deque>

// https://adventofcode.com/2017/day/18

namespace {

enum class IType { snd, set, add, mul, mod, rcv, jgz };
using Operand = std::variant<std::int64_t, char>;

struct Instruction {
  IType type;
  Operand x;
  Operand y;
};

struct Program {
  std::array<std::int64_t, 26> registry;
  std::deque<std::int64_t> played_sound;
  std::int64_t qtt_sent = 0;
  int pc = 0;

  Program() { registry.fill(0); }

  std::int64_t get(Operand operator_) {
    if (const char * operand_char = std::get_if<char>(&operator_)) {
      return registry[*operand_char - 'a'];
    } else {
      return std::get<std::int64_t>(operator_);
    }
  }

  bool run_until_rcv(const std::vector<Instruction> & instructions) {
    const size_t max_pc = instructions.size();

    while (pc >= 0 && std::cmp_less(pc, max_pc)) {
      const Instruction & instruction = instructions[pc];
      
      if (instruction.type == IType::snd) {
        played_sound.emplace_back(get(instruction.x));
        ++qtt_sent;
        ++pc;
      } else if (instruction.type == IType::set) {
        registry[std::get<char>(instruction.x) - 'a'] = get(instruction.y);
        ++pc;
      } else if (instruction.type == IType::add) {
        registry[std::get<char>(instruction.x) - 'a'] += get(instruction.y);
        ++pc;
      } else if (instruction.type == IType::mul) {
        registry[std::get<char>(instruction.x) - 'a'] *= get(instruction.y);
        ++pc;
      } else if (instruction.type == IType::mod) {
        registry[std::get<char>(instruction.x) - 'a'] %= get(instruction.y);
        ++pc;
      } else if (instruction.type == IType::rcv) {
        return true;
      } else if (instruction.type == IType::jgz) {
        if (get(instruction.x) > 0) {
          pc += get(instruction.y);
        } else {
          ++pc;
        }
      }
    }

    return false;
  }

  void receive(std::int64_t value, const std::vector<Instruction> & instructions) {
    if (pc >= 0 && std::cmp_less(pc, instructions.size())) {
      registry[std::get<char>(instructions[pc].x) - 'a'] = value;
      ++pc;
    }
  }

  bool try_to_resume(Program & sender, const std::vector<Instruction> & instructions) {
    if (pc < 0 || std::cmp_greater_equal(pc, instructions.size())) {
      return false;
    }

    if (sender.played_sound.empty()) return false;

    std::int64_t sent_value = sender.played_sound.front();
    sender.played_sound.pop_front();

    receive(sent_value, instructions);
    run_until_rcv(instructions);
    try_to_resume(sender, instructions);

    return true;
  }
};

IType to_itype(const std::string & name) {
  if (name == "snd") { return IType::snd;
  } else if (name == "set") { return IType::set;
  } else if (name == "add") { return IType::add;
  } else if (name == "mul") { return IType::mul;
  } else if (name == "mod") { return IType::mod;
  } else if (name == "rcv") { return IType::rcv;
  } else if (name == "jgz") { return IType::jgz;
  } else {
    throw std::runtime_error("Invalid itype");
  }
}

Operand to_operand(const std::string & name) {
  if (name.size() == 1 && name[0] >= 'a' && name[0] <= 'z') {
    return char(name[0]);
  } else {
    return std::int64_t(std::stoi(name));
  }
}

std::vector<Instruction> to_instructions(const std::vector<std::string> & lines) {
  std::vector<Instruction> instructions;

  for (const std::string & line : lines) {
    const auto tokens = bj::string_split(line, " ");
    instructions.emplace_back(
      Instruction{
        to_itype(tokens[0]),
        to_operand(tokens[1]),
        tokens.size() < 3 ? Operand(char('a')) : to_operand(tokens[2])
      }
    );
  }

  return instructions;
}

}

Output day_2017_18(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto instructions = to_instructions(lines);

  std::int64_t part_a = 0;

  {
    Program program;
    program.run_until_rcv(instructions);
    part_a = program.played_sound.back();
  }

  std::int64_t part_b = 0;
  {
    Program p0;
    Program p1; p1.registry['p' - 'a'] = 1;

    p0.run_until_rcv(instructions);
    p1.run_until_rcv(instructions);
    p1.try_to_resume(p0, instructions);

    while (true) {
      if (!p0.try_to_resume(p1, instructions)) break;
      if (!p1.try_to_resume(p0, instructions)) break;
    }

    part_b = p1.qtt_sent;
  }

  return Output(part_a, part_b);
}
