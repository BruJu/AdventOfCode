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
    constexpr auto toExpected = [](const std::string & s) -> std::optional<int> {
        if (s == "?") {
            return std::nullopt;
        } else {
            return std::stoll(s);
        }
    };

    StringSplitter splitter = StringSplitter(line);
    const int day = std::stoi(splitter());
    const std::string filename = splitter();
    const std::optional<int> expected1 = toExpected(splitter());
    const std::optional<int> expected2 = toExpected(splitter());
    return InputConfig(day, filename, expected1, expected2);
}

InputsConfig InputConfig::read_configuration(const char * path) {
    InputsConfig configs;

    std::ifstream file(path);

    std::string line;
    while (std::getline(file, line)) {
        configs.emplace_back(from_line(line));
    }

    return configs;
}
