#include "../advent_of_code.hpp"
#include "../util/regex_helper.hpp"

// https://adventofcode.com/2016/day/4

// Rooms are identified with a name, a sector id and a check sum
// Valid room have a check sum = the letters that occcur the most, in order
// a) Which rooms are valid?
// b) Find in which sector North Pole items are stored. To decypher names, 
// we have to use the letter shift algorithm with a shift equal to the sector
// id on the (encoded) name.

namespace {
    struct Room {
        static constexpr const char * Regex_Pattern = R"(^([a-z\-]*)-([0-9]*)\[([a-z]*)\]$)";
        
        explicit Room(const std::vector<std::string> & values) {
            name = values[0];
            sector_id = std::stoi(values[1]);
            checksum = values[2];
        }

        std::string name;
        int32_t sector_id;
        std::string checksum;

        [[nodiscard]] bool exists() const {
            std::map<char, size_t> cnt;

            for (char c : name) {
                cnt[c] += 1;
            }

            cnt['-'] = 0;

            std::vector<std::pair<char, size_t>> found;
            for (const auto [letter, occurrences] : cnt) {
                found.push_back(std::pair<size_t, char>(-occurrences, letter));
            }

            std::sort(found.begin(), found.end());

            std::string expected_checksum = "";
            for (size_t i = 0 ; i < 5 && i < found.size() ; ++i) {
                expected_checksum += found[i].second;
            }

            return expected_checksum == checksum;
        }

        [[nodiscard]] std::string get_real_name() const noexcept {
            std::string real_name;

            for (char c : name) {
                if (c != '-') {
                    c = (((c - 'a') + sector_id) % 26) + 'a';
                }

                real_name += c;
            }

            return real_name;
        }
    };
}

Output day_2016_04(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const std::vector<Room> rooms = bj::lines_to_class_by_regex<Room>(lines);

    int32_t sum_of_sectors = 0;
    int32_t sector_of_polaris = 0;

    for (const auto & room : rooms) {
        if (room.exists()) {
            sum_of_sectors += room.sector_id;

            if (room.get_real_name().find("north") != std::string::npos) {
                sector_of_polaris = room.sector_id;
            }
        }
    }

    return Output(sum_of_sectors, sector_of_polaris);
}
