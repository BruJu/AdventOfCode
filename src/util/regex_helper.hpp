#pragma once

#include <regex>
#include <vector>

namespace bj {
    template <typename T>
    std::vector<T> lines_to_class_by_regex(const std::vector<std::string> & lines) {
        std::regex regex_ { T::Regex_Pattern };
        std::smatch smatch;

        std::vector<T> retval;

        for (const auto & line : lines) {
            std::regex_search(line, smatch, regex_);

            std::vector<std::string> values;

            for (int i = 1 ; i != smatch.length() ; ++i) {
                values.emplace_back(smatch[i].str());
            }

            retval.emplace_back(values);
        }

        return retval;
    }
}