#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"

#include <algorithm>
#include <array>
#include <numeric>
#include <regex>

// https://adventofcode.com/2015/day/6

namespace {
    enum class InstructionType { TurnOn, TurnOff, Toggle };

    using Brightness = long long int;

    class Instruction {
    public:
        InstructionType type;
        bj::Rectangle area;

        template <typename Collection>
        void apply(Collection & switch_lights) const {
            area.for_each_position([&](bj::Position position) {
                auto & light = switch_lights[position.x * 1000 + position.y];
                switch (type) {
                    case InstructionType::TurnOn:
                        light = 1;
                        break;
                    case InstructionType::TurnOff:
                        light = 0;
                        break;
                    case InstructionType::Toggle:
                        light = 1 - light;
                        break;
                }
            });
        }
        
        template <typename Collection>
        void apply_brightness(Collection & bright_lights) const {
            area.for_each_position([&](bj::Position position) {
                auto & light = bright_lights[position.x * 1000 + position.y];

                switch (type) {
                    case InstructionType::TurnOn:
                        light += 1;
                        break;
                    case InstructionType::TurnOff:
                        light = std::max<Brightness>(0, light - 1);
                        break;
                    case InstructionType::Toggle:
                        light += 2;
                        break;
                }
            });
        }
    };

    struct InstructionMaker {
        std::regex pattern { R"(^(turn off|turn on|toggle) ([0-9]+),([0-9]+) through ([0-9]+),([0-9]+)$)" } ;
        std::smatch matches;

        [[nodiscard]] Instruction operator()(const std::string & line) {
            std::regex_search(line, matches, pattern);

            InstructionType type;
            if (matches[1].str() == "turn on") {
                type = InstructionType::TurnOn;
            } else if (matches[1].str() == "turn off") {
                type = InstructionType::TurnOff;
            } else /* if (matches[1].str() == "toggle") */ {
                type = InstructionType::Toggle;
            }

            const int left   = std::stoi(matches[2].str());
            const int top    = std::stoi(matches[3].str());
            const int right  = std::stoi(matches[4].str());
            const int bottom = std::stoi(matches[5].str());

            return Instruction {
                type,
                bj::Rectangle(left, top, right, bottom)
            };
        }
    };
}

Output day_2015_06(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const std::vector<Instruction> instructions =
        lines_transform::map<Instruction>(lines, InstructionMaker{});

    std::vector<int /* bool */> lights;
    std::vector<Brightness>     bright_lights;

    for (size_t i = 0 ; i != 1000 * 1000 ; ++i) {
        lights.push_back(0);
        bright_lights.push_back(0);
    }
   
    for (const auto & instruction : instructions) {
        instruction.apply(lights);
        instruction.apply_brightness(bright_lights);
    }

    return Output(
        std::reduce(lights.begin(), lights.end()),
        std::reduce(bright_lights.begin(), bright_lights.end())
    );
}
