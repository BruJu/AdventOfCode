#include "../advent_of_code.hpp"
#include <cstring>
#include <algorithm>
#include <array>
#include <map>
#include <queue>

// https://adventofcode.com/2016/day/11

namespace {
    enum class Kind {
        Generator,
        Microchip
    };

    class Factory;

    struct Element {
        const Factory * factory;
        size_t atom_id;
        Kind kind;

        [[nodiscard]] bool operator==(const Element & other) const {
            return atom_id == other.atom_id && kind == other.kind;
        }

        [[nodiscard]] bool operator<(const Element & other) const {
            return atom_id < other.atom_id || (atom_id == other.atom_id && kind == Kind::Generator && other.kind == Kind::Microchip);
        }
    
        [[nodiscard]] bool isGeneratorOf(const Element & chip) const {
            return atom_id == chip.atom_id && kind == Kind::Generator;
        }

        [[nodiscard]] bool canFry(const Element & chip) const {
            return atom_id != chip.atom_id && kind == Kind::Generator;
        }

        [[nodiscard]] std::string get_name() const;
    };

    class Factory {
        std::vector<std::string> known_elements;

        [[nodiscard]] size_t indexOf(const std::string & element_name) {
            for (size_t i = 0 ; i != known_elements.size() ; ++i) {
                if (known_elements[i] == element_name) {
                    return i;
                }
            }

            known_elements.push_back(element_name);
            return known_elements.size() - 1;
        }

    public:
        Element build(const std::string & element_name, Kind kind) {
            return Element {
                this,
                indexOf(element_name),
                kind
            };
        }

        [[nodiscard]] std::string get_element_name(size_t element_id) const {
            if (element_id >= known_elements.size()) return "#" + std::to_string(element_id);
            return known_elements[element_id];
        }

        [[nodiscard]] size_t get_number_of_elements() const {
            return known_elements.size();
        }
    };

    std::string Element::get_name() const {
        return factory->get_element_name(atom_id);
    }

    std::ostream & operator<<(std::ostream & stream, const Element & element) {
        return stream << "Element{ " <<
            element.get_name() << ", " <<
            (element.kind == Kind::Generator ? "Generator" : "Microchip")
            << " }";
    }

    static constexpr size_t none = std::string::npos;

    class State {
    private:
        const State * parent = nullptr;
        size_t elevator_position = 1;
        std::vector<std::vector<Element>> floors;

    public:
        explicit State(std::vector<std::vector<Element>> floors) : floors(std::move(floors)) {
            for (auto & floor : floors) {
                std::sort(floor.begin(), floor.end());
            }
        }

        State(const State & parent, bool goUp, std::array<size_t, 2> movedItems) {
            this->parent = &parent;
            elevator_position = parent.elevator_position;
            floors = parent.floors;

            if (goUp) {
                if (elevator_position == floors.size()){
                    return;
                }
                ++elevator_position;
            } else {
                if (elevator_position == 1) {
                    floors = parent.floors;
                    return;
                }
                --elevator_position;
            }

            transfer_items(
                floors[elevator_position - 1],
                floors[parent.elevator_position - 1],
                movedItems
            );
        }

        static void transfer_items(
            std::vector<Element> & destination,
            std::vector<Element> & source,
            std::array<size_t, 2> moved) {
            std::sort(moved.begin(), moved.end());

            for (auto it = moved.rbegin() ; it != moved.rend() ; ++it) {
                if (*it == none) continue;

                const auto moved_element = source[*it];
                source.erase(source.begin() + *it);
                destination.push_back(moved_element);
            }

            std::sort(destination.begin(), destination.end());
        }

        [[nodiscard]] bool leads_to_death() const {
            for (size_t i = 1 ; i < floors.size() ; ++i) {
                const auto & floor = floors[i];

                for (const auto & chip : floor) {
                    if (chip.kind == Kind::Microchip) {
                        const auto my_generator = std::find_if(floor.begin(), floor.end(),
                            [&](const Element & found) { return found.isGeneratorOf(chip); }
                        );
                        if (my_generator != floor.end()) continue;

                        const auto other_generator = std::find_if(floor.begin(), floor.end(),
                            [&](const Element & found) { return found.canFry(chip); }
                        );
                        if (other_generator != floor.end()) return true;
                    }
                }
            }

            return false;
        }

        [[nodiscard]] bool is_final_state() const {
            for (size_t i = 0 ; i != floors.size() - 1 ; ++i) {
                if (!floors[i].empty()) return false;
            }

            return true;
        }

        [[nodiscard]] bool operator==(const State & state) const {
            // No sure if required
            return elevator_position == state.elevator_position
                && floors == state.floors;
        }

        [[nodiscard]] bool operator<(const State & state) const {
            // For std::set
            if (elevator_position < state.elevator_position) return true;
            if (elevator_position > state.elevator_position) return false;
            if (floors.size() < state.floors.size()) return true;
            if (floors.size() > state.floors.size()) return false;

            for (size_t i = 0 ; i != floors.size() ; ++i) {
                if (floors[i].size() < state.floors[i].size()) return true;
                if (floors[i].size() > state.floors[i].size()) return false;

                for (size_t ei = 0 ; ei != floors[i].size() ; ++ei) {
                    if (floors[i][ei] < state.floors[i][ei]) return true;
                    if (state.floors[i][ei] < floors[i][ei]) return false;
                }
            }

            return false;
        }

        friend std::ostream & operator<<(std::ostream & stream, const State & state) {
            for (const auto & floor : state.floors) {
                for (const auto & element : floor) {
                    stream << element.get_name()[0];

                    if (element.kind == Kind::Generator) {
                        stream << "G";
                    } else {
                        stream << "M";
                    }
                }
                stream << '/';
            }

            stream << " death=" << state.leads_to_death();

            return stream;
        }
    
