#include "configuration.h"
#include <sstream>
#include "../libs.hpp"

std::string InputConfig::to_string() const {
    std::ostringstream stringbuilder;

    stringbuilder << "Day " << day << " <" << filename << "> ";
    stringbuilder << '<' << m_expected_part_1 << "> <" << m_expected_part_2 << '>';
    
    return stringbuilder.str();
}

test::Expected::Expected(std::string line) {
    if (line == "?") {
        type  = Type::Wanted;
        value = 0;
    } else if (line == "_" || line.substr(0, 5) == "nope_") {
        type  = Type::Ignore;
        value = 0;
    } else if (line[0] == '?') {
        type  = Type::WantedKnown;
        value = std::stoll(line.substr(1));
    } else {
        type  = Type::Known;
        value = std::stoll(line);
    }
}

InputConfig InputConfig::from_line(std::string_view line) {
    StringSplitter splitter = StringSplitter(line);
    const int day = std::stoi(splitter());
    const std::string filename = splitter();
    const test::Expected expected1 = test::Expected(splitter());
    const test::Expected expected2 = test::Expected(splitter());
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

    for (auto it = configs.begin(); it != configs.end();) {
        if (it->m_expected_part_1.type == test::Expected::Type::Ignore
            && it->m_expected_part_2.type == test::Expected::Type::Ignore) {
            it = configs.erase(it);
        } else {
            ++it;
        }
    }

    return configs;
}
