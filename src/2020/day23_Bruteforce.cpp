#include "../advent_of_code.hpp"

#include <vector>
#include <algorithm>

// https://adventofcode.com/2020/day/23

using Cup = size_t;

static std::vector<Cup> read(const std::string & line) {
    std::vector<Cup> x;

    for (char c : line) {
        x.push_back(c - '0');
    }

    return x;
}

// Part A and part B are the same algorithm but part B is more efficient

static auto do_part_a(std::vector<Cup> cups, int rounds) {
    const Cup min_cup = *std::min_element(cups.begin(), cups.end());
    const Cup max_cup = *std::max_element(cups.begin(), cups.end());
    
    std::vector<Cup> removed;

    for (; rounds != 0 ; --rounds) {
        for (size_t i = 0 ; i != 4 ; ++i) {
            removed.push_back(cups.front());
            cups.erase(cups.begin());
        }

        Cup inserted_at_cup = removed[0];
        std::vector<Cup>::iterator insert_at;
        do {
            if (inserted_at_cup == min_cup) inserted_at_cup = max_cup;
            else --inserted_at_cup;

            insert_at = std::find(cups.begin(), cups.end(), inserted_at_cup);
        } while(insert_at == cups.end());

        ++insert_at;

        cups.insert(insert_at, removed.begin() + 1, removed.end());

        cups.push_back(removed[0]);
        removed.clear();
    }

    // Shift until 1 is at the first position
    while (cups[0] != 1) {
        Cup f = cups.front();
        cups.erase(cups.begin());
        cups.push_back(f);
    }

    long long int answer = 0;

    for (auto x : cups) {
        if (x == 1) continue;
        answer = answer * 10 + x;
    }

    return answer;
}

[[maybe_unused]] static void print_linked_elements(std::ostream & stream, const std::vector<size_t> & linked_list) {
    stream << "- As a map:\n";
    size_t i = 0;
    for (auto x : linked_list) {
        stream << i << ':' << x << ' ';
        ++i;
    }
    stream << '\n';

    stream << "- As a list:\n";
    std::optional<Cup> visited = std::nullopt;
    i = linked_list[0];
    for (; !visited || *visited != i; i = linked_list[i]) {
        stream << i << ' ';

        if (!visited) visited = i;
    }

    stream << "...\n";
}

static std::vector<size_t> to_linked_map(const std::vector<Cup> & cups, const Cup max_cup) {
    std::vector<size_t> linked_list;
    
    // Cup 0 is a pointer to the "first" element (for display)
    linked_list.push_back(cups.empty() ? 1 : cups[0]);

    // linked_list can now be a map : Cup -> the next Cup/Cup index
    for (Cup cup = 1 ; cup <= max_cup ; ++cup) {
        const auto cup_it = std::find(cups.begin(), cups.end(), cup);

        if (cup_it == cups.end()) {
            if (cup != max_cup) {
                linked_list.push_back(cup + 1);
            } else {
                linked_list.push_back(cups.empty() ? 1 : cups[0]);
            }

            continue;
        }

        const auto next_cup_it = cup_it + 1;
        if (next_cup_it != cups.end()) {
            linked_list.push_back(*next_cup_it);
            continue;
        }

        if (cup != max_cup) {
            linked_list.push_back(cups.size() + 1);
        } else {
            linked_list.push_back(cups.empty() ? 1 : cups[0]);
        }
    }

    return linked_list;
}

static auto do_part_b(const std::vector<Cup> & cups, const Cup max_cup, size_t number_of_rounds) {
    std::vector<size_t> linked_list = to_linked_map(cups, max_cup);

    size_t i = 0;
    while (--number_of_rounds != 0) {
        i = linked_list[i];

        std::array<Cup, 3> moved_cups;
        moved_cups[0] = linked_list[i];
        moved_cups[1] = linked_list[moved_cups[0]];
        moved_cups[2] = linked_list[moved_cups[1]];

        Cup moved_after = i;
        do {
            --moved_after;
            if (moved_after == 0) moved_after = max_cup;
        } while (std::find(moved_cups.begin(), moved_cups.end(), moved_after) != moved_cups.end());
        
        std::swap(linked_list[i]          , linked_list[moved_cups[2]]);
        std::swap(linked_list[moved_after], linked_list[moved_cups[2]]);
    }

    // if (max_cup < 100) print_linked_elements(std::cout, linked_list);
    
    return linked_list[1] * linked_list[linked_list[1]];
}

Output day_2020_23(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const std::vector<Cup> cups = read(lines[0]);
    const int loops = std::stoi(lines[1]);

    const auto part_a = do_part_a(cups, loops);

    // do_part_b(cups, cups.size() + 1, 100); (for testing purpose)
    const auto part_b = do_part_b(cups, 1000000, 10000000);
    
    return Output(part_a, part_b);
}
