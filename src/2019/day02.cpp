#include "../advent_of_code.hpp"
#include <string>

// https://adventofcode.com/2019/day/02
// https://adventofcode.com/2019/day/05
// https://adventofcode.com/2019/day/07
// https://adventofcode.com/2019/day/09

using Int = std::int64_t;

enum class OpInstruction {
  // Day 2
  Op1_Plus, Op2_Mult, Op99_Halt,
  // Day 5
  Op3_Input, Op4_Output, 
  Op5_JumpTrue, Op6_JumpFalse, Op7_LessThan, Op8_Equals
};

static std::vector<Int> read_op_codes(const std::string & line) {
  std::vector<std::string> opcodes_str = bj::string_split(line, ",");
  std::vector<Int> opcodes;
  for (const std::string & opcode_str : opcodes_str) {
    try {
    opcodes.emplace_back(std::stoll(opcode_str));
    } catch(std::exception e){
      std::cout << "lavaleur <" <<opcode_str << ">\n";
      throw e;
    }
  }
  return opcodes;
}

struct OpCodeMachine {
  size_t pc = 0;
  std::vector<Int> opcodes;
  std::vector<Int> inputs;
  std::vector<Int> outputs;

  explicit OpCodeMachine(std::vector<Int> opcodes)
    : opcodes(std::move(opcodes)) {}

  OpInstruction run_current_instruction();
  void run_until_halt();

  static bool get_parameter_type(int position, int code) {
    int div = 100;
    for (int i = 0; i != position; ++i) {
      div *= 10;
    }
    return (code / div) % 10;
  }

  Int get_parameter(Int value, int parameter_mode) {
    if (parameter_mode == 0) {
      return opcodes[value];
    } else if (parameter_mode == 1) {
      return value;
    } else {
      throw std::runtime_error("invalid parameter mode");
    }
  }

};


OpInstruction OpCodeMachine::run_current_instruction() {
  const auto instruction_int = opcodes[pc];

  const Int instruction_type = opcodes[pc] % 100;
  const int mode_param_1 = get_parameter_type(0, opcodes[pc]);
  const int mode_param_2 = get_parameter_type(1, opcodes[pc]);
  const int mode_param_3 = get_parameter_type(2, opcodes[pc]);

  if (instruction_type == 1) {
    const auto lhs = get_parameter(opcodes[pc + 1], mode_param_1);
    const auto rhs = get_parameter(opcodes[pc + 2], mode_param_2);
    auto & output = opcodes[opcodes[pc + 3]];
    output = lhs + rhs;
    pc += 4;
    return OpInstruction::Op1_Plus;
  } else if (instruction_type == 2) {
    const auto lhs = get_parameter(opcodes[pc + 1], mode_param_1);
    const auto rhs = get_parameter(opcodes[pc + 2], mode_param_2);
    auto & output = opcodes[opcodes[pc + 3]];
    output = lhs * rhs;
    pc += 4;
    return OpInstruction::Op2_Mult;
  } else if (instruction_type == 99) {
    pc += 1;
    return OpInstruction::Op99_Halt;
  } else if (instruction_type == 3) {
    auto & target = opcodes[opcodes[pc + 1]];
    if (inputs.empty()) throw std::runtime_error("No more value in input");
    target = inputs.front();
    inputs.erase(inputs.begin());
    pc += 2;
    return OpInstruction::Op3_Input;
  } else if (instruction_type == 4) {
    outputs.emplace_back(get_parameter(opcodes[pc + 1], mode_param_1));
    pc += 2;
    return OpInstruction::Op4_Output;
  } else if (instruction_type == 5) {
    const auto value = get_parameter(opcodes[pc + 1], mode_param_1);
    if (value != 0) {
      pc = get_parameter(opcodes[pc + 2], mode_param_2);
    } else {
      pc += 3;
    }
    return OpInstruction::Op5_JumpTrue;
  } else if (instruction_type == 6) {
    const auto value = get_parameter(opcodes[pc + 1], mode_param_1);
    if (value == 0) {
      pc = get_parameter(opcodes[pc + 2], mode_param_2);
    } else {
      pc += 3;
    }
    return OpInstruction::Op6_JumpFalse;
  } else if (instruction_type == 7) {
    const auto lhs = get_parameter(opcodes[pc + 1], mode_param_1);
    const auto rhs = get_parameter(opcodes[pc + 2], mode_param_2);
    auto & output = opcodes[opcodes[pc + 3]];
    output = lhs < rhs ? 1 : 0;
    pc += 4;
    return OpInstruction::Op7_LessThan;
  } else if (instruction_type == 8) {
    const auto lhs = get_parameter(opcodes[pc + 1], mode_param_1);
    const auto rhs = get_parameter(opcodes[pc + 2], mode_param_2);
    auto & output = opcodes[opcodes[pc + 3]];
    output = lhs == rhs ? 1 : 0;
    pc += 4;
    return OpInstruction::Op8_Equals;
  } else {
    std::cout << instruction_type << "\n";
    throw std::runtime_error("Unknown instruction");
  }
}

void OpCodeMachine::run_until_halt() {
  while (run_current_instruction() != OpInstruction::Op99_Halt);
}


namespace {

namespace aoc2019_02 {

static int run(std::vector<Int> opcodes, int noun, int verb) {
  opcodes[1] = noun;
  opcodes[2] = verb;

  OpCodeMachine machine(std::move(opcodes));
  machine.run_until_halt();
  return machine.opcodes[0];
}

static int find_19690720(const std::vector<Int> & opcodes) {
  for (int noun = 0; noun <= 99; ++noun) {
    for (int verb = 0; verb <= 99; ++verb) {
      const auto res = run(opcodes, noun, verb);
      if (res == 19690720) {
        return noun * 100 + verb;
      }
    }
  }
  return -1;
}

}

}


Output day_2019_02(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto opcodes = read_op_codes(lines[0]);

  return Output(
    aoc2019_02::run(opcodes, 12, 2),
    aoc2019_02::find_19690720(opcodes)
    );
}

Output day_2019_05(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto opcodes = read_op_codes(lines[0]);
  
  Int part_a;
  {
    OpCodeMachine machine(opcodes);
    machine.inputs.emplace_back(1);
    machine.run_until_halt();
    part_a = machine.outputs.back();
  }

  Int part_b;
  {
    OpCodeMachine machine(opcodes);
    machine.inputs.emplace_back(5);
    machine.run_until_halt();
    part_b = machine.outputs.back();
  }

  return Output(part_a, part_b);
}

static Int compute_thruster(std::array<bool, 5> & taken, const std::vector<Int> & codes, Int input) {
  Int best = 0;
  bool found_one = false;

  for (size_t i = 0; i != 5; ++i) {
    if (taken[i]) continue;

    taken[i] = true;
    OpCodeMachine amplifier(codes);
    amplifier.inputs.emplace_back(i);
    amplifier.inputs.emplace_back(input);
    amplifier.run_until_halt();
    const Int value = compute_thruster(taken, codes, amplifier.outputs.back());
    found_one = true;
    best = std::max(best, value);
    taken[i] = false;
  }

  if (!found_one) return input;
  return best;
}

Output day_2019_07(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto opcodes = read_op_codes(lines[0]);

  std::array<bool, 5> values;
  values.fill(false);
  const auto part_a = compute_thruster(values, opcodes, 0);


  return Output(part_a, 0);
}
