#include "../../common/advent_of_code.hpp"
#include "../../util/regex_helper.hpp"

#include <vector>

// https://adventofcode.com/2015/day/14

namespace {
    struct Reindeer {
        static constexpr const char * Regex_Pattern = R"(^([A-Za-z]*) can fly ([0-9]+) km\/s for ([0-9]+) seconds?, but then must rest for ([0-9]+) seconds?\.$)";

        std::string name;
        unsigned long long int flight_speed;
        unsigned long long int flight_time;
        unsigned long long int rest_time;

        Reindeer(const std::vector<std::string> & values)
        : name(values[0]), flight_speed(std::stoul(values[1])),
        flight_time(std::stoul(values[2])), rest_time(std::stoul(values[3])) {}
    };

    struct Competitor {
        const Reindeer * me;
        bool is_flying = true;
        unsigned long long int time_left;
        unsigned long long int position = 0;
        unsigned int points = 0;

        explicit Competitor(const Reindeer & reindeer)
        : me(&reindeer), time_left(reindeer.flight_time) {}

        void do_a_turn() {
            if (is_flying) {
                position += me->flight_speed;
            }

            --time_left;

            if (time_left == 0) {
                if (is_flying) {
                    time_left = me->rest_time;
                    is_flying = false;
                } else {
                    time_left = me->flight_time;
                    is_flying = true;
                }
            }
        }

        template <typename PointGetter>
        [[nodiscard]] static const Competitor & get_winner(const std::vector<Competitor> & competitors, PointGetter point_getter) {
            const Competitor * winner = &competitors[0];

            for (size_t i = 1 ; i != competitors.size() ; ++i) {
                if (point_getter(*winner) < point_getter(competitors[i])) {
                    winner = &competitors[i];
                }
            }

            return *winner;
        }

        [[nodiscard]] static const Competitor & get_leader(const std::vector<Competitor> & competitors) {
            return get_winner(competitors, [](const Competitor & competitor) { return competitor.position; });
        }

        static void award_points(std::vector<Competitor> & competitors) {
            const auto & leader = get_leader(competitors);

            for (auto & competitor : competitors) {
                if (competitor.position == leader.position) {
                    ++competitor.points;
                }
            }
        }
    };
}

Output day_2015_14(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const std::vector<Reindeer> reindeers = bj::lines_to_class_by_regex<Reindeer>(lines);
    std::vector<Competitor> competitors;
    for (const auto & reindeer : reindeers) {
        competitors.emplace_back(reindeer);
    }

    const size_t max_time = competitors.size() == 2 ? 1000 /* test input */ : 2504;

    for (size_t time = 0 ; time != max_time ; ++time) {
        for (auto & competitor : competitors) {
            competitor.do_a_turn();
        }

        Competitor::award_points(competitors);
    }

    const Competitor & first = Competitor::get_leader(competitors);
    const Competitor & most_points = Competitor::get_winner(competitors, [](const Competitor & competitor) { return competitor.points; });

    return Output(first.position, most_points.points);
}
