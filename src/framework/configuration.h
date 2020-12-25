#pragma once

#include "../advent_of_code.hpp"
#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <cstring>
#include <vector>
#include <type_traits>
#include <chrono>

#include "../colors.h"

inline auto && getline_cleaned(std::ifstream & file, std::string & line) {
    auto && b = std::getline(file, line);

    if (b) {
        auto it = line.begin();
        while (it != line.end()) {
            if (*it == '\r') {
                it = line.erase(it);
            } else {
                ++it;
            }
        }
    }

    return b;
}

struct InputConfig {
    using ExpectedType = std::pair<std::optional<int>, bool>;

    int day;
    std::string filename;
    bool m_is_inline;
    test::Expected m_expected_part_1;
    test::Expected m_expected_part_2;

    InputConfig(int pDay, std::string pFileName) : day(pDay), filename(pFileName), m_is_inline(true),
    m_expected_part_1(test::Expected::Type::Inline), m_expected_part_2(test::Expected::Type::Inline) {}

    InputConfig(int pDay, std::string pFileName, test::Expected part1, test::Expected part2)
    : day(pDay), filename(pFileName), m_is_inline(false), m_expected_part_1(part1), m_expected_part_2(part2) {}

    static std::vector<InputConfig> read_configuration(int year);
    static InputConfig from_line(std::string_view line, const std::string & prefix);

    template <typename Runner> std::optional<test::RunResult> run(Runner runner) const;

    [[nodiscard]] std::string to_string() const;

    [[nodiscard]] static int last_day(const std::vector<InputConfig> & configs) {
        return std::max_element(configs.begin(), configs.end(),
            [](const auto & lhs, const auto & rhs) { return lhs.day < rhs.day; }
        )->day;
    }
};

template <typename Runner>
// requires (std::is_invocable_r<Output, Fn, const std::vector<std::string> &, const DayExtraInfo &>::value)
std::optional<test::RunResult> InputConfig::run(Runner runner) const {
    // Task
    std::vector<std::string> lines;

    {
        std::ifstream file(filename);

        if (!file) {
            std::cout << "No file " << filename << "\n";
            return std::nullopt;
        }

        std::string line;
        while (getline_cleaned(file, line)) {
            lines.emplace_back(line);
        }
        // free file
    }

    DayExtraInfo day_extra_info {
        .can_skip_part_A = m_expected_part_1.type == test::Expected::Type::Ignore,
        .can_skip_part_B = m_expected_part_2.type == test::Expected::Type::Ignore,
    };

    if (!m_is_inline) {
        const auto start = std::chrono::steady_clock::now();
        Output result = runner(lines, day_extra_info);
        const auto end = std::chrono::steady_clock::now();
        const auto elapsed_time = end - start;

        return test::RunResult {
            .parts = std::array<std::optional<test::PartResult>, 2>({
                test::PartResult::from(result.part_a, m_expected_part_1),
                test::PartResult::from(result.part_b, m_expected_part_2)
            }),
            .elapsed_time = elapsed_time
        };
    } else {
        const auto start = std::chrono::steady_clock::now();
        std::vector<std::string> part_a_messages;
        std::vector<std::string> part_b_messages;
        unsigned int valid_a = 0;
        unsigned int valid_b = 0;
        unsigned int total_a = 0;
        unsigned int total_b = 0;

        for (size_t i_line = 0; i_line + 1 < lines.size() ; i_line += 2) {
            std::vector<std::string> input;
            input.emplace_back(lines[i_line]);
            StringSplitter splitter { lines[i_line + 1] };
            const std::string expect_a = splitter ? splitter() : "";
            const std::string expect_b = splitter ? splitter() : "";

            Output result = runner(input, day_extra_info);

            if (expect_a != "" && expect_a != "_") {
                if (expect_a == result.part_a) {
                    ++valid_a;
                } else {
                    const std::string s =
                        ":: Part A :: Expected <" + expect_a +
                        "> ; Returned <" + result.part_a + ">" +
                        "\n" + lines[i_line];
                    part_a_messages.emplace_back(s);
                }
                ++total_a;
            }

            if (expect_b != "" && expect_b != "_") {
                if (expect_b == result.part_b) {
                    ++valid_b;
                } else {
                    const std::string s =
                        ":: Part A :: Expected <" + expect_b +
                        "> ; Returned <" + result.part_b + ">" +
                        "\n" + lines[i_line];
                    part_b_messages.emplace_back(s);
                }
                ++total_b;
            }
        }

        const auto end = std::chrono::steady_clock::now();
        const auto elapsed_time = end - start;

        return test::RunResult {
            .parts = std::array<std::optional<test::PartResult>, 2>({
                test::PartResult::from(valid_a, total_a, part_a_messages),
                test::PartResult::from(valid_b, total_b, part_b_messages)
            }),
            .elapsed_time = elapsed_time
        };
    }
}

using InputsConfig = std::vector<InputConfig>;
using DayEntryPoint = Output(const std::vector<std::string> & lines, const DayExtraInfo &);
