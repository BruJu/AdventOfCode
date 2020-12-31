#include "../advent_of_code.hpp"

#include <set>
#include <stack>
#include <unordered_map>

// https://adventofcode.com/2015/day/19

namespace {
    struct Rule {
        std::string from;
        std::string to;

        explicit Rule(const std::string & str) {
            StringSplitter splitter { str, ' ' };
            from = splitter();
            splitter();
            to = splitter();
        }

        Rule(std::string from, std::string to) : from(from), to(to) {}

        friend std::ostream & operator<<(std::ostream & stream, const Rule & rule) {
            return stream << rule.from << " -> " << rule.to;
        }

        [[nodiscard]] int length_difference() const noexcept {
            return static_cast<int>(to.length()) - static_cast<int>(from.length());
        }
    };

    static std::vector<Rule> revert_rules(const std::vector<Rule> & rules) {
        std::vector<Rule> retval;
        for (const auto & rule : rules) {
            retval.emplace_back(rule.to, rule.from);
        }
        return retval;
    }

    class KnownWords {
    private:
        std::set<std::string> words;
        std::set<std::string> new_words;
        std::vector<Rule> rules;

    public:
        KnownWords(std::string word, std::vector<Rule> rules) : rules(std::move(rules)) {
            words.insert(word);
            new_words.insert(word);
        }

        void find_new_words() {
            std::set<std::string> discovered_words;

            for (const auto & rule : rules) {
                const size_t extract_length = rule.from.size();
                for (const auto & word : new_words) {
                    if (word.size() < extract_length) continue;

                    for (size_t i = 0 ; i <= word.length() - extract_length ; ++i) {
                        if (word.substr(i, extract_length) == rule.from) {
                            const std::string new_word = word.substr(0, i) + rule.to + word.substr(i + extract_length);

                            if (!words.contains(new_word)) {
                                discovered_words.insert(new_word);
                            }
                        }
                    }
                }
            }

            for (const auto & word : discovered_words) {
                words.insert(word);
            }

            new_words = std::move(discovered_words);
        }

        [[nodiscard]] auto number_of_words() const noexcept {
            return words.size();
        }

        [[nodiscard]] auto number_of_new_words() const noexcept {
            return new_words.size();
        }

        [[nodiscard]] static unsigned int search(std::string word, std::vector<Rule> rules, const std::string & searched) {
            // To slow
            KnownWords known_words { std::move(word), std::move(rules) };

            unsigned int steps = 0;

            std::set<std::string> visited_words;

            while (!known_words.words.contains(searched)) {
                known_words.find_new_words();
                ++steps;
                std::cout << known_words.number_of_new_words() << '\n';
            }

            return steps;
        }
    };
}

Output day_2015_19(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const auto [rules, initial_string] = [&]() {
        std::vector<Rule> rules;
        std::string str;

        bool empty_line = false;
        for (const auto & line : lines) {
            if (line == "") {
                empty_line = true;
            } else if (empty_line) {
                str = line;
            } else {
                rules.emplace_back(line);
            }
        }

        return std::pair<std::vector<Rule>, std::string>(rules, str);
    }();

    KnownWords known_words { initial_string, rules };
    known_words.find_new_words();
    const auto new_words = known_words.number_of_new_words();

    return Output(new_words, 0);
}
