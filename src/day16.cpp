#include "libs.hpp"
#include <unordered_map>
#include <vector>
#include <set>
#include <map>
#include <regex>
#include <algorithm>

// https://adventofcode.com/2020/day/16


struct Restriction {
    int min;
    int max;

    [[nodiscard]] constexpr bool operator()(int value) const noexcept {
        return min <= value && value <= max;
    }

    friend std::ostream & operator<<(std::ostream & stream, const Restriction & restriction) {
        return stream << restriction.min << '-' << restriction.max;
    }

    static bool match_restriction_2(const std::array<Restriction, 2> & restrictionx2, int value) {
        return restrictionx2[0](value) || restrictionx2[1](value);
    }
};

using Restriction2 = std::array<Restriction, 2>;

struct Ticket {
    std::vector<int> values;

    Ticket() = default;
    Ticket(const std::string & line)
    : values(StringSplitter::line_to_vector<int>(line, to_int, ',')) {}

    friend std::ostream & operator<<(std::ostream & stream, const Ticket & ticket) {
        bool first = true;
        for (int value : ticket.values) {
            if (!first) stream << '-';
            first = false;
            stream << value;
        }
        return stream;
    }

    static int to_int(const std::string & str) { return std::stoi(str); }

    [[nodiscard]] std::optional<size_t> find_unmatching_field(const std::vector<Restriction2> & restrictions) const {
        const auto invalid_field = std::find_if(values.begin(), values.end(), [&](int value) {
            return std::all_of(restrictions.begin(), restrictions.end(), [&](const auto & restrictionx2) {
                return !Restriction::match_restriction_2(restrictionx2, value);
            });
        });

        if (invalid_field == values.end()) return std::nullopt;
        return invalid_field - values.begin();
    }

    int operator[](size_t position) const { return values[position]; }
};


class TicketTranslation {
    std::vector<std::string> m_field_names;
    std::vector<Restriction2> m_restrictions;

    Ticket my_ticket;
    std::vector<Ticket> m_other_tickets;
    std::optional<std::vector<size_t>> m_position_in_ticket_to_field = std::nullopt;

public:
    explicit TicketTranslation(const std::vector<std::string> & lines) {
        enum class State { Fields, MyTicket, OtherTickets };

        State current_state = State::Fields;

        std::regex field_regex { R"(([a-z ]*): ([0-9]*)-([0-9]*) or ([0-9]*)-([0-9]*))" };

        for (const std::string & line : lines) {
            if (line == "your ticket:") {
                current_state = State::MyTicket;
            } else if (line == "nearby tickets:") {
                current_state = State::OtherTickets;
            } else if (line == "") {
            } else if (current_state == State::Fields) {
                std::smatch matches;
                std::regex_search(line, matches, field_regex);

                m_field_names.push_back(matches[1].str());

                std::array<Restriction, 2> restrictions;
                restrictions[0] = Restriction { std::stoi(matches[2].str()), std::stoi(matches[3].str()) };
                restrictions[1] = Restriction { std::stoi(matches[4].str()), std::stoi(matches[5].str()) };
                m_restrictions.push_back(restrictions);
            } else if (current_state == State::MyTicket) {
                my_ticket = Ticket(line);
                m_other_tickets.push_back(my_ticket);
            } else if (current_state == State::OtherTickets) {
                m_other_tickets.emplace_back(line);
            }
        }
    }

    test::Value remove_invalid_tickets() {
        test::Value error_rate = 0;

        std::erase_if(m_other_tickets,
            [&](const Ticket & ticket) {
                const auto unmatch = ticket.find_unmatching_field(m_restrictions);
                if (unmatch) {
                    error_rate += ticket[*unmatch];
                }
                return unmatch.has_value();
            }
        );

        return error_rate;
    }

    void deduce_fields() {
        std::vector<std::vector<size_t>> possibilitiess = [](size_t size) {
            std::vector<std::vector<size_t>> retval;

            for (size_t i = 0 ; i != size ; ++i) {
                retval.push_back({});
                for (size_t j = 0 ; j != size ; ++j) {
                    retval.back().push_back(j);
                }
            }

            return retval;
        }(m_field_names.size());
        
        for (const Ticket & ticket : m_other_tickets) {
            for (size_t c = 0 ; c != possibilitiess.size() ; ++c) {
                std::vector<size_t> & allowed_fields = possibilitiess[c];

                std::erase_if(allowed_fields, [&](size_t field_id) {
                    return !Restriction::match_restriction_2(m_restrictions[field_id], ticket[c]);
                });
            }
        }

        // There are more efficient way to do it, but there are only 20 elements
        // so let's go for a O(n²) complexity
        std::set<size_t> singletons;
        for (const auto & column : possibilitiess) {
            if (column.size() == 1) singletons.insert(column[0]);
        }
        
        size_t singletons_size;
        do {
            singletons_size = singletons.size();

            for (auto & column : possibilitiess) {
                if (column.size() == 1) continue;
                
                std::erase_if(column, [&](size_t field_id) { return singletons.contains(field_id); });

                if (column.size() == 1) {
                    singletons.insert(column[0]);
                }
            }

        } while (singletons_size != singletons.size());
        
        m_position_in_ticket_to_field = std::vector<size_t>();
        for (const auto & column : possibilitiess) {
            m_position_in_ticket_to_field->push_back(column[0]);
        }
    }

    test::Value my_departure() const {
        if (!m_position_in_ticket_to_field) {
            return 0;
        }

        test::Value sum = 1;

        for (size_t pos = 0 ; pos != m_position_in_ticket_to_field->size() ; ++pos) {
            const size_t field_id = (*m_position_in_ticket_to_field)[pos];
            const std::string & field_name = m_field_names[field_id];

            if (field_name.substr(0, std::strlen("departure")) == "departure") {
                sum *= my_ticket[pos];
            }
        }

        return sum;
    }

    friend std::ostream & operator<<(std::ostream & stream, const TicketTranslation & self) {
        stream << "\n-- Fields:\n";

        for (size_t i = 0 ; i != self.m_field_names.size() ; ++i) {
            stream << self.m_field_names[i] << ": ";
            const auto & restrictions = self.m_restrictions[i];
            stream << restrictions[0] << ' ' << restrictions[1] << '\n';
        }

        stream << "\n-- Other tickets:\n";
        for (const auto & ticket : self.m_other_tickets) {
            stream << ticket << '\n';
        }

        stream << "\n-- My ticket:\n" << self.my_ticket << '\n';

        if (self.m_position_in_ticket_to_field) {
            stream << "-- Field order\n";
            for (size_t pos = 0 ; pos != self.m_position_in_ticket_to_field->size() ; ++pos) {
                const size_t field_id = (*self.m_position_in_ticket_to_field)[pos];
                const std::string & field_name = self.m_field_names[field_id];

                stream << field_name << ' ';
            }
            stream << '\n';
        }

        return stream;
    }
};


Output day16(const std::vector<std::string> & lines, const DayExtraInfo & extra) {
    TicketTranslation ticket_list = TicketTranslation(lines);

    const test::Value error_rate = ticket_list.remove_invalid_tickets();
    
    if (extra.can_skip_part_B) {
        return Output(error_rate, 0);
    }

    ticket_list.deduce_fields();

    const test::Value my_ticket_departure = ticket_list.my_departure();
    return Output(error_rate, my_ticket_departure);
}
