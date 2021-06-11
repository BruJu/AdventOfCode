#include "../advent_of_code.hpp"
#include <array>
#include <stack>

// https://adventofcode.com/2016/day/9

// Input are strings in the form "AZZA(2x3)EFG"
// These strings are encoded
// To decode, find the markers in the form (axb)
// (axb) means that the a characters after the marker must be repeated b times
// so (2x3)EFG must be decoded into EFEFEFG

// Part A: text inside the scope of a marker are considered as text
// Part B: Markers in the scope of a marker are still considered markers
//
// What is the length of the given decoded string?

namespace {
    /** Read the number written in str from position i  */
    size_t read_number(const std::string & str, size_t & i) {
        std::string s = "";

        while (str[i] >= '0' && str[i] <= '9') {
            s += str[i];
            ++i;
        }

        return std::stoi(s);
    }

    /**
     * Read the marker. A marker is something in the form (AxB).
     * Return the pair (A, B)
     */
    std::pair<size_t, size_t> read_marker(const std::string & str, size_t & i) {
        ++i; // (
        size_t data_length = read_number(str, i);
        ++i; // x
        size_t data_repetition = read_number(str, i);
        ++i; // )

        return std::pair(data_length, data_repetition);
    }

    /**
     * Decrypt the text and return the decrypted string.
     *
     * Markers that are affected by other markers are considered to be raw text
     */
    std::string decrypt(const std::string & original) {
        std::string result;

        size_t i = 0;

        while (i < original.size()) {
            if (original[i] != '(') {
                result += original[i];
                ++i;
            } else {
                const auto [data_length, data_repetition] = read_marker(original, i);

                for (size_t r = 0 ; r != data_repetition ; ++r) {
                    for (size_t l = 0 ; l != data_length ; ++l) {
                        result += original[i + l];
                    }
                }

                i += data_length;
            }
        }

        return result;
    }

    /**
     * Return the length of the decrypted string if markers affected by markers
     * are considered to still be markers.
     */
    size_t do_not_decrypt_recursive(const std::string & original) {
        size_t result = 0;

        size_t coef = 1;

        size_t i = 0;

        std::stack<std::pair<size_t, size_t>> markers;

        while (i < original.size()) {
            while (!markers.empty() && markers.top().first == i) {
                coef = markers.top().second;
                markers.pop();
            }

            if (original[i] != '(') {
                result += coef;
                ++i;
            } else {
                const auto [data_length, data_repetition] = read_marker(original, i);

                markers.push(std::pair<size_t, size_t>(i + data_length, coef));

                coef *= data_repetition;
            }
        }

        return result;
    }
    
    /** Add to s the recursively decoded string from original[start:end] */
    void decrypt_recursive_(std::string & s, const std::string & original, size_t start, size_t end) {
        size_t i = start;

        while (i < end) {
            if (original[i] != '(') {
                s += original[i];
                ++i;
            } else {
                const auto [data_length, data_repetition] = read_marker(original, i);

                for (size_t x = 0 ; x != data_repetition ; ++x) {
                    decrypt_recursive_(s, original, i, i + data_length);
                }

                i += data_length;
            }
        }
    }

    /**
     * Return the decrypted string if the markers are recursive.
     *
     * I was a bit surprised that my computer could handle this implementation.
     */
    [[maybe_unused]] std::string decrypt_recursive(const std::string & original) {
        std::string s;
        decrypt_recursive_(s, original, 0, original.length());
        return s;
    }
}

Output day_2016_09(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const auto result_a = decrypt(lines[0]).size();

    const size_t result_b = do_not_decrypt_recursive(lines[0]);
    // const auto result_b = decrypt_recursive(lines[0]).size();
    
    return Output(result_a, result_b);
}
