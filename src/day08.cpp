#include "libs.hpp"

// https://adventofcode.com/2020/day/8

enum class InstructionType {
    Nop, Acc, Jmp
};

static InstructionType invert_of(InstructionType type) {
    switch (type) {
        case InstructionType::Nop: return InstructionType::Jmp;
        case InstructionType::Jmp: return InstructionType::Nop;
        case InstructionType::Acc: return InstructionType::Acc;
        default: return InstructionType::Acc;
    }
}

struct Instruction {
    InstructionType type;
    int value;

    static Instruction from_string(const std::string & s) {
        const std::string type = s.substr(0, 3);
        const int value = std::stoi(s.substr(4));

        if (type == "acc") {
            return { InstructionType::Acc, value };
        } else if (type == "jmp") {
            return { InstructionType::Jmp, value };
        } else {
            if (type != "nop") { std::cerr << "Invalid instruction " << type << '\n'; }

            return { InstructionType::Nop, value };
        }
    }
};

class Computer {
    size_t m_pc = 0;
    int m_register = 0;
    const std::vector<Instruction> * m_program;
    std::vector<bool> m_already_visited_lines;

public:
    explicit Computer(const std::vector<Instruction> & program)
    : m_program(&program), m_already_visited_lines(program.size(), false) {}

    bool is_on_valid_instruction() const {
        return !pc_is_out_of_memory() && !m_already_visited_lines[m_pc];
    }

    [[nodiscard]] bool pc_is_out_of_memory() const noexcept { return m_pc >= m_program->size(); }
    [[nodiscard]] bool pc_is_at_end()        const noexcept { return m_pc == m_program->size(); }
    
    void run() {
        const auto & instruction = (*m_program)[m_pc];
        m_already_visited_lines[m_pc] = true;

        switch (instruction.type) {
            case InstructionType::Nop:
                ++m_pc;
                break;
            case InstructionType::Acc:
                ++m_pc;
                m_register += instruction.value;
                break;
            case InstructionType::Jmp:
                m_pc += instruction.value;
                break;
        }
    }

    [[nodiscard]] int register_value() const noexcept { return m_register; }

    static std::pair<bool, int> compute(const std::vector<Instruction> & program) {
        Computer computer = Computer(program);

        while (computer.is_on_valid_instruction()) {
            computer.run();
        }

        return std::pair<bool, int>(computer.pc_is_at_end(), computer.register_value());
    }
};

Output day08(const std::vector<std::string> & lines) {
    std::vector<Instruction> instructions = lines_transform::map<Instruction>(lines, Instruction::from_string);

    const auto [_1, accumulator_loop] = Computer::compute(instructions);

    int accumulator_decorrupted = -1;
    for (Instruction & instruction : instructions) {
        if (instruction.type != invert_of(instruction.type)) {
            instruction.type = invert_of(instruction.type);
            const auto [good_end, accumulator] = Computer::compute(instructions);
            instruction.type = invert_of(instruction.type);
            
            if (good_end) {
                accumulator_decorrupted = accumulator;
                break;
            }
        }
    }

    return Output(accumulator_loop, accumulator_decorrupted);
}
