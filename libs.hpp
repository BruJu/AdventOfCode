#pragma once

#include <vector>
#include <string>
#include <optional>

#include <fstream>

struct InputConfig {
    std::string filename;
    std::optional<int> expectedPart1 = std::nullopt;
    std::optional<int> expectedPart2 = std::nullopt;

    InputConfig(std::string pFileName, std::optional<int> pPart1, std::optional<int> pPart2)
    : filename(pFileName), expectedPart1(pPart1), expectedPart2(pPart2) {}
};

using InputsConfig = std::vector<InputConfig>;

[[nodiscard]] inline std::vector<std::string> split(std::string line) {
    std::vector<std::string> retval;
    retval.push_back("input.tt");
    retval.push_back("7");
    retval.push_back("10");
    return retval;
}

inline InputsConfig readConfig() {
    constexpr auto toExpected = [](const std::string & s) -> std::optional<int> {
        if (s == "?") {
            return std::nullopt;
        } else {
            return std::stoi(s);
        }
    };

    InputsConfig configs;

    std::ifstream file("config.txt");

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> splitted = split(line);
        configs.emplace_back(splitted[0], toExpected(splitted[1]), toExpected(splitted[2]));
    }

    return configs;
}

