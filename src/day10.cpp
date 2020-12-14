#include "libs.hpp"

// https://adventofcode.com/2020/day/10


Output day10(const std::vector<std::string> & lines, const DayExtraInfo &) {
    std::vector<int> voltage = lines_transform::map<int>(lines, [](const std::string & s) { return std::stoi(s); });

    std::sort(voltage.begin(), voltage.end());
    voltage.insert(voltage.begin(), 0);
    voltage.push_back(voltage.back() + 3);

    unsigned int oneJoltDiff = 0;
    unsigned int threeJoltDifference = 0;

    for (auto it = voltage.begin() + 1 ; it != voltage.end() ; ++it) {
        const int difference = *it - *(it - 1);
        if (difference == 3) {
            ++threeJoltDifference;
        } else if (difference == 1) {
            ++oneJoltDiff;
        }
    }

    std::vector<long long unsigned int> number_of_ways_to_reach(voltage.size(), 0);

    number_of_ways_to_reach[0] = 1;

    for (size_t i = 1 ; i != voltage.size() ; ++i) {
        size_t previous_i = i - 1;

        while (voltage[previous_i] + 3 >= voltage[i]) {
            number_of_ways_to_reach[i] += number_of_ways_to_reach[previous_i];

            if (previous_i == 0) break;
            --previous_i;
        }
    }
    
    return Output(oneJoltDiff * threeJoltDifference, number_of_ways_to_reach.back());
}
