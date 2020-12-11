#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include <iostream>

#include <fstream>
#include <sstream>

class StringSplitter {
    std::string_view::const_iterator pos;
    std::string_view::const_iterator end;

public:
    explicit StringSplitter(const std::string_view & str) : pos(str.begin()), end(str.end()) {}

    std::string operator()() {
        const auto from = pos;

        while (pos != end && *pos != ' ') {
            ++pos;
        }

        const auto to = pos;

        while (pos != end && *pos == ' ') {
            ++pos;
        }

        return std::string(from, to);
    }
};

namespace lines_transform {
    inline std::vector<int> to_ints(const std::vector<std::string> & lines) {
        std::vector<int> values;

        for (const std::string & line : lines) {
            values.emplace_back(std::stoi(line));
        }

        return values;
    }

    template <typename T, typename Mapper>
    std::vector<T> map(const std::vector<std::string> & lines, Mapper mapper) {
        std::vector<T> retval;

        for (const std::string & line : lines) {
            retval.emplace_back(mapper(line));
        }

        return retval;
    }

}

