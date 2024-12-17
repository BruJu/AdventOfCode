#include "../../common/advent_of_code.hpp"
#include <cstring>
#include <utility>

// https://adventofcode.com/2024/day/17

namespace {

struct Input {
  std::array<long long int, 3> ram;
  std::vector<int> program;

  Input(std::vector<std::string> input) {
    ram[0] = std::stoi(input[0].substr(std::strlen("Register A: ")));
    ram[1] = std::stoi(input[1].substr(std::strlen("Register B: ")));
    ram[2] = std::stoi(input[2].substr(std::strlen("Register C: ")));
    program = bj::extract_ints_from_line(input[4]);
  }
};

long long int get_combo(std::span<long long int, 3> ram, int operand) {
  if (operand <= 3) return operand;
  return ram[operand - 4];
}

std::optional<int> do_instruction(std::span<long long int, 3> ram, int operand, int instruction, int & pc) {
  std::optional<int> out;
  switch (instruction) {
    case 0: // adv
      ram[0] = ram[0] / (1 << get_combo(ram, operand)); 
      pc += 2;
      break;
    case 6: // bdv
      ram[1] = ram[0] / (1 << get_combo(ram, operand)); 
      pc += 2;
      break;
    case 7: // cdv
      ram[2] = ram[0] / (1 << get_combo(ram, operand)); 
      pc += 2;
      break;
    case 1: // bxl
      ram[1] = ram[1] ^ operand;
      pc += 2;
      break;
    case 2: // bst
      ram[1] = get_combo(ram, operand) % 8;
      pc += 2;
      break;
    case 3: // jnz
      if (ram[0] == 0) {
        pc += 2;
      } else {
        pc = operand;
      }
      break;
    case 4: // bxc
      ram[1] = ram[1] ^ ram[2];
      pc += 2;
      break;
    case 5: // out
      out = get_combo(ram, operand) % 8;
      pc += 2;
      break;
  }

  return out;
}

std::string to_string(std::span<const int> values) {
  std::string str;
  for (int value : values) {
    if (!str.empty()) str += ',';
    str += std::to_string(value);
  }

  return str;
}

std::vector<int> run(Input input) {
  std::array<long long int, 3> ram = input.ram;
  std::vector<int> program = input.program;

  std::vector<int> output;
  int pc = 0;
  while (true) {
    if (std::cmp_greater_equal(pc, program.size())) break;

    auto out = do_instruction(ram, program[pc + 1], program[pc], pc);
    if (out) output.emplace_back(out.value());
  }

  return output;
}

bool equals(std::span<const int> left, std::span<const int> right) {
  if (left.size() != right.size()) return false;
  for (size_t i = 0; i != left.size(); ++i) {
    if (left[i] != right[i]) return false;
  }
  return true;
};

long long int run_b_on_any(Input input, std::span<const int> expected) {
  input.ram[0] = 0;

  while (true) {
    std::vector<int> output = run(input);
    if (equals(output, expected)) {
      return input.ram[0];
    }

    input.ram[0] += 1;
  }
}

std::optional<long long int> run_b_on_real(Input input, std::span<const int> expected) {
  // Maybe given a number ABCDEFG in base 8:
  // the last out number only depends on A
  // then the number before only depends on AB
  // ...

  input.ram[0] = 0;
  for (size_t i = 0; i < expected.size(); ++i) {
    input.ram[0] *= 8;

    while (true) {
      std::vector<int> output = run(input);
      if (equals(output, expected.subspan(expected.size() - 1 - i))) {
        break;
      }

      if (output.size() > expected.size()) {
        return std::nullopt;
      }

      ++input.ram[0];
    }
  }
  return input.ram[0];
}

}

Output day_2024_17(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  Input input(lines);

  std::string part_a = to_string(run(input));

  long long int part_b = 0;

  if (!dei.can_skip_part_B) {
    std::optional<long long int> solution = run_b_on_real(input, input.program);
    if (solution) {
      part_b = *solution;
    } else {
      part_b = run_b_on_any(input, input.program);
    }
  }
  
  return Output(part_a, part_b);
}
