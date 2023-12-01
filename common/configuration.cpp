#include "configuration.h"
#include <sstream>
#include "advent_of_code.hpp"

std::string InputConfig::to_string() const {
    std::ostringstream stringbuilder;

    stringbuilder << "Day " << day << " <" << filename << "> ";
    stringbuilder << '<' << m_expected_part_1 << "> <" << m_expected_part_2 << '>';
    
    return stringbuilder.str();
}

test::Expected::Expected(std::string line) {
    if (const auto parameter = line.find("`"); parameter != std::string::npos) {
        extra_parameter = std::stoi(line.substr(0, parameter));
        line = line.substr(parameter + 1);
    }

    if (line == "?") {
        type  = Type::Wanted;
        value = "";
    } else if (line == "_" || line[0] == '$') {
        type  = Type::Ignore;
        value = "";
    } else if (line[0] == '?') {
        type  = Type::WantedKnown;
        value = line.substr(1);
    } else {
        type  = Type::Known;
        value = line;
    }
}

InputConfig InputConfig::from_line(std::string_view line, const std::string & prefix) {
    StringSplitter splitter = StringSplitter(line);
    const int day = std::stoi(splitter());
    const std::string filename = prefix + splitter();
    const std::string first_expected = splitter();
    if (first_expected == "inline") {
        return InputConfig(day, filename);
    } else {
        const test::Expected expected1 = test::Expected(first_expected);
        const test::Expected expected2 = test::Expected(splitter());
        return InputConfig(day, filename, expected1, expected2);
    }
}

static bool config_has_no_test(const InputConfig & config) {
    return config.m_expected_part_1.is_ignore()
        && config.m_expected_part_2.is_ignore();
}

InputsConfig InputConfig::read_configuration() {
    const std::string prefix = "inputs/";
    const std::string path = prefix + "config.txt";

    InputsConfig configs;

    std::ifstream file(path);

    std::string line;
    while (getline_cleaned(file, line)) {
        if (line.substr(0, 2) != "//") {
            configs.emplace_back(from_line(line, prefix));
        }
    }

    std::erase_if(configs, config_has_no_test);

    return configs;
}
