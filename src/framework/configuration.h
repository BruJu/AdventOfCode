#pragma once

#include "../libs.hpp"
#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <cstring>
#include <vector>

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

struct TestScore {
    unsigned int success = 0;
    unsigned int failed  = 0;

    TestScore & operator+=(const TestScore & other) {
        success += other.success;
        failed  += other.failed;
        return *this;
    }

    TestScore & operator+=(std::optional<bool> v) {
        if (v) {
            if (*v) {
                ++success;
            } else {
                ++failed;
            }
        }
        return *this;
    }

    [[nodiscard]] unsigned int total() const noexcept { return success + failed; }
};

struct InputConfig {
    using ExpectedType = std::pair<std::optional<int>, bool>;

    int day;
    std::string filename;
    Output::Optional expectedPart1 = std::nullopt; bool display1 = false;
    Output::Optional expectedPart2 = std::nullopt; bool display2 = false;

    InputConfig(int pDay, std::string pFileName, ExpectedType pPart1, ExpectedType pPart2)
    : day(pDay), filename(pFileName),
      expectedPart1(pPart1.first), display1(pPart1.second),
      expectedPart2(pPart2.first), display2(pPart2.second) {}

    static std::vector<InputConfig> read_configuration(const char * path = "config.txt");
    static InputConfig from_line(std::string_view line);

    template <typename Runner>
    TestScore run(Runner runner) const;

    [[nodiscard]] std::string to_string() const;
};

template <typename Runner> // runner(const std::vector<std::string> & lines) -> std::vector<int>
TestScore InputConfig::run(Runner runner) const {
    TestScore sc;

    // Task
    std::vector<std::string> lines;

    {
        std::ifstream file(filename);

        if (!file) {
            std::cout << "No file " << filename << "\n";
            return sc;
        }

        std::string line;
        while (getline_cleaned(file, line)) {
            lines.emplace_back(line);
        }
        // free file
    }

    Output result = runner(lines);

    // Display
    //std::cout << "== " << filename << '\n';

    const auto display_result = [&](Output::Optional expected, Output::Optional computed, char part) -> std::optional<bool> {
        // I really hate std::ostream, and std::format is not yet available in g++
        // So let's pretend it's C even thought that's unsafe

        char text[512];
        char buffer[512];

        // Macro because with a lambda, g++ is unhappy about buffer being a variable
#define SPRINTF_STRCAT1(format)       std::sprintf(buffer, format); strcat(text, buffer);
#define SPRINTF_STRCAT2(format, arg1) std::sprintf(buffer, format, arg1); strcat(text, buffer);

        std::sprintf(text, "-- Day %02d-%c  %-30s ", day, part, filename.c_str());

        if (!computed) {
            if (expected) {
                SPRINTF_STRCAT2("NO DATA - Expected= %lld", expected.value());
                std::cout << "\x1B[1m" << KRED << text << RST << '\n';
            }
            return false;
        }

        std::optional<bool> result;

        if (expected) {
            if (expected.value() == computed.value()) {
                SPRINTF_STRCAT1(" OK  ");
                result = true;
            } else {
                SPRINTF_STRCAT1("FAIL ");
                result = false;
            }
        } else {
            SPRINTF_STRCAT1("RSLT ");
            result = std::nullopt;
        }

        SPRINTF_STRCAT2("%lld", computed.value());

        if (expected && expected.value() != computed.value()) {
            SPRINTF_STRCAT2(" - Expected= %lld", expected.value());
        }

        if (!result)             std::cout << "\x1B[1m" << KBLU;
        else if (result.value()) std::cout << "\x1B[1m" << KGRN;
        else                     std::cout << "\x1B[1m" << KRED;

        std::cout << text << RST << '\n';
        return result;
    };

    if (display1) sc += display_result(expectedPart1, result.part1, 'A');
    if (display2) sc += display_result(expectedPart2, result.part2, 'B');

    return sc;
}

using InputsConfig = std::vector<InputConfig>;
