#include "../../common/advent_of_code.hpp"
#include <algorithm>
#include <unordered_map>
#include <regex>
#include <bit>

// https://adventofcode.com/2020/day/14

using Word = std::int64_t;  // We only require 35 bits thought

struct MaskChanger {
    Word                one_setter;
    Word                zero_setter;
    std::vector<size_t> joker_position;

    explicit MaskChanger(std::string mask) {
        for (size_t i = 0 ; i != mask.length() ; ++i) {
            if (mask[i] == 'X') {
                joker_position.emplace_back(mask.length() - i - 1);
            }
        }

        std::string copy = mask;
        
        std::replace_if(mask.begin(), mask.end(),
            [](char c) { return c == 'X'; }, '1'
        );
        std::replace_if(copy.begin(), copy.end(),
            [](char c) { return c == 'X'; }, '0'
        );

        zero_setter = std::stoull(mask, nullptr, 2);
        one_setter  = std::stoull(copy, nullptr, 2);
    }
};

struct Affectation {
    Word address;
    Word value;

    Affectation(const std::string & address, const std::string & value)
    : address(std::stoull(address)), value(std::stoull(value)) {}
};

struct UnknownOperation { std::string line; };

using Instruction = std::variant<MaskChanger, Affectation, UnknownOperation>;

class ProgramExecutor {
public:
    using Ram = std::unordered_map<Word, Word>;
private:
    Ram m_ram;
    const MaskChanger * m_mask_changer = nullptr;
    const std::vector<Instruction> * m_instructions;
    size_t m_pc = 0;

public:
    explicit ProgramExecutor(const std::vector<Instruction> & instructions)
    : m_instructions(&instructions){}

    [[nodiscard]] bool is_terminated() const { return m_pc == m_instructions->size(); }

    template<typename Affecter>
    bool execute(Affecter affecter) {
        if (is_terminated()) return false;

        const Instruction & instruction = (*m_instructions)[m_pc];

        struct Handler {
            ProgramExecutor * that;
            Affecter & affecter;

            void operator()(const MaskChanger & changer) {
                that->m_mask_changer = &changer;
            }

            void operator()(const UnknownOperation &) {}

            void operator()(const Affectation & affectation) {
                if (that->m_mask_changer) {
                    affecter(that->m_ram, affectation.address, affectation.value, *that->m_mask_changer);
                }
            }
        };

        std::visit(Handler{ this, affecter }, instruction);

        ++m_pc;
        return true;
    }

    [[nodiscard]] Word sum_of_memory() const noexcept {
        Word word = 0;
        for (const auto [_address, value] : m_ram) {
            word += value;
        }
        
        return word;
    }

    template <typename Affecter>
    static auto stand_alone_run(const std::vector<Instruction> & instructions, Affecter affecter) {
        ProgramExecutor executor = ProgramExecutor(instructions);
        while (executor.execute<Affecter>(affecter));
        return executor.sum_of_memory();
    }
};

void value_masker(ProgramExecutor::Ram & ram, Word address, Word value, const MaskChanger & mask) {
    ram[address] = (value & mask.zero_setter) | mask.one_setter;
}

void address_masker(ProgramExecutor::Ram & ram, Word address, Word value, const MaskChanger & mask) {
    address |= mask.one_setter;

    Word init_mask = ~0; // All bit at 1
    for (Word bit : mask.joker_position) {
        init_mask &= ~(1llu << bit); // Set to 0 the bit-th bit
    }

    address &= init_mask;

    size_t i = 0;
    while (true) {
        // Ascend
        if (i == mask.joker_position.size()) {
            // Leaf of the tree : modify ram
            ram[address] = value;
        } else {
            // address[i] == 0 so let's continue the ascention
            ++i;
            continue;
        }

        // Descend
        while (true) {
            if (i == 0) {
                // Trying to visit the parent of root = everything has been visited
                return;
            }

            // Go to parent
            --i;
            const Word i_mask = 1llu << mask.joker_position[i];
            const bool current_bit_at_1 = address & i_mask;

            if (current_bit_at_1) {
                // Just visited right (great)*-children
                address &= ~(i_mask);

                // Continue to descend
            } else {
                // Just visited every left (great)*-children : go to right child
                address |= i_mask;
                ++i;
                break;
            }
        }
    }
}

Output day_2020_14(const std::vector<std::string> & lines, const DayExtraInfo & extra) {
    std::regex mask_regex        { R"(mask = ([X10]*))" };
    std::regex affectation_regex { R"(mem\[([0-9]*)\] = ([0-9]*))" };

    const std::vector<Instruction> instructions = lines_transform::map<Instruction>(lines,
        [&](const std::string & line) -> Instruction {
            std::smatch smatch;

            if (std::regex_search(line, smatch, mask_regex)) {
                return MaskChanger(smatch[1].str());
            } else if (std::regex_search(line, smatch, affectation_regex)) {
                return Affectation(smatch[1].str(), smatch[2].str());
            } else {
                return UnknownOperation { line };
            }
        }
    );

    return Output(
        extra.can_skip_part_A ? 0 : ProgramExecutor::stand_alone_run(instructions, &value_masker),
        extra.can_skip_part_B ? 0 : ProgramExecutor::stand_alone_run(instructions, &address_masker)
    );
}
