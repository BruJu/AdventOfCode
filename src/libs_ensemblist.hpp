#pragma once

#include <set>

template <typename T>
std::set<T> & operator&=(std::set<T> & lhs, const std::set<T> & rhs) {
    auto it = lhs.begin();
    while (it != lhs.end()) {
        if (rhs.contains(*it)) {
            ++it;
        } else {
            it = lhs.erase(it);
        }
    }
    return lhs;
}

template <typename T>
std::set<T> & operator^=(std::set<T> & lhs, const std::set<T> & rhs) {
    auto it = lhs.begin();
    while (it != lhs.end()) {
        if (!rhs.contains(*it)) {
            ++it;
        } else {
            it = lhs.erase(it);
        }
    }
    return lhs;
}

namespace set {
    template <typename T>
    std::set<T> intersection(const std::set<T> & lhs, const std::set<T> & rhs) {
        std::set<T> set;
        for (const auto & element : lhs) {
            if (rhs.contains(element)) {
                set.insert(element);
            }
        }
        return set;
    }

    template <typename T>
    std::set<T> union_(const std::set<T> & lhs, const std::set<T> & rhs) {
        std::set<T> set = lhs;
        set.insert(rhs.begin(), rhs.end());
        return set;
    }

    template <typename T>
    std::set<T> difference(const std::set<T> & lhs, const std::set<T> & rhs) {
        std::set<T> set;
        for (const auto & element : lhs) {
            if (!rhs.contains(element)) {
                set.insert(element);
            }
        }
        return set;
    }
}
