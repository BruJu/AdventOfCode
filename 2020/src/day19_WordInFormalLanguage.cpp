#include "../../common/advent_of_code.hpp"

#include <algorithm>
#include <vector>
#include <variant>
#include <memory>
#include <map>
#include <regex>

// https://adventofcode.com/2020/day/19

static const char * const RULE_DESCRIPTION = R"(^([0-9]*): (.*)$)";

// ============================================================================

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

    friend std::ostream & operator<<(std::ostream & stream, const ComplexRule & cr) {
        return into_stream(stream, cr.next_rules, " | ", [](std::ostream & stream, const auto & rules)  {
            into_stream(stream, rules, ">",  [](std::ostream & stream, RuleNumber rule_number) {
                stream << rule_number;
            });
        });
    }
};

using Rule = std::variant<char, ComplexRule>;

void in_stream(std::ostream & stream, const Rule & rule) {
    if (const char * c = std::get_if<char>(&rule)) {
        stream << '"' << *c << '"';
    } else {
        const ComplexRule & r = *std::get_if<ComplexRule>(&rule);
        stream << r;
    }
}

using Ruleset = std::map<RuleNumber, Rule>;

// ============================================================================

struct DeterministicState {
    std::vector<RuleNumber> next_rules;

    DeterministicState() {}

    DeterministicState(RuleNumber rule_number, const std::vector<RuleNumber> & followed_by) {
        next_rules.reserve(followed_by.size() + 1);
        next_rules.push_back(rule_number);
        next_rules.insert(next_rules.end(), followed_by.begin(), followed_by.end());
    }

    friend std::ostream & operator<<(std::ostream & stream, const DeterministicState & state) {
        return into_stream(stream, state.next_rules, ">",  [](std::ostream & stream, RuleNumber rule_number) {
            stream << rule_number;
        });
    }
};

// No deterministic
class State {
    const Ruleset & m_rules;
    std::vector<DeterministicState> m_inner_states;

public:
    State(const Ruleset & rules, RuleNumber start_state) : m_rules(rules) {
        push_states_from(start_state, {});
    }

    void push_states_from(RuleNumber rule_number, std::vector<RuleNumber> followed_by) {
        const Rule & rule = m_rules.find(rule_number)->second;

        if (std::get_if<char>(&rule)) {
            m_inner_states.emplace_back(rule_number, followed_by);
        } else {
            const ComplexRule & r = *std::get_if<ComplexRule>(&rule);

            std::vector<RuleNumber> follow_next;
            for (const std::vector<RuleNumber> & nexts : r.next_rules) {
                follow_next.clear();
                follow_next.insert(follow_next.end(), nexts.begin() + 1, nexts.end());
                follow_next.insert(follow_next.end(), followed_by.begin(), followed_by.end());

                push_states_from(nexts[0], follow_next);
            }
        }
    }

    void read(char c) {
        std::vector<DeterministicState> states = std::move(m_inner_states);

        for (const DeterministicState & state : states) {
            if (state.next_rules.empty()) continue;
            // We are guaranted by construction that state.next_rules[0] is a char

            const Rule & rule = m_rules.find(state.next_rules[0])->second;
            char state_expect = std::get<char>(rule);

            if (state_expect != c) continue;

            if (state.next_rules.size() == 1) {
                m_inner_states.push_back({});
                continue;
            }

            std::vector<RuleNumber> follow_next;
            follow_next.insert(follow_next.end(), state.next_rules.begin() + 2, state.next_rules.end());

            push_states_from(state.next_rules[1], follow_next);
        }
    }

    [[nodiscard]] bool is_on_terminate() const noexcept {
        return std::any_of(m_inner_states.begin(), m_inner_states.end(),
            [](const DeterministicState & state) { return state.next_rules.empty(); }
        );
    }

    friend std::ostream & operator<<(std::ostream & stream, const State & state) {
        stream << "== Current State\n";

        for (const auto & det_state : state.m_inner_states) {
            stream << det_state << '\n';
        }

        return stream;
    }
};

// ============================================================================

class Automata {
    Ruleset m_rules;

public:
    explicit Automata(Ruleset rules) : m_rules(std::move(rules)) {}

    [[nodiscard]] static std::pair<Automata, std::vector<std::string>> make(const std::vector<std::string> & lines) {
        Ruleset rules;
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
        State state { m_rules, 0 };

        for (size_t i = 0 ; i != word.size() ; ++i) {
            state.read(word[i]);
        }

        return state.is_on_terminate();
    }

    friend std::ostream & operator<<(std::ostream & stream, const Automata & nier) {
        stream << "== Automata\n";

        for (const auto & [rule_number, rules] : nier.m_rules) {
            stream << rule_number << ": ";
            in_stream(stream, rules);
            stream << '\n';
        }

        return stream;
    }
};

// ============================================================================

Output day_2020_19(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const auto [automata, words] = Automata::make(lines);

    const auto matching_words = std::count_if(words.begin(), words.end(),
        [&](const std::string & word) { return automata.match(word); }
    );
    
    return Output(matching_words, matching_words);
}
