#include "../../common/advent_of_code.hpp"

#include <array>
#include <unordered_set>

// https://adventofcode.com/2015/day/2

namespace {
    struct Dimension {
        int l;
        int w;
        int h;

        [[nodiscard]] static Dimension from(std::string_view line) {
            StringSplitter splitter { line, 'x' };
            Dimension d;
            d.l = std::stoi(splitter());
            d.w = std::stoi(splitter());
            d.h = std::stoi(splitter());
            return d;
        }

        [[nodiscard]] unsigned int required_paper() const noexcept {
            return 2 * (l * w + w * h + h * l) + std::min(std::min(l * w, w * h), h * l);
        }

        [[nodiscard]] unsigned int required_ribbon() const noexcept {
            return 2 * (l + w + h - std::max(std::max(l, w), h)) + l * w * h;
        }
    };
}

Output day_2015_02(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const std::vector<Dimension> dimensions = lines_transform::map<Dimension>(lines, Dimension::from);

    unsigned int paper_sum  = 0;
    unsigned int ribbon_sum = 0;
    for (const auto & dimension : dimensions) {
        paper_sum  += dimension.required_paper();
        ribbon_sum += dimension.required_ribbon();
    }

    return Output(paper_sum, ribbon_sum);
}
