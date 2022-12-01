#include "../advent_of_code.hpp"

#include <variant>
#include <regex>

// https://adventofcode.com/2015/day/7

namespace {
    enum class Operator { And, Or, LShift, Not, RShift, Assign };
    using Source = std::variant<std::uint16_t, std::string>;

    struct Formula {
        Source   lhs;
        Operator op;
        Source   rhs;

        explicit Formula(const std::vector<std::string> & values) {
            if (values.size() == 3) {
                /* VAL -> dest */
                lhs = std::uint16_t(0);
                rhs = to_value(values[0]);
                op = Operator::Assign;
            } else if (values.size() == 4) {
                /* NOT VAL -> dest */
                lhs = std::uint16_t(0);
                rhs = to_value(values[1]);
                op = Operator::Not;
            } else {
                lhs = to_value(values[0]);
                rhs = to_value(values[2]);

                if (values[1] == "AND") {
                    op = Operator::And;
                } else if (values[1] == "OR") {
                    op = Operator::Or;
                } else if (values[1] == "LSHIFT") {
                    op = Operator::LShift;
                } else if (values[1] == "RSHIFT") {
                    op = Operator::RShift;
                } else {
                    op = Operator::And;
                }
            }
        }
        
        /* implicit */ Formula(uint16_t value) : lhs(static_cast<std::uint16_t>(0)), op(Operator::Assign), rhs(value) {}

        [[nodiscard]] static Source to_value(const std::string & s) {
            if (s[0] >= '0' && s[0] <= '9') {
                return static_cast<std::uint16_t>(std::stoi(s));
            } else {
                return s;
            }
        }
    };

    struct WireValue {
        Formula                 formula;
        std::optional<uint16_t> value = std::nullopt;
    };

    class WireNetwork {
        std::map<std::string, WireValue> m_wires;

    public:
        explicit WireNetwork(const std::vector<std::string> & instructions) {
            for (const auto & wire_instruction : instructions) {
                std::vector<std::string> values = StringSplitter::line_to_vector<std::string>(
                    wire_instruction, [](const std::string & s) { return s; }, ' '
                );

                m_wires.emplace(values.back(), WireValue { Formula { values } });
            }
        }

        /// Replace the wire with a new formula
        void set_wire(const std::string & wire_name, const Formula & new_formula) {
            m_wires.insert_or_assign(wire_name, WireValue { new_formula });

            // Reset the wires value
            for (auto & [_wire_name, data] : m_wires) {
                data.value = std::nullopt;
            }
        }
        
        /// Returns the value of the wire
        [[nodiscard]] std::uint16_t get_wire(const std::string & wire_name) {
            ensure_has_value(wire_name);
            return m_wires.find(wire_name)->second.value.value();
        }

        /// Ensure the value of the wire is known
        void ensure_has_value(std::string wire_name) {
            if (auto & wire_value = m_wires.find(wire_name)->second; !wire_value.value.has_value()) {
                wire_value.value = evaluate(wire_value.formula);
            }
        }
        
        /// Returns the value of the formula (predecessing wires)
        uint16_t evaluate(const Formula & formula) {
            switch (formula.op) {
                case Operator::Assign: return  get_value(formula.rhs);
                case Operator::Not:    return ~get_value(formula.rhs);
                case Operator::And:    return  get_value(formula.lhs) &  get_value(formula.rhs);
                case Operator::Or:     return  get_value(formula.lhs) |  get_value(formula.rhs);
                case Operator::LShift: return  get_value(formula.lhs) << get_value(formula.rhs);
                case Operator::RShift: return  get_value(formula.lhs) >> get_value(formula.rhs);
                default:               return 0;
            }
        }

        /// Returns the constant if value is a constant, the value of the wire if its a wire.
        [[nodiscard]] uint16_t get_value(const Source & value) {
            if (const auto * int_value = std::get_if<uint16_t>(&value)) {
                return *int_value;
            } else if (const auto * wire = std::get_if<std::string>(&value)) {
                return get_wire(*wire);
            } else {
                return 0;
            }
        }
    };
}

Output day_2015_07(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
    WireNetwork wire_network { lines };

    if (dei.can_skip_part_B) {
        return Output(wire_network.get_wire("d"), 0);
    } else {
        const auto part_a = wire_network.get_wire("a");

        wire_network.set_wire("b", part_a);
        const auto part_b = wire_network.get_wire("a");

        return Output(part_a, part_b);
    }
}
