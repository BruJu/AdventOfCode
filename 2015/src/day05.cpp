#include "../../common/advent_of_code.hpp"

#include <set>

// https://adventofcode.com/2015/day/5

// ----

static bool three_vowels(const std::string & line) {
    size_t number_of_vowel = 0;

    for (char c : line) {
        switch (c) {
            case 'a':
            case 'e':
            case 'o':
            case 'u':
            case 'i':
                ++number_of_vowel;
        }
    }

    return number_of_vowel >= 3;
}

static bool twice_in_a_row(const std::string & line) {
    for (size_t i = 1 ; i != line.size(); ++i) {
        if (line[i] == line[i - 1]) {
            return true;
        }
    }


    return false;
}

static bool banned(const std::string & line) {
    for (const char * w : { "ab", "cd", "pq", "xy" }) {
        if (line.find(w) != std::string::npos) {
            return true;
        }
    }

    return false;
}

static bool is_nice(const std::string & line) {
    return three_vowels(line) && twice_in_a_row(line) && !banned(line);
}

// ----

static bool two_pair_two(const std::string & line) {
    std::set<std::string> seen;

    std::string last_seen = "";
    for (size_t i = 1 ; i < line.size(); ++i) {
        std::string current = line.substr(i - 1, 2);
        if (current == last_seen) {
            last_seen = "";
        } else {
            if (seen.contains(current)) {
                return true;
            }

            seen.insert(current);
            last_seen = current;
        }
    }

    return false;
}

static bool repeating_letter(const std::string & line) {
    for (size_t i = 2 ; i < line.size(); ++i) {
        if (line[i - 2] == line[i]) {
            return true;
        }
    }

    return false;
}

static bool is_nice_b(const std::string & line) {
    return two_pair_two(line) && repeating_letter(line);
}

// ----

Output day_2015_05(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const size_t total = std::count_if(lines.begin(), lines.end(), is_nice);
    const size_t total_b = std::count_if(lines.begin(), lines.end(), is_nice_b);

    return Output(total, total_b);
}
