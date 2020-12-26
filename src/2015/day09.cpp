#include "../advent_of_code.hpp"

#include <set>
#include <map>

// https://adventofcode.com/2015/day/9

namespace {
    using City = std::string;

    struct Distances {
        std::map<std::pair<City, City>, int> m_data;

        void insert(City city1, City city2, int distance) {
            if (city1 > city2) {
                insert(city2, city1, distance);
                return;
            }

            m_data.insert_or_assign(std::pair<City, City>(city1, city2), distance);
        }

        int get(const City &  city1, const City & city2) const {
            if (city1 > city2) return get(city2, city1);
            return m_data.find(std::pair<City, City>(city1, city2))->second;
        }

        void flip() {
            for (auto & [city, distance] : m_data) {
                distance *= -1;
            }
        }
    }; 

    struct VisitableCity {
        City name;
        bool visited = false;
    };

    int find_smaller_path(const City & from, std::vector<VisitableCity> & visitables, const Distances & distances, int current, int current_best) {
        if (std::all_of(visitables.begin(), visitables.end(), [](const VisitableCity & c) { return c.visited; })) {
            return std::min(current, current_best); // current should always be < to current_best
        }

        for (auto & visitable : visitables) {
            if (visitable.visited) continue;

            const int distance = from == "" ? 0 : distances.get(from, visitable.name);

            if (distance > 0 && current + distance > current_best) continue;

            visitable.visited = true;

            const int best_in_branch = find_smaller_path(visitable.name, visitables, distances, current + distance, current_best);

            if (best_in_branch < current_best) {
                current_best = best_in_branch;
            }

            visitable.visited = false;
        }

        return current_best;
    }

    int find_smaller_path(const std::set<City> & cities, const Distances & distances) {
        std::vector<VisitableCity> visitable;
        for (const auto & city : cities) {
            visitable.push_back(VisitableCity { city });
        }

        return find_smaller_path("", visitable, distances, 0, 2100000000);
    }
}

Output day_2015_09(const std::vector<std::string> & lines, const DayExtraInfo &) {
    std::set<std::string> cities;
    Distances distances;

    for (const auto & line : lines) {
        StringSplitter splitter { line };

        const std::string city1 = splitter();
        splitter();
        const std::string city2 = splitter();
        splitter();
        const unsigned int distance = std::stoul(splitter());

        cities.insert(city1);
        cities.insert(city2);
        distances.insert(city1, city2, distance);
    }

    const auto smaller_path = find_smaller_path(cities, distances);
    // Find longest path = find smaller path if the distances are negatives!
    distances.flip();
    const auto longest_path = find_smaller_path(cities, distances);

    return Output(smaller_path, -longest_path);
}
