#include "configuration.h"
#include <sstream>
#include "../libs.hpp"

std::string InputConfig::to_string() const {
    std::ostringstream stringbuilder;

    stringbuilder << "Day " << day << " <" << filename << "> ";

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


InputConfig InputConfig::from_line(std::string_view line) {
    constexpr auto toExpected = [](const std::string & s) -> ExpectedType {
        if (s == "?" || s == "_") {
            return ExpectedType(std::nullopt, s == "?");
        } else {
            return ExpectedType(std::stoll(s), true);
        }
    };

    StringSplitter splitter = StringSplitter(line);
    const int day = std::stoi(splitter());
    const std::string filename = splitter();
    const ExpectedType expected1 = toExpected(splitter());
    const ExpectedType expected2 = toExpected(splitter());
    return InputConfig(day, filename, expected1, expected2);
}

InputsConfig InputConfig::read_configuration(const char * path) {
    InputsConfig configs;

    std::ifstream file(path);

    std::string line;
    while (getline_cleaned(file, line)) {
        if (line.substr(0, 2) != "//") {
            configs.emplace_back(from_line(line));
        }
    }

    return configs;
}
