#include "libs.hpp"
#include <regex>

// https://adventofcode.com/2020/day/4

class ValidationRules {
    using Validator = std::function<bool (std::string)>;

    std::map<std::string, Validator> m_validators;

public:
    static std::function<bool (std::string)> four_digits(int min, int max) {
        return [min, max](std::string s) {
            static constexpr const char * PATTERN = "^([0-9]{4})$";
            std::regex pattern { PATTERN };
            std::smatch matches;

            const auto rs = std::regex_search(s, matches, pattern);
            if (!rs) return false;
            const int value = std::stoi(matches[1].str());
            return min <= value && max >= value;
        };
    }

    static std::function<bool (std::string)> match_pattern(std::string pattern) {
        return [pattern](std::string s) {
            std::regex r { pattern };
            std::smatch matches;
            return std::regex_search(s, matches, r);
        };
    }

    ValidationRules() {
        m_validators["byr"] = four_digits(1920, 2002);
        m_validators["iyr"] = four_digits(2010, 2020);
        m_validators["eyr"] = four_digits(2020, 2030);
        m_validators["hgt"] = [](const std::string & s) {
            std::regex r { "^([0-9]*)(cm|in)$"};
            std::smatch matches;
            const auto rs = std::regex_search(s, matches, r);
            if (!rs) return false;
            // TODO : I need a function "direct_regex" that does the three operations

            const int raw_value = std::stoi(matches[1].str());

            if (matches[2].str() == "cm") {
                return 150 <= raw_value && raw_value <= 193;
            } else {
                return 59 <= raw_value && raw_value <= 76;
            }
        };
        m_validators["hcl"] = match_pattern("^#[0-9a-f]{6}$");
        m_validators["ecl"] = match_pattern("^(amb|blu|brn|gry|grn|hzl|oth)$");
        m_validators["pid"] = match_pattern("^[0-9]{9}$");
    }

    [[nodiscard]] bool operator()(std::string key, std::string value) const {
        const auto itValidator = m_validators.find(key);
        if (itValidator == m_validators.end()) return true;

        return itValidator->second(value);
    }
};

class Passport {
    static constexpr const char * PATTERN = "^ ?([a-z]*):([#|0-9a-zA-Z]*)";

    std::map<std::string, std::string> m_content;
public:
    explicit Passport(std::string passport) {
        std::regex pattern { PATTERN };

        std::smatch matches;

        while (std::regex_search(passport, matches, pattern)) {
            m_content.insert_or_assign(matches[1].str(), matches[2].str());
            
            passport = matches.suffix();
        }
    }

    [[nodiscard]] bool has_all_fields() const {
        static constexpr std::array<const char *, 7> required = {
            "byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid" /*, "cid" */
        };

        for (const auto req : required) {
            if (!m_content.contains(req)) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] bool has_valid_fields(const ValidationRules & rules) const {
        return std::all_of(m_content.begin(), m_content.end(),
            [&](const auto & pair) { return rules(pair.first, pair.second); }
        );
    }

    [[nodiscard]] std::string to_string() const {
        std::stringstream builder;

        for (const auto & [key, value] : m_content) {
            builder << key << ":" << value << " ";
        }

        return builder.str();
    }
};

Output day04(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const std::vector<Passport> passports = lines_transform::group<Passport, std::string>(
        lines,
        lines_transform::map_identity<std::string>,
        [](const std::string & lhs, const std::string & rhs) { return lhs + " " + rhs; },
        [](const std::string & s) { return Passport(s); }
    );

    const size_t passports_with_all_fields = std::count_if(
        passports.begin(), passports.end(),
        [](const Passport & p) { return p.has_all_fields(); }
    );

    ValidationRules rules = ValidationRules();
    const size_t passports_with_valid_fields = std::count_if(
        passports.begin(), passports.end(),
        [&](const Passport & p) { return p.has_all_fields() && p.has_valid_fields(rules); }
    );

    return Output(passports_with_all_fields, passports_with_valid_fields);
}
