#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <optional>

#include <fstream>
#include <sstream>

struct InputConfig {
    std::string filename;
    std::optional<int> expectedPart1 = std::nullopt;
    std::optional<int> expectedPart2 = std::nullopt;

    InputConfig(std::string pFileName, std::optional<int> pPart1, std::optional<int> pPart2)
    : filename(pFileName), expectedPart1(pPart1), expectedPart2(pPart2) {}

    static std::vector<InputConfig> read_configuration(const char * path = "config.txt");
    static InputConfig from_line(std::string_view line);

    template <typename Runner>
    void run(Runner runner) const;

    [[nodiscard]] std::string to_string() const;
};


std::string InputConfig::to_string() const {
    std::ostringstream stringbuilder;

    stringbuilder << '<' << filename << "> ";

    const auto write_opt = [&](std::optional<int> v) {
        stringbuilder << '<';
        if (v) {
            stringbuilder << v.value();
        } else {
            stringbuilder << '?';
        }

        stringbuilder << '>';
    };

    write_opt(expectedPart1);
    stringbuilder << ' ';
    write_opt(expectedPart2);
    
    return stringbuilder.str();
}


template <typename Runner> // runner(const std::vector<std::string> & lines) -> std::vector<int>
void InputConfig::run(Runner runner) const {
    // Task
    std::vector<std::string> lines;

    {
        std::ifstream file(filename);

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

    //if (result.empty()) {
    //    std::cout << "/!\\ No result\n";
    //}

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

namespace lines_transform {

    inline std::vector<int> to_ints(const std::vector<std::string> & lines) {
        std::vector<int> values;

        for (const std::string & line : lines) {
            values.emplace_back(std::stoi(line));
        }

        return values;
    }


}

