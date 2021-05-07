#include "../advent_of_code.hpp"
#include <array>
#include <regex>

// https://adventofcode.com/2016/day/3

// = Part A:
// Input is composed of lines of 3 length
// A triangle is possible if the sum of two smaller length is greater than the
// 3rd length.
//
// TA TA TA
// TB TB TB
// TC TC TC
// TD TD TD
// (...)
//
// = Part B:
// The lengths of a triangles are displayed in columns
//
// TA TB TC
// TA TB TC
// TA TB TC
// TD TE TF 
// TD TE TF
// TD TE TF

namespace {
    static const std::regex pattern { R"(^\s*([0-9]*)\s+([0-9]*)\s+([0-9]*)\s*$)" };

    std::array<int, 3> to_triangle(const std::string & line) {
        std::smatch matches;
        std::regex_search(line, matches, pattern);

        std::array<int, 3> result;
        result[0] = std::stoi(matches[1].str());
        result[1] = std::stoi(matches[2].str());
        result[2] = std::stoi(matches[3].str());

        return result;
    }

    template <typename Container>
    bool is_possible_triangle(Container & container) {
        std::sort(container.begin(), container.end());
        return container[0] + container[1] > container[2];
    }
}

Output day_2016_03(const std::vector<std::string> & lines, const DayExtraInfo &) {
    size_t possible_horizontal_triangles = 0;
    size_t possible_vertical_triangles = 0;

    std::array<std::vector<int>, 3> vertical_triangles;

    for (const auto & line : lines) {
        auto triangle = to_triangle(line);

        // Vertical triangles
        for (size_t i = 0 ; i != 3 ; ++i) {
            vertical_triangles[i].push_back(triangle[i]);

            if (vertical_triangles[i].size() == 3) {
                if (is_possible_triangle(vertical_triangles[i])) {
                    ++possible_vertical_triangles;
                }
                vertical_triangles[i].clear();
            }
        }

        // Horizontal triangles
        if (is_possible_triangle(triangle)) {
            ++possible_horizontal_triangles;
        }
    }

    return Output(
        possible_horizontal_triangles,
        possible_vertical_triangles
    );
}
