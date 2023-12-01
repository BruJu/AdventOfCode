#include "../../common/advent_of_code.hpp"
#include <algorithm>
#include <set>

// https://adventofcode.com/2016/day/7

// Given IP in the IPv7 format (don't ask questions):
// letters[letters]letters(...)
//
// a) How many IPs of the list support TLS?
// TLS has at least 1 abba outside of brackets and no abba in brackets
// where a and b can be anything (but note equal)
//
// b) How many Ips of the list support SSL?
// SSL= there is at least one aba outside of brackets and a corresponding bab
// inside of brackets


namespace {
    // We split IPv7 into sequences of characters that can be either inside
    // or outside of brackets

    enum class Position { Bracket, Outside };

    using IPv7 = std::vector<std::pair<std::string, Position>>;

    IPv7 into_sequences(const std::string & ip) {
        std::vector<std::pair<std::string, Position>> r;

        Position p = Position::Outside;
        std::string current = "";

        const auto store = [&]() {
            if (current == "") return;

            r.push_back(std::pair<std::string, Position>(current, p));
            current = "";
        };
        const auto accumulate = [&](char c) { current += c; };
        const auto enter_brackets = [&]() { store(); p = Position::Bracket; };
        const auto exit_brackets = [&]() { store(); p = Position::Outside; };

        for (char c : ip) {
            switch (c) {
                case '[': enter_brackets(); break;
                case ']': exit_brackets();  break;
                default : accumulate(c);    break;
            }
        }

        store();

        return r;
    }

    // Part A

    bool has_abba(const std::string & str) {
        for (size_t i = 0 ; i + 3 < str.size() ; ++i) {
            if (str[i] == str[i + 3] && str[i + 1] == str[i + 2] && str[i] != str[i + 1]) {
                return true;
            }
        }

        return false;
    }

    bool supports_tls(const IPv7 & sequences) {
        bool has_tls = false;

        for (const auto & [subsequence, position] : sequences) {
            if (has_abba(subsequence)) {
                if (position == Position::Outside) {
                    has_tls = true;
                } else {
                    return false;
                }
            }
        }

        return has_tls;
    }

    // Part B

    void extract_aba(const std::string & str, std::set<std::string> & container) {
        for (size_t i = 0 ; i + 2 < str.size() ; ++i) {
            if (str[i] == str[i + 2] && str[i] != str[i + 1]) {
                std::string aba;
                aba = str[i];
                aba += str[i + 1];
                aba += str[i + 2];
                container.insert(aba);
            }
        }
    }
   
    std::string aba_to_bab(std::string str) {
        std::string r = "";
        r += str[1];
        r += str[0];
        r += str[1];
        return r;
    }

    bool supports_ssl(const IPv7 & sequences) {
        // Find every ABA (and BAB)
        std::set<std::string> outside;
        std::set<std::string> inside;

        for (const auto & [subsequence, position] : sequences) {
            if (position == Position::Outside) {
                extract_aba(subsequence, outside);
            } else {
                extract_aba(subsequence, inside);
            }
        }

        // Check if one of our ABA matches one of our BAB
        for (const auto & outside_element : outside) {
            const auto r = aba_to_bab(outside_element);
            if (inside.contains(r)) {
                return true;
            }
        }

        return false;
    }
}

Output day_2016_07(const std::vector<std::string> & lines, const DayExtraInfo &) {
    std::vector<IPv7> ipv7s;

    std::transform(
        lines.begin(), lines.end(),
        std::back_inserter(ipv7s),
        into_sequences
    );

    return Output(
        std::count_if(ipv7s.begin(), ipv7s.end(), supports_tls),
        std::count_if(ipv7s.begin(), ipv7s.end(), supports_ssl)
    );
}
