#include "libs.hpp"
#include "libs_ensemblist.hpp"

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

std::pair<std::set<Ingredient>, std::set<Alergen>> find_components(const std::vector<ReaderRetVal> & all) {
    std::pair<std::set<Ingredient>, std::set<Alergen>> retval;

    for (const auto & read_values : all) {
        retval.first .insert(read_values.first .begin(), read_values.first .end());
        retval.second.insert(read_values.second.begin(), read_values.second.end());
    }

    return retval;
}


std::map<Alergen, std::set<Ingredient>> make_initial_mapping(const std::set<Ingredient> & ingredients, const std::set<Alergen> & alergens) {
    std::map<Alergen, std::set<Ingredient>> retval;
    for (const auto & alergen : alergens) {
        retval[alergen] = ingredients;
    }
    return retval;
}

void restraint_from_known(std::map<Alergen, std::set<Ingredient>> & mapping, const std::vector<ReaderRetVal> & read) {
    for (const auto & [ingredients, alergens] : read) {
        for (const auto & alergen : alergens) {
            mapping.find(alergen)->second &= ingredients;
        }
    }
}

std::set<Ingredient> get_safe_ingredients(
    const std::map<Alergen, std::set<Ingredient>> & mapping,
    const std::set<Ingredient> & all_ingredients) {

    std::set<Ingredient> contaminated;
    for (const auto & [alergen, ingredients] : mapping) {
        contaminated.insert(ingredients.begin(), ingredients.end());
    }

    return set::difference(all_ingredients, contaminated);
}

}

Output day21(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const std::vector<ReaderRetVal> read = lines_transform::map<ReaderRetVal>(lines, reader);

    const auto [all_ingredients, all_alergens] = find_components(read);
    
    std::map<Alergen, std::set<Ingredient>> mapping = make_initial_mapping(all_ingredients, all_alergens);
    restraint_from_known(mapping, read);
    set::resolve_key_to_value(mapping);

    std::set<Ingredient> safe_ingredients = get_safe_ingredients(mapping, all_ingredients);

    test::Value r1 = 0;
    for (const auto & [recipy_ingredients, _] : read) {
        r1 += set::intersection(recipy_ingredients, safe_ingredients).size();
    }

    std::string r2 = "";
    for (const auto & [alergen, ingredient] : mapping) {
        if (!r2.empty()) r2 += ',';
        r2 += *ingredient.begin();
    }
    
    //std::cout << r2 << "\n";

    return Output(r1, r2.size());
}
