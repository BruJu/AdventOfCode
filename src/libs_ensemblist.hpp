#pragma once

#include <map>
#include <set>

/// Removes from lhs the elements that are not in rhs
template <typename T, typename OtherTs>
std::set<T> & operator&=(std::set<T> & lhs, const OtherTs & rhs) {
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

/// Removes from lhs the elements in rhs
template <typename T, typename OtherTs>
std::set<T> & operator^=(std::set<T> & lhs, const OtherTs & rhs) {
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

template <typename T, typename OtherTs>
std::set<T> & operator|=(std::set<T> & lhs, const OtherTs & rhs) {
    lhs.insert(rhs.begin(), rhs.end());
    return lhs;
}


namespace set {
    /// result = lhs intersection rhs
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

    /// result = lhs U rhs
    template <typename T>
    std::set<T> union_(const std::set<T> & lhs, const std::set<T> & rhs) {
        std::set<T> set = lhs;
        set.insert(rhs.begin(), rhs.end());
        return set;
    }

    /// result = lhs - rhs
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

    template <typename Ts>
    auto to_set(const Ts & map) {
        std::set<typename Ts::key_type> retval;

        for (const auto & [key, _value] : map) {
            retval.insert(key);
        }

        return retval;
    }

    /// Resolve the 1 Key = 1 Value mapping
    ///
    /// Input : a map 1 Key = N possibles values
    /// using the fact that 1 key is associated to only one possible value
    /// this function removes from every N possibles values the values that
    /// are already bounded to one key.
    template <typename Key, typename Value>
    void resolve_key_to_value(std::map<Key, std::set<Value>> & to_solve) {
        // O(n²) complexity
        std::set<Value> singletons;
        for (const auto & [key, values] : to_solve) {
            if (values.size() == 1) singletons.insert(*values.begin());
        }
        
        size_t singletons_size;
        do {
            singletons_size = singletons.size();

            for (auto & [key, values] : to_solve) {
                if (values.size() == 1) continue;
                
                values ^= singletons;

                if (values.size() == 1) {
                    singletons.insert(*values.begin());
                }
            }

        } while (singletons_size != singletons.size());
    }
}
