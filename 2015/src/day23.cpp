#include "../../common/advent_of_code.hpp"
#include <algorithm>
#include <optional>
#include <cstring>
#include <array>
#include "../../util/regex_helper.hpp"

// https://adventofcode.com/2015/day/23

// Computer stuff

namespace {
    struct Instruction;

    /** Computer's memory with 2 registers and the PC */
    struct Computer {
        // Memory
        std::array<int32_t, 2> memory = { 0, 0 };
        int32_t pc = 0;

        // Run
        void run(const std::vector<Instruction> & instructions);

        // Lazier access to registers
        int32_t & operator[](uint32_t register_) {
            return memory[register_];
        }
        
        int32_t operator[](uint32_t register_) const {
            return memory[register_];
        }
    };

    /** Supported instructions types */
    enum class Type {
        /** hlf r -> r /= 2 */
        hlf,
        /** tpl r -> r *= 3 */
        tpl,
        /* inc r -> ++r */
        inc,
        /* jmp offset -> pc += offset */
        jmp,
        /* jie r, offset -> jmp offset if r is even  */
        jie,
        /* jio r, offset -> jmp offset if r == 1 */
        jio
    };

    /** Instructions. Regex black magic ensures register name and offset go at the right place */
    struct Instruction {
        // Regex
        static constexpr const char * Regex_Pattern = R"(^([a-z]*) (a|b)?,? *((\+|-)[0-9]*)?$)";

        // Fields (3-address style assembly)
        Type type_;
        int32_t register_;
        int32_t offset_;

        // Constructing an instruction
        explicit Instruction(const std::vector<std::string> & values);

        // Running an instruction
        void apply(Computer & computer) const;
    };
    
    Instruction::Instruction(const std::vector<std::string> & values) {
        if (values[0] == "hlf") type_ = Type::hlf;
        else if (values[0] == "tpl") type_ = Type::tpl;
        else if (values[0] == "inc") type_ = Type::inc;
        else if (values[0] == "jmp") type_ = Type::jmp;
        else if (values[0] == "jie") type_ = Type::jie;
        else if (values[0] == "jio") type_ = Type::jio;
        else std::cerr << "D2015-23: unknown type " << values[0] << "\n";

        if (values[1] == "") register_ = 0;
        else register_ = values[1][0] - 'a';
        
        if (values[2] == "") offset_ = 0;
        else offset_ = std::stoi(values[2]);
    }

    void Instruction::apply(Computer & computer) const {
        switch (type_) {
            case Type::hlf:
                computer[register_] /= 2;
                ++computer.pc;
                break;
            case Type::tpl:
                computer[register_] *= 3;
                ++computer.pc;
                break;
            case Type::inc:
                computer[register_] += 1;
                ++computer.pc;
                break;
            case Type::jmp:
                computer.pc += offset_;
                break;
            case Type::jie:
                if (computer[register_] % 2 == 0) {
                    computer.pc += offset_;
                } else {
                    ++computer.pc;
                }
                break;
            case Type::jio:
                if (computer[register_] == 1) {
                    computer.pc += offset_;
                } else {
                    ++computer.pc;
                }
                break;
        }
    }

    // Overall execution
    void Computer::run(const std::vector<Instruction> & instructions) {
        const int32_t bound = static_cast<int32_t>(instructions.size());

        while (pc >= 0 && pc < bound) {
            instructions[pc].apply(*this);
        }
    }
}

Output day_2015_23(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
    const std::vector<Instruction> instructions = bj::lines_to_class_by_regex<Instruction>(lines);

    // Part A : just run the code
    Computer part_a;
    part_a.run(instructions);

    // Part B : register a = 1
    Computer part_b;
    part_b[0] = 1;
    part_b.run(instructions);

    return Output(part_a[dei.part_a_extra_param], part_b[dei.part_b_extra_param]);
}
