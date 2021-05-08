#include "../advent_of_code.hpp"
#include "../util/instruction_reader.hpp"
#include <algorithm>
#include <regex>
#include <array>
#include <variant>

// https://adventofcode.com/2016/day/8

namespace {
    // ==== Individual instructions

    struct DrawRect {
        DrawRect(int a, int b) : wide(a), tall(b) {}

        uint32_t wide;
        uint32_t tall;
    };

    struct RotateRow {
        RotateRow(int a, int b) : row(a), pixels(b) {}

        uint32_t row;
        uint32_t pixels;
    };

    struct RotateColumn {
        RotateColumn(int a, int b) : column(a), pixels(b) {}

        uint32_t column;
        uint32_t pixels;
    };

    // ==== std::variant to regroup the instructions

    using Instruction = std::variant<DrawRect, RotateRow, RotateColumn>;

    // ==== Surface

    class Surface {
        static constexpr size_t WIDE = 50;
        static constexpr size_t TALL = 6;
        std::array<bool, WIDE * TALL> m_lights;

        static size_t to_index(size_t row, size_t column) {
            row = row % TALL;
            column = column % WIDE;
            return row * WIDE + column;
        }

    public:
        Surface() { m_lights.fill(false); }

        // Instruction visitor

        void operator()(DrawRect draw) {
            for (size_t row = 0 ; row < draw.tall ; ++row) {
                for (size_t column = 0 ; column < draw.wide ; ++column) {
                    m_lights[to_index(row, column)] = true;
                }
            }
        }

        void operator()(RotateRow rotate) {
            const auto state = m_lights;
            
            for (size_t i = 0 ; i != WIDE ; ++i) {
                m_lights[to_index(rotate.row, i + rotate.pixels)] = state[to_index(rotate.row, i)];
            }
        }

        void operator()(RotateColumn rotate) {
            const auto state = m_lights;
            
            for (size_t i = 0 ; i != TALL ; ++i) {
                m_lights[to_index(i + rotate.pixels, rotate.column)] = state[to_index(i, rotate.column)];
            }
        }

        // Output

        [[nodiscard]] size_t count_lights() const noexcept {
            return std::count(m_lights.begin(), m_lights.end(), true);
        }

        friend std::ostream & operator<<(std::ostream & stream, const Surface & self) {
            for (size_t i = 0 ; i != TALL ; ++i) {
                for (size_t j = 0 ; j != WIDE ; ++j) {
                    if (self.m_lights[to_index(i, j)]) {
                        stream << "#";
                    } else {
                        stream << ".";
                    }
                }
                stream << "\n";
            }

            return stream;
        }
    };
}

Output day_2016_08(const std::vector<std::string> & lines, const DayExtraInfo &) {
    // Convert input
    bj::InstructionReader<Instruction> converter;

    converter.add_handler(
        R"(^rect ([0-9]*)x([0-9]*)$)",
        [](const std::vector<std::string> & values) -> Instruction { return DrawRect(std::stoi(values[0]), std::stoi(values[1])); }
    );

    converter.add_handler(
        R"(^rotate row y=([0-9]*) by ([0-9]*)$)",
        [](const std::vector<std::string> & values) -> Instruction { return RotateRow(std::stoi(values[0]), std::stoi(values[1])); }
    );

    converter.add_handler(
        R"(^rotate column x=([0-9]*) by ([0-9]*)$)",
        [](const std::vector<std::string> & values) -> Instruction { return RotateColumn(std::stoi(values[0]), std::stoi(values[1])); }
    );

    const auto result = converter(lines);

    // Run the instructions
    Surface surface;

    // As we have implemented Surface as an instruction visitor, we can just
    // visit every instruction.

    for (const auto & instruction : result.value()) {
        std::visit(surface, instruction);
    }

    // For part B
    std::cout << surface << std::flush;

    return Output(
        surface.count_lights(),
        0
    );
}
