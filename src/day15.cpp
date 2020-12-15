#include "libs.hpp"
#include <vector>
#include <map>

// https://adventofcode.com/2020/day/15


struct Occurrences {
    int lastTurn;
    int beforeTurn;

    [[nodiscard]] std::optional<int> get_turn_difference() const noexcept {
        if (beforeTurn == 0) {
            return std::nullopt;
        } else {
            return lastTurn - beforeTurn;
        }
    }
};

class RambunctiousRecitation {
    std::map<int, Occurrences> m_occurences;
    int m_turn_id = 0;
    int m_last_number = 0;
public:
    
    void add(int number) {
        ++m_turn_id;

        const auto it = m_occurences.find(number);
        if (it == m_occurences.end()) {
            m_occurences[number] = Occurrences{ m_turn_id , 0 };
        } else {
            it->second.beforeTurn = it->second.lastTurn;
            it->second.lastTurn = m_turn_id;
        }

        m_last_number = number;
    }

    void next() {
        const auto it = m_occurences.find(m_last_number);
        const auto diff = it->second.get_turn_difference();
        if (!diff) {
            add(0);
        } else {
            add(*diff);
        }
    }


    [[nodiscard]] bool is_turn(int turn_id) const noexcept {
        return m_turn_id == turn_id;
    }

    [[nodiscard]] int last_number() const noexcept { return m_last_number; }

};


Output day15(const std::vector<std::string> & lines, const DayExtraInfo & extra) {
    StringSplitter splitter = StringSplitter(lines[0], ',');

    RambunctiousRecitation instance;

    while (splitter) {
        instance.add(std::stoi(splitter()));
    }

    while (!instance.is_turn(2020)) {
        instance.next();
    }

    int _2020 = instance.last_number();
    

    while (!instance.is_turn(30000000)) {
        instance.next();
    }

    return Output(
        _2020,
        instance.last_number()
    );
}
