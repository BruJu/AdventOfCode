#include "libs.hpp"

#include <algorithm>
#include <vector>
#include <variant>
#include <memory>
#include <map>
#include <regex>

// https://adventofcode.com/2020/day/19


static const char * const RULE_DESCRIPTION = R"(^([0-9]*): (.*)$)";

using RuleNumber = int;

struct ComplexRule {
    std::vector<std::vector<RuleNumber>> next_rules;

    explicit ComplexRule(std::string_view str) {
        StringSplitter pipeSplitter { str, '|' };

        while (pipeSplitter) {
            std::string rule = pipeSplitter();
            next_rules.push_back({});

            StringSplitter valSplitter { rule, ' ' };

            while (valSplitter) {
                std::string rule_number_str = valSplitter();

                if (!rule_number_str.empty()) {
                    next_rules.back().emplace_back(std::stoi(rule_number_str));
                }
            }
        }
    }
};

struct Rule {
    using T = std::variant<char, ComplexRule>;
    T rule;

    Rule(char c) : rule(c) {}
    Rule(ComplexRule complex_rule) : rule(complex_rule) {}
};

class Automata {
    std::map<RuleNumber, Rule> m_rules;

public:
    explicit Automata(std::map<RuleNumber, Rule> rules) : m_rules(std::move(rules)) {}

    [[nodiscard]] static std::pair<Automata, std::vector<std::string>> make(const std::vector<std::string> & lines) {
        std::map<RuleNumber, Rule> rules;
        std::vector<std::string> leftovers;
        bool leftover = false;

        std::smatch matches;

        std::regex rule_description { RULE_DESCRIPTION };

        for (const std::string & line : lines) {
            if (line == "") {
                leftover = true;
            } else if (leftover) {
                leftovers.emplace_back(line);
            } else {
                std::regex_search(line, matches, rule_description);

                const RuleNumber rule_number = std::stoi(matches[1].str());
                const std::string rule_description = matches[2].str();

                if (rule_description.size() == 3 && rule_description[0] == '"') {
                    rules.insert_or_assign(rule_number, rule_description[1]);
                } else {
                    rules.insert_or_assign(rule_number, ComplexRule(rule_description));
                }
            }
        }

        return std::pair<Automata, std::vector<std::string>>(Automata(rules), std::move(leftovers));
    }

    [[nodiscard]] bool match(const std::string & word) const {
        return false;
    }
};




Output day19(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const auto [automata, words] = Automata::make(lines);

    test::Value matching_words = std::count_if(words.begin(), words.end(),
        [&](const std::string & word) { return automata.match(word); }
    );

    return Output(
        matching_words,
        0
    );
}
