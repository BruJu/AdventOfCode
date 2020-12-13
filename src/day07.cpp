#include "libs.hpp"
#include <regex>

// https://adventofcode.com/2020/day/7

// == COMPLETE REGEX
// ^([a-z ]*) bags contain (no other bags|([0-9])+ ([a-z ]*) bags?(, ([0-9])+ ([a-z ]*) bags?)*)\.$

// https://stackoverflow.com/a/37004214
// "your capture group gets repeated by the pattern (you used the + quantifier
// on the surrounding non-capturing group), only the last value that matches
// it gets stored."

static constexpr const char * const BAGS_CONTAIN = R"(^([a-z ]*) bags contain ([a-z0-9, ]*)\.$)";
static constexpr const char * const CONTAINED_BAG = R"(([0-9]+) ([a-z ]*) bags?)" ;

struct NamedBag;

class Bag {
public:
    using All     = std::map<std::string, Bag>;
    using Content = std::map<std::string, unsigned int>;
private:
    Content m_contained_bags;
    bool is_transitively_closed = false;

public:
    static NamedBag New(std::string line, std::regex & regex_contain, std::regex & regex_contained);

    Bag() = default;
    explicit Bag(Content content) : m_contained_bags(std::move(content)) {}

    void ensure_is_transitively_closed(All & all_bags);

    [[nodiscard]] unsigned int get_number_of_bags() const noexcept {
        unsigned int total = 0;

        for (const auto & [_, one_kind_quantity] : m_contained_bags) {
            total += one_kind_quantity;
        }

        return total;
    }

    [[nodiscard]] unsigned int get_number_of(const std::string & bag_name) const noexcept {
        const auto it = m_contained_bags.find(bag_name);
        if (it == m_contained_bags.end()) return 0;
        return it->second;
    }

    friend std::ostream & operator<<(std::ostream & stream, const Bag::All & all);
};

std::ostream & operator<<(std::ostream & stream, const Bag::All & all) {
    for (const auto & [name, bag] : all) {

        stream << (bag.is_transitively_closed ? "CLOSED " : " OPEN  ")
               << name << ": ";

        for (const auto & [nameContained, quantity] : bag.m_contained_bags) {
            stream << quantity << ' ' << nameContained << " / ";
        }

        stream << '\n';
    }

    return stream;
}

struct NamedBag { std::string name; Bag bag; };


NamedBag Bag::New(std::string line, std::regex & regex_contain, std::regex & regex_contained) {
    std::smatch matches;

    std::regex_search(line, matches, regex_contain);
    std::string this_bag_name = matches[1].str();
    std::string content = matches[2].str();
    
    Bag::Content bag_content;
    while (std::regex_search(content, matches, regex_contained)) {
        unsigned int quantity = std::stoul(matches[1].str());
        std::string name = matches[2].str();

        bag_content[name] += quantity;

        content = matches.suffix();
    }

    return NamedBag{ this_bag_name, Bag(bag_content) };
}

void Bag::ensure_is_transitively_closed(Bag::All & all_bags) {
    if (is_transitively_closed) return;

    std::map<std::string, unsigned int> closure;

    for (const auto & [bag_name, quantity] : m_contained_bags) {
        closure[bag_name] += quantity;

        const auto itContainedBag = all_bags.find(bag_name);
        if (itContainedBag == all_bags.end()) continue;

        itContainedBag->second.ensure_is_transitively_closed(all_bags);

        for (const auto & [bag_in_bag_name, bag_in_bag_quantity] : itContainedBag->second.m_contained_bags) {
            closure[bag_in_bag_name] += bag_in_bag_quantity * quantity;
        }
    }

    m_contained_bags = std::move(closure);

    is_transitively_closed = true;
}

Output day07(const std::vector<std::string> & lines) {
    std::regex regex_bag_contains { BAGS_CONTAIN };
    std::regex regex_contained   { CONTAINED_BAG };

    const auto bag_map = [&](const std::string & line) {
        return Bag::New(line, regex_bag_contains, regex_contained);
    };

    Bag::All all_bags;
    for (auto && named_bag : lines_transform::map<NamedBag>(lines, bag_map)) {
        all_bags[named_bag.name] = std::move(named_bag.bag);
    }

    for (auto & [_name, bag] : all_bags) {
        bag.ensure_is_transitively_closed(all_bags);
    }

    // std::cout << all_bags;

    unsigned int shiny_gold_containers = 0;

    for (auto & [_name, bag] : all_bags) {
        if (bag.get_number_of("shiny gold") > 0) {
            ++shiny_gold_containers;
        }
    }

    return Output(shiny_gold_containers, all_bags["shiny gold"].get_number_of_bags());
}
