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

    [[nodiscard]] static int str_to_int(std::string s, char left) {
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

    explicit Seat(std::string s)
    : row   (Interval::str_to_int(s.substr(0, 7), 'F'))
    , column(Interval::str_to_int(s.substr(7, 3), 'L')) {}

    [[nodiscard]] int to_id() const noexcept { return row * 8 + column; }
};

Output day05(const std::vector<std::string> & lines) {
    std::vector<int> seats;
    for (const auto & line : lines) {
        seats.emplace_back(Seat(line).to_id());
    }

    // Part A :
    // const int max = *std::max_element(seats.begin(), seats.end());

    // Part B :
    // Dichotomic search is possible but only on a sorted array
    // As sorting is O(n logn) ... let's just use a O(n) algorithm

    std::sort(seats.begin(), seats.end());

    const int max = seats.back();

    size_t i = 1;
    while (i < seats.size() && seats[i] == seats[i - 1] + 1) {
        ++i;
    }

    return Output(max, seats[i] - 1);
}
