#include "libs.hpp"

// https://adventofcode.com/2020/day/5


struct Interval {
    int left;
    int right;

    void split(char c, char cLeft) {
        if (c == cLeft) {
            right = left + (right - left) / 2;
        } else {
            left = left  + (right - left) / 2;
        }
    }

    static int str_to_int(std::string s, char left) {
        Interval i { 0, 1 << s.size() };

        for (const char c : s) {
            i.split(c, left);
        }

        return i.left;
    }
};

struct Seat {
    int row;
    int column;

    [[nodiscard]] int to_id() const noexcept { return row * 8 + column; }
};

static Seat get_seat(std::string line) {
    return Seat {
        Interval::str_to_int(line.substr(0, 7), 'F'),
        Interval::str_to_int(line.substr(7, 3), 'L'),
    };
}


Output day05(const std::vector<std::string> & lines) {
    std::vector<int> seats;
    for (const auto & line : lines) {
        seats.push_back(get_seat(line).to_id());
    }

    std::sort(seats.begin(), seats.end());

    const int max = seats.back();

    // Dichotomic search is possible but only on a sorted array
    // As sorting is O(n logn) ... let's just use a O(n) algorithm
    size_t i = 1;
    while (i < seats.size() && seats[i] == seats[i - 1] + 1) {
        ++i;
    }

    return Output { max, seats[i] - 1 };
}
