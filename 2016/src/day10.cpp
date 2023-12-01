#include "../../common/advent_of_code.hpp"
#include "../../util/instruction_reader.hpp"
#include <algorithm>
#include <regex>
#include <array>
#include <variant>
#include <stack>

// https://adventofcode.com/2016/day/10

namespace {
    class Taker {
    public:
        virtual void take(int32_t value) = 0;
    };

    class Robot : public Taker {
        std::optional<int32_t> value1;
        std::optional<int32_t> value2;

    public:
        void take(int32_t value) override {
            if (value1 && value == *value1) return;
            if (value2 && value == *value2) return;

            if (!value1) value1 = value;
            else if (!value2) value2 = value;
            else std::cerr << "Robot have more than 2 values\n";
        }

        std::optional<std::pair<int32_t, int32_t>> get_values() const {
            if (!value1 || !value2) return std::nullopt;

            if (*value1 < *value2) return std::pair<int32_t, int32_t>{ *value1, *value2 };
            else return                   std::pair<int32_t, int32_t>{ *value2, *value1 };
        }
    };

    class Output_ : public Taker {
        std::optional<int32_t> m_value;

    public:
        void take(int32_t value) override {
            if (m_value && *m_value != value) return;
            m_value = value;
        }

        int32_t operator*() {
            return m_value.value();
        }
    };

    struct ValueAction {
        int32_t value;
        int32_t bot;
    };

    struct BotAction {
        int32_t bot;
        bool dest_low_bot;
        int32_t dest_low_number;
        bool dest_high_bot;
        int32_t dest_high_number;
    };

    using Instruction = std::variant<ValueAction, BotAction>;

    class Factory {
        std::map<int32_t, Robot > bots;
        std::map<int32_t, Output_> outputs;

        Taker & find(bool is_bot, int32_t target) {
            if (is_bot) {
                return bots[target];
            } else {
                return outputs[target];
            }
        }

    public:
        // ==== VISITOR

        // The visitor return true if the instruction have been processed,
        // false if the system couldn't process it.

        bool operator()(ValueAction va) {
            bots[va.bot].take(va.value);
            return true;
        }

        bool operator()(BotAction ba) {
            auto & bot = bots[ba.bot];

            const auto values = bot.get_values();
            if (!values) {
                return false;
            }

            find(ba.dest_low_bot , ba.dest_low_number ).take(values->first);
            find(ba.dest_high_bot, ba.dest_high_number).take(values->second);
            return true;
        }

        // ==== OUTPUT

        std::optional<int32_t> find_responsible_for(int32_t chip1, int32_t chip2) {
            const std::pair<int32_t, int32_t> pair = std::pair<int32_t, int32_t>(chip1, chip2);
            for (const auto & [id, robot] : bots) {
                if (robot.get_values() == pair) {
                    return id;
                }
            }

            return std::nullopt;
        }

        int32_t find_part_b() {
            return *outputs[0] * *outputs[1] * *outputs[2];
        }
    };
}

Output day_2016_10(const std::vector<std::string> & lines, const DayExtraInfo &) {
    bj::InstructionReader<Instruction> ir;

    ir.add_handler(
        R"(^value ([0-9]*) goes to bot ([0-9]*)$)",
        [](const std::vector<std::string> & values) -> Instruction {
            return ValueAction{ std::stoi(values[0]), std::stoi(values[1]) };
        }
    );

    ir.add_handler(
        R"(^bot ([0-9]*) gives low to (bot|output) ([0-9]*) and high to (bot|output) ([0-9]*)$)",
        [](const std::vector<std::string> & values) -> Instruction {
            return BotAction {
                std::stoi(values[0]),
                values[1] == "bot", std::stoi(values[2]),
                values[3] == "bot", std::stoi(values[4])
            };
        }
    );

    auto instructions = ir(lines).value();

    Factory factory;

    // Assign instructions until they have all been used
    while (instructions.begin() != instructions.end()) {
        auto it = instructions.begin();
        while (it != instructions.end()) {
            const bool ok = std::visit(factory, *it);
            if (ok) {
                it = instructions.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    return Output(
        factory.find_responsible_for(17, 61).value(),
        factory.find_part_b()
    );
}
