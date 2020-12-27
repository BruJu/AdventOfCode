#include "../advent_of_code.hpp"
#include "../util/regex_helper.hpp"

#include <vector>

// https://adventofcode.com/2015/day/15

namespace {
    struct Property {
        int64_t capacity = 0;
        int64_t durability = 0;
        int64_t flavor = 0;
        int64_t texture = 0;
        int64_t calories = 0;

        Property() = default;

        explicit Property(const std::vector<std::string> & values) {
            capacity   = std::stoi(values[1]);
            durability = std::stoi(values[2]);
            flavor     = std::stoi(values[3]);
            texture    = std::stoi(values[4]);
            calories   = std::stoi(values[5]);
        }

        Property & operator+=(const Property & rhs) {
            capacity   += rhs.capacity;
            durability += rhs.durability;
            flavor     += rhs.flavor;
            texture    += rhs.texture;
            calories   += rhs.calories;

            return *this;
        }
        
        Property & operator-=(const Property & rhs) {
            capacity   -= rhs.capacity;
            durability -= rhs.durability;
            flavor     -= rhs.flavor;
            texture    -= rhs.texture;
            calories   -= rhs.calories;

            return *this;
        }

        friend Property operator+(const Property & lhs, const Property & rhs) {
            Property copy = lhs;
            copy += rhs;
            return copy;
        }

        Property & operator*=(int64_t value) {
            capacity   *= value;
            durability *= value;
            flavor     *= value;
            texture    *= value;
            calories   *= value;

            return *this;
        }

        friend Property operator*(const Property & lhs, int64_t value) {
            Property copy = lhs;
            copy *= value;
            return copy;
        }

        [[nodiscard]] int64_t evaluate() const noexcept {
            if (capacity <= 0 || durability <= 0 || flavor <= 0 || texture <= 0 )
                return 0;

            return capacity * durability * flavor * texture;
        }

        bool operator>(Property & rhs) const {
            return evaluate() > rhs.evaluate();
        }
    };

    struct Ingredient {
        static constexpr const char * Regex_Pattern = R"(^([a-zA-Z]*): capacity (-?[0-9]+), durability (-?[0-9]+), flavor (-?[0-9]+), texture (-?[0-9]+), calories (-?[0-9]+)$)";

        std::string name;
        Property properties;

        Ingredient(const std::vector<std::string> & values)
        : name(values[0]), properties(values) {}
    };

    template <typename Validation>
    std::optional<Property> naive_picking(const std::vector<Ingredient> & ingredients, unsigned int left, const Validation & validation, size_t i_ing = 0, Property prop = Property()) {
        if (i_ing == ingredients.size() - 1) {
            prop += ingredients[i_ing].properties * left;

            if (validation(prop)) return prop;
            else                  return std::nullopt;
        }

        std::optional<Property> current_best = std::nullopt;

        for (size_t i = 0 ; i != left ; ++i) {
            auto branch_prop = naive_picking(ingredients, left - i, validation, i_ing + 1, prop + ingredients[i_ing].properties * i);
            if (!branch_prop) continue;

            if (!current_best || *branch_prop > *current_best) {
                current_best = branch_prop;
            }
        }

        return current_best;
    }
}

Output day_2015_15(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const std::vector<Ingredient> ingredients = bj::lines_to_class_by_regex<Ingredient>(lines);

    // If we would like to get the exact recipee, we should add another return value which is the (reverse) picked ingredients
    // to the naive_picking function.

    const std::optional<Property> optimal        = naive_picking(ingredients, 100, [](const Property &) { return true; });
    const std::optional<Property> optimal_500cal = naive_picking(ingredients, 100, [](const Property & prop) { return prop.calories == 500; });

    return Output(optimal->evaluate(), optimal_500cal->evaluate());
}
