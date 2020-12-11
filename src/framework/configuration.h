#pragma once

#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

struct InputConfig {
    int day;
    std::string filename;
    std::optional<int> expectedPart1 = std::nullopt;
    std::optional<int> expectedPart2 = std::nullopt;

    InputConfig(int pDay, std::string pFileName, std::optional<int> pPart1, std::optional<int> pPart2)
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

        
    std::vector<int> result = runner(lines);

    // Display
    std::cout << "== " << filename << '\n';
    if (result.size() > 2) {
        std::cout << "/!\\ More than 2 results (" << result.size()  << ")\n";
    }
    const auto display_result = [&result](std::optional<int> expected, size_t pos) {
        if (pos >= result.size()) {
            if (expected) {
                std::cout << "NO DATA (Expected= "<< expected.value() << ")\n";
            }
            return;
        }

        int computed = result[pos];

        if (expected) {
            if (expected.value() == computed) {
                std::cout << " OK  ";
            } else {
                std::cout << "FAIL ";
            }
        } else {
            std::cout << "RSLT ";
        }

        std::cout << computed;

        if (expected && expected.value() != computed) {
            std::cout << " - Expected= " << expected.value() << ")\n";
        } else {
            std::cout << "\n";
        }

    };

    display_result(expectedPart1, 0);
    display_result(expectedPart2, 1);
    std::cout << "\n";
}

using InputsConfig = std::vector<InputConfig>;
