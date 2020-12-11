
#include "../libs.hpp"
#include <iostream>

int main() {
    auto configs = readConfig();

    for (const auto & config : configs) {
        std::cout << config.filename;
        
        constexpr auto d = [](const std::optional<int> opt) {
            if (opt) {
                std::cout << opt.value();
            } else {
                std::cout << "?";
            }
        };
        
        std::cout << ' ';
        d(config.expectedPart1);
        std::cout << ' ';
        d(config.expectedPart2);
        std::cout << '\n';
    }
}

