#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <optional>

#include <fstream>

struct InputConfig {
    std::string filename;
    std::optional<int> expectedPart1 = std::nullopt;
    std::optional<int> expectedPart2 = std::nullopt;

    InputConfig(std::string pFileName, std::optional<int> pPart1, std::optional<int> pPart2)
    : filename(pFileName), expectedPart1(pPart1), expectedPart2(pPart2) {}

    static std::vector<InputConfig> read_configuration(const char * path = "config.txt");
    static InputConfig from_line(std::string_view line);
};

using InputsConfig = std::vector<InputConfig>;

class StringSplitter {
    std::string_view::const_iterator pos;
    std::string_view::const_iterator end;

public:
    explicit StringSplitter(const std::string_view & str) : pos(str.begin()), end(str.end()) {}

    std::string operator()() {
        const auto from = pos;

        while (pos != end && *pos != ' ') {
            ++pos;
        }

        const auto to = pos;

        while (pos != end && *pos == ' ') {
            ++pos;
        }

        return std::string(from, to);
    }
};

InputConfig InputConfig::from_line(std::string_view line) {
    constexpr auto toExpected = [](const std::string & s) -> std::optional<int> {
        if (s == "?") {
            return std::nullopt;
        } else {
            return std::stoi(s);
        }
    };

    StringSplitter splitter = StringSplitter(line);
    std::string filename = splitter();
    std::optional<int> expected1 = toExpected(splitter());
    std::optional<int> expected2 = toExpected(splitter());
    return InputConfig(filename, expected1, expected2);
}

inline InputsConfig InputConfig::read_configuration(const char * path) {

    InputsConfig configs;

    std::ifstream file(path);

    std::string line;
    while (std::getline(file, line)) {
        configs.emplace_back(from_line(line));
    }

    return configs;
}

