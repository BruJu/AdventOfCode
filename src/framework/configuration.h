#pragma once

#include "../libs.hpp"
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
    test::Expected m_expected_part_1;
    test::Expected m_expected_part_2;

    InputConfig(int pDay, std::string pFileName, test::Expected part1, test::Expected part2)
    : day(pDay), filename(pFileName), m_expected_part_1(part1), m_expected_part_2(part2) {}

    static std::vector<InputConfig> read_configuration(const char * path = "config.txt");
    static InputConfig from_line(std::string_view line);

    template <typename Runner> std::optional<test::RunResult> run(Runner runner) const;

    [[nodiscard]] std::string to_string() const;
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
}

using InputsConfig = std::vector<InputConfig>;