        template<typename Predicate>
        void filter_each_next_state(Predicate predicate) const {            
            const auto & floor = floors[elevator_position - 1];

            for (size_t i = 0 ; i != floor.size() ; ++i) {
                for (size_t j = i ; j != floor.size() ; ++j) {
                    std::array<size_t, 2> arr = { i, i == j ? none : j };

                    if (predicate(State(*this, true, arr)) || predicate(State(*this, false, arr))) {
                        return;
                    }
                }
            }
        }

        [[nodiscard]] size_t find_number_of_elements() const {
            for (const auto & floor : floors) {
                for (const auto & element : floor) {
                    return element.factory->get_number_of_elements();
                }
            }

            return 0;
        }

        [[nodiscard]] std::uint64_t minimize() const {
            // Doesn't work if [number_of_elements * (number of floors + 1)] >= 64
            size_t number_of_elements = find_number_of_elements();

            std::uint64_t result = elevator_position;

            for (const auto & chip_floor : floors) {
                for (const auto & generator_floor : floors) {
                    size_t value = 0;

                    for (const auto & element : chip_floor) {
                        if (element.kind != Kind::Microchip) continue;

                        const auto generator = std::find_if(generator_floor.begin(), generator_floor.end(),
                            [&](const Element & generator_e) { return generator_e.isGeneratorOf(element); }
                        );

                        if (generator != generator_floor.end()) {
                            ++value;
                        }
                    }

                    result = result * number_of_elements + value;
                }
            }

            return result;
        }
    };

}

[[maybe_unused]] std::ostream & operator<<(std::ostream & stream, const std::vector<std::vector<Element>> & elementss) {
    for (const auto & elements : elementss) {
        for (const auto & element : elements) {
            stream << element << " ";
        }

        stream << '\n';
    }

    return stream;
}

static std::vector<std::vector<Element>> read_input(const std::vector<std::string> & lines, Factory & factory) {
    std::vector<std::vector<Element>> floors;

    for (const std::string & line : lines) {
        const size_t contains_pos = line.find("contains");
        std::string elements_str = line.substr(contains_pos + std::strlen("contains"));

        if (elements_str == "nothing relevant.") {
            floors.push_back({});
            continue;
        }

        std::vector<Element> elems;
        size_t current = 0;

        while (true) {
            current = elements_str.find(" a ", current);

            if (current == std::string::npos) {
                break;
            }

            current += 3;

            size_t end = elements_str.find(",", current);
            if (end == std::string::npos) end = elements_str.find(" a ", current);
            if (end == std::string::npos) end = elements_str.find(".", current);
            if (end == std::string::npos) std::cerr << "Error when parsing the file\n";

            std::string element_name = elements_str.substr(current, end - current);
            if (element_name.substr(element_name.length() - 4) == " and") {
                element_name = element_name.substr(0, element_name.length() - 4);
            }

            if (element_name.find("generator") != std::string::npos) {
                elems.push_back(
                    factory.build(
                        element_name.substr(0, element_name.size() - std::strlen(" generator")),
                        Kind::Generator
                    )
                );
            } else if (element_name.find("-compatible microchip") != std::string::npos) {
                elems.push_back(
                    factory.build(
                        element_name.substr(0, element_name.size() - std::strlen("-compatible microchip")),
                        Kind::Microchip
                    )
                );
            } else {
                std::cerr << "Unexpected element " << element_name << '\n';
            }
        }

        floors.push_back(elems);
    }

    return floors;
}

static size_t solve(std::vector<std::vector<Element>> floors) {
    State state = State(floors);

    std::map<uint64_t, std::pair<State, size_t>> exploredStates;

    std::queue<std::pair<State, size_t>> todo;

    exploredStates.emplace(state.minimize(), std::pair<State, size_t>(state, 0));
    todo.push({ state, 0 });

    const auto maybe_add = [&](State state, size_t rank) -> std::optional<State> {
        const auto it = exploredStates.find(state.minimize());
        if (it != exploredStates.end()) { return std::nullopt; }
        if (state.leads_to_death())     { return std::nullopt; }
        if (state.is_final_state()) return state;

        exploredStates.emplace(state.minimize(), std::pair<State, size_t>(state, rank + 1));
        todo.push({ state, rank + 1 });
        return std::nullopt;
    };

    size_t max_seen_rank = 0;

    while (!todo.empty()) {
        const auto [state_, rank] = todo.front();
        todo.pop();

        if (rank > max_seen_rank) {
            max_seen_rank = rank;
        }

        const auto it = exploredStates.find(state_.minimize());
        const auto & state = it->second.first;


        std::optional<State> final_state = std::nullopt;

        const size_t r = rank; // Can't capture rank
        state.filter_each_next_state(
            [&](State state) {
                const auto end = maybe_add(std::move(state), r);
                if (end) {
                    final_state = end;
                    return true;
                }

                return false;
            }
        );

        if (final_state) {
            return rank + 1;
        }
    }

    return none;
}

Output day_2016_11(const std::vector<std::string> & lines, const DayExtraInfo &) {
    Factory factory;
    std::vector<std::vector<Element>> floors = read_input(lines, factory);

    const size_t result_a = solve(floors);

    floors[0].push_back(factory.build("elerium"  , Kind::Generator ));
    floors[0].push_back(factory.build("elerium"  , Kind::Microchip ));
    floors[0].push_back(factory.build("dilithium", Kind::Generator ));
    floors[0].push_back(factory.build("dilithium", Kind::Microchip ));
    
    const size_t result_b = solve(floors);

    return Output(
        result_a, result_b
    );
}
