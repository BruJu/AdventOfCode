#include "../../common/advent_of_code.hpp"
#include <regex>

#include <set>
#include <vector>

// https://adventofcode.com/2015/day/13

namespace {
    using Person = std::string;
    using Position = int;

    struct Relationship {
        std::map<Person, std::map<Person, int>> m_happiness_gain;

        explicit Relationship(const std::vector<std::string> & lines) {
            std::regex regex { R"(^([A-Za-z]*) would (gain|lose) ([0-9]+) happiness units by sitting next to ([A-Za-z]*)\.$)" };
            std::smatch smatch_;

            for (const auto & line : lines) {
                std::regex_search(line, smatch_, regex);

                const auto person1 = smatch_[1].str();
                const auto person2 = smatch_[4].str();
                const auto is_gain = smatch_[2].str() == "gain";
                const auto value = std::stoi(smatch_[3].str());
                
                m_happiness_gain[person1][person2] = value * (is_gain ? 1 : -1);
            }
        }

        void add(Person subject, Person object, int gain) {
            m_happiness_gain[subject][object] = gain;
        }
    };

    struct GlobalHappinness {
        std::map<std::pair<Person, Person>, int> m_global_gain;

        explicit GlobalHappinness(const Relationship & relations) {
            for (const auto & [person, map] : relations.m_happiness_gain) {
                for (const auto & [other, gain] : map) {
                    add(person, other, gain);
                }
            }
        }

        void add(const Person & person, const Person & other, int gain) {
            const auto key = person < other ? std::pair<Person, Person>(person, other) : std::pair<Person, Person>(other, person);
            m_global_gain[key] += gain;
        }

        [[nodiscard]] int get_gain(const Person & p1, const Person & p2) const {
            if (p1 > p2) return get_gain(p2, p1);
            return m_global_gain.find(std::pair<Person, Person>(p1, p2))->second;
        }

        [[nodiscard]] std::vector<Person> get_persons() const {
            std::set<Person> persons;
            for (auto & [persons_in_pair, _] : m_global_gain) {
                persons.insert(persons_in_pair.first);
                persons.insert(persons_in_pair.second);
            }

            return std::vector<Person>(persons.begin(), persons.end());
        }

        void add(Person swiss_person) {
            for (const auto & other : get_persons()) {
                add(swiss_person, other, 0);
            }
        }
    };


    struct Explorer {
        const GlobalHappinness & m_happiness;

        // In general, I'm not fond of using const members in C++, but here, it acts more as
        // a local global variable than a real class, so I prefer to ensure that it is const
        // accross the use of the class over being able to copy
        const std::vector<Person> m_persons;
        std::vector<bool> m_has_been_assigned;

        std::vector<Person> m_places;

        Explorer(const GlobalHappinness & happiness, std::vector<Person> persons, std::vector<bool> has_been_assigned, std::vector<Person> places)
        : m_happiness(happiness), m_persons(persons), m_has_been_assigned(has_been_assigned), m_places(places) {
            m_has_been_assigned[0] = true;
            m_places.push_back(m_persons.front());
        }

        int compute_current_happiness() const {
            int total = 0;

            for (size_t i = 0 ; i != m_places.size() ; ++i) {
                const Person & me = m_places[i];
                const Person & you = m_places[(i + 1) % m_places.size()];
                total += m_happiness.get_gain(me, you);
            }

            return total;
        }

        int maximize_happiness() {
            if (m_places.size() == m_persons.size()) {
                return compute_current_happiness();
            }

            std::optional<int> best_local_happiness = std::nullopt;

            for (size_t i = 0 ; i != m_persons.size() ; ++i) {
                if (m_has_been_assigned[i]) continue;

                m_places.push_back(m_persons[i]);
                m_has_been_assigned[i] = true;

                const int happy = maximize_happiness();
                if (!best_local_happiness || happy > *best_local_happiness) {
                    best_local_happiness = happy;
                }

                m_has_been_assigned[i] = false;
                m_places.pop_back();
            }

            return *best_local_happiness;
        }
    };

    int maximize_happiness(const GlobalHappinness & global) {
        const auto persons = global.get_persons();
        std::vector<bool> assigned;
        for (size_t i = 0 ; i != persons.size() ; ++i) assigned.push_back(false);

        std::vector<Person> places;

        Explorer explorer { global, persons, assigned, places };
        return explorer.maximize_happiness();
    }
    
    int maximize_happiness(const GlobalHappinness & global, std::string new_person) {
        GlobalHappinness copy = global;
        copy.add(new_person);
        return maximize_happiness(copy);
    }
}

Output day_2015_13(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const Relationship relations { lines };
    const GlobalHappinness global { relations };

    const auto maximized_happiness = maximize_happiness(global);
    const auto maximized_happiness_with_me = maximize_happiness(global, "A swiss citizen");

    return Output(maximized_happiness, maximized_happiness_with_me);
}
