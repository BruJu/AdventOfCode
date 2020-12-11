#pragma once

#include "../libs.hpp"
#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

struct InputConfig {
    int day;
    std::string filename;
    Output::Optional expectedPart1 = std::nullopt;
    Output::Optional expectedPart2 = std::nullopt;

    InputConfig(int pDay, std::string pFileName, Output::Optional pPart1, Output::Optional pPart2)
    : day(pDay), filename(pFileName), expectedPart1(pPart1), expectedPart2(pPart2) {}

    static std::vector<InputConfig> read_configuration(const char * path = "config.txt");
    static InputConfig from_line(std::string_view line);

    template <typename Runner>
    void run(Runner runner) const;

    [[nodiscard]] std::string to_string() const;
};

template <typename Runner> // runner(const std::vector<std::string> & lines) -> std::vector<int>
void InputConfig::run(Runner runner) const {
    // Task
    std::vector<std::string> lines;

    {
        std::ifstream file(filename);

        if (!file) {
            std::cout << "No file " << filename << "\n";
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            lines.emplace_back(line);
        }
        // free file
    }

    Output result = runner(lines);

    // Display
    std::cout << "== " << filename << '\n';

    constexpr auto display_result = [](Output::Optional expected, Output::Optional computed) {
        if (!computed) {
            if (expected) {
                std::cout << "NO DATA - Expected= "<< expected.value() << "\n";
            }
            return;
        }

        if (expected) {
            if (expected.value() == computed.value()) {
                std::cout << " OK  ";
            } else {
                std::cout << "FAIL ";
            }
        } else {
            std::cout << "RSLT ";
        }

        std::cout << computed.value();

        if (expected && expected.value() != computed.value()) {
            std::cout << " - Expected= " << expected.value() << "\n";
        } else {
            std::cout << "\n";
        }
    };

    display_result(expectedPart1, result.part1);
    display_result(expectedPart2, result.part2);
    std::cout << "\n";
}

using InputsConfig = std::vector<InputConfig>;
