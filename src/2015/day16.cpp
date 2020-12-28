#include "../advent_of_code.hpp"
#include "../util/regex_helper.hpp"

#include <map>

// https://adventofcode.com/2015/day/16

namespace {
    struct Sue {
        static constexpr const char * Regex_Pattern = R"(^Sue ([0-9]+): ([a-z]*): ([0-9]+), ([a-z]*): ([0-9]+), ([a-z]*): ([0-9]+)$)";

        int number;
        std::map<std::string, int> properties;

        explicit Sue(const std::vector<std::string> & values) {
            number = std::stoi(values[0]);
            properties[values[1]] = std::stoi(values[2]);
            properties[values[3]] = std::stoi(values[4]);
            properties[values[5]] = std::stoi(values[6]);
        }
    };

    struct Required {
        enum class Operator { Eq, Lt, Gt };
        enum class SearchKind { Exact, WithOperator };

        struct Criterion {
            Operator op;
            int value;

            bool match(int sue_value, SearchKind sk) const {
                if (sk == SearchKind::Exact || op == Operator::Eq) {
                    return value == sue_value;
                } else if (op == Operator::Lt) {
                    return sue_value < value;
                } else /* if (op == Operator::Gt) */{
                    return sue_value > value;
                }
            }
        };

        std::map<std::string, Criterion> properties;
        
        Required() {
            properties["children"]    = Criterion { Operator::Eq, 3 };
            properties["cats"]        = Criterion { Operator::Gt, 7 };
            properties["samoyeds"]    = Criterion { Operator::Eq, 2 };
            properties["pomeranians"] = Criterion { Operator::Lt, 3 };
            properties["akitas"]      = Criterion { Operator::Eq, 0 };
            properties["vizslas"]     = Criterion { Operator::Eq, 0 };
            properties["goldfish"]    = Criterion { Operator::Lt, 5 };
            properties["trees"]       = Criterion { Operator::Gt, 3 };
            properties["cars"]        = Criterion { Operator::Eq, 2 };
            properties["perfumes"]    = Criterion { Operator::Eq, 1 };
        }

        [[nodiscard]] bool match(const Sue & sue, SearchKind search_kind) const {
            for (const auto & [name, value] : sue.properties) {
                const auto it = properties.find(name);
                if (it == properties.end()) {
                    std::cerr << "Invalid property in Sue " << sue.number << ": " << name << '\n';
                } else if (!it->second.match(value, search_kind)) {
                    return false;
                }
            }

            return true;
        }

        const Sue * search(const std::vector<Sue> & sues, SearchKind search_kind = SearchKind::Exact) const {
            for (const auto & sue : sues) {
                if (match(sue, search_kind)) {
                    return &sue;
                }
            }

            return nullptr;
        }
    };
}

Output day_2015_16(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const std::vector<Sue> sues = bj::lines_to_class_by_regex<Sue>(lines);
    const Required req;

    return Output(req.search(sues)->number, req.search(sues, Required::SearchKind::WithOperator)->number);
}
