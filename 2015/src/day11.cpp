#include "../../common/advent_of_code.hpp"

// https://adventofcode.com/2015/day/11

namespace {
    void next(std::string & password) {
        size_t i = password.size() - 1;

        while (true) {
            if (password[i] != 'z') {
                ++password[i];
                return;
            }

            password[i] = 'a';
            --i;
        }
    }

    bool valid(const std::string & password) {
        constexpr auto banned_letter = [](const std::string & pwd) {
            return std::any_of(pwd.begin(), pwd.end(), [](char c) {
                return c == 'o' || c == 'i' || c == 'l';
            });
        };

        constexpr auto no_inc_three_letters = [](const std::string & pwd) {
            for (size_t i = 2 ; i != pwd.size() ; ++i) {
                if (pwd[i] == pwd[i - 1] + 1 && pwd[i - 1] == pwd[i - 2] + 1) {
                    return false;
                }
            }

            return true;
        };

        constexpr auto has_two_double_letters = [](const std::string & pwd) {
            size_t count = 0;

            for (size_t i = 1 ; i < pwd.size() ; ++i) {
                if (pwd[i] == pwd[i - 1]) {
                    ++count;
                    ++i;
                }
            }

            return count >= 2;
        };

        return !banned_letter(password) && !no_inc_three_letters(password) && has_two_double_letters(password);
    }

    std::string find_next(std::string password) {
        do {
            next(password);
        } while (!valid(password));

        return password;
    }
}

Output day_2015_11(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const std::string current_password = lines[0];
    const std::string a = find_next(current_password);
    const std::string b = find_next(a);

    return Output(a, b);
}
