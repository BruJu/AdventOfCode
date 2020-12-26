#include "../advent_of_code.hpp"
#include "../util/position.hpp"

#include <set>

// https://adventofcode.com/2015/day/3

namespace {
    struct Solo {
        bj::Position position;
        void move(bj::Direction direction) { position.move(direction); }
        [[nodiscard]] auto operator*() const { return position; }
    };

    struct Duo {
        bj::Position santa;
        bj::Position robot;
        bool robot_turn = true;

        void move(bj::Direction direction) {
            robot_turn = !robot_turn;
            if (robot_turn) robot.move(direction);
            else            santa.move(direction);
        }

        [[nodiscard]] auto operator*() const {
            if (robot_turn) return robot;
            else            return santa;
        }
    };

    template <typename VisitStrategy>
    struct Santa {
        std::set<bj::Position> m_visited_houses;
        VisitStrategy m_strategy;
        
        Santa() { m_visited_houses.insert(*m_strategy); }

        void move(const bj::Direction dir) {
            m_strategy.move(dir);
            m_visited_houses.insert(*m_strategy);
        }

        [[nodiscard]] size_t visited_houses() const noexcept { return m_visited_houses.size(); }
    };
}

Output day_2015_03(const std::vector<std::string> & lines, const DayExtraInfo &) {
    Santa<Solo> santa;
    Santa<Duo>  santa_and_robot;

    for (char c : lines[0]) {
        const bj::Direction dir = *bj::to_direction_from_lrtd(c, "<>^v");
        santa.move(dir);
        santa_and_robot.move(dir);
    }

    return Output(santa.visited_houses(), santa_and_robot.visited_houses());
}
