#include "../advent_of_code.hpp"
#include "../libs_ensemblist.hpp"

#include <map>
#include <set>

// https://adventofcode.com/2020/day/21

namespace {

using Ingredient = std::string;
using Alergen = std::string;

using ReaderRetVal = std::pair<std::set<Ingredient>, std::set<Alergen>>;

ReaderRetVal reader(const std::string & line) {
    std::string ingredients_str = line.substr(0, line.find("(contains") - 1);
    std::set<Ingredient> ingredients;
    StringSplitter ingredients_splitter { ingredients_str, ' ' };
    while (ingredients_splitter) {
        std::string s = ingredients_splitter();
        ingredients.insert(std::move(s));
    }

    std::string contains = line.substr(line.find("(contains"));
    contains = contains.substr(10);
    contains = contains.substr(0, contains.length() - 1);
    std::set<Alergen> alergens;
    StringSplitter alergen_splitter { contains, ',' };
    while (alergen_splitter) {
        std::string s = alergen_splitter();
        if (s[0] == ' ') s = s.substr(1);
        alergens.insert(std::move(s));
    }

    return ReaderRetVal(std::move(ingredients), std::move(alergens));
}

struct InitialValues {
    std::map<Ingredient, size_t> ingredients_occurrences;
    std::map<Alergen, std::set<Ingredient>> alergens;
};

InitialValues read(const std::vector<std::string> & lines) {
    InitialValues retval;

    for (const auto & line : lines) {
        const auto [ingredients, alergens] = reader(line);

        for (const auto & alergen : alergens) {
            const auto it = retval.alergens.find(alergen);

            if (it == retval.alergens.end()) {
                retval.alergens[alergen] = ingredients;
            } else {
                it->second &= ingredients;
            }
        }
        
        for (const auto & ingredient : ingredients) {
            ++retval.ingredients_occurrences[ingredient];
        }
    }

    return retval;
}

std::set<Ingredient> get_safe_ingredients(
    const std::map<Alergen, std::set<Ingredient>> & mapping,
    const std::set<Ingredient> & all_ingredients) {

    std::set<Ingredient> contaminated;
    for (const auto & [alergen, ingredients] : mapping) {
        contaminated |= ingredients;
    }

    return set::difference(all_ingredients, contaminated);
}

}

Output day_2020_21(const std::vector<std::string> & lines, const DayExtraInfo &) {
    auto [ingredients_occurrences, mapping] = read(lines);
    set::resolve_key_to_value(mapping);

    std::set<Ingredient> safe_ingredients = get_safe_ingredients(mapping, set::to_set(ingredients_occurrences));

    long long int r1 = 0;
    for (const auto & safe : safe_ingredients) {
        r1 += ingredients_occurrences.find(safe)->second;
    }

    std::string r2 = "";
    for (const auto & [alergen, ingredient] : mapping) {
        if (!r2.empty()) r2 += ',';
        r2 += *ingredient.begin();
    }
    
    return Output(r1, r2);
}
