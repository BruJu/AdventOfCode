#include "libs.hpp"

#include <algorithm>
#include <vector>
#include <variant>
#include <memory>
#include <set>
#include <map>
#include <regex>
#include <ostream>
#include <cmath>

// https://adventofcode.com/2020/day/21

using Ingredient = std::string;
using Alergen = std::string;

template <typename Ts>
void print(const Ts & ts) {
    for (const auto & s : ts) {
        std::cout << s << '\n';
    }
}


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

std::set<Ingredient> find_ingredients(const std::vector<ReaderRetVal> & all) {
    std::set<Ingredient> retval;
    for (const auto & read_values : all) {
        retval.insert(read_values.first.begin(), read_values.first.end());
    }
    return retval;
}

std::set<Alergen> find_alergens(const std::vector<ReaderRetVal> & all) {
    std::set<Alergen> retval;
    for (const auto & read_values : all) {
        retval.insert(read_values.second.begin(), read_values.second.end());
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


void restrain_mapping(std::set<Ingredient> & dest, const std::set<Ingredient> & known) {
    auto it = dest.begin();
    while (it != dest.end()) {
        if (known.contains(*it)) {
            ++it;
        } else {
            it = dest.erase(it);
        }
    }
}

void restrain_mapping_not(std::set<Ingredient> & dest, const std::set<Ingredient> & known) {
    auto it = dest.begin();
    while (it != dest.end()) {
        if (!known.contains(*it)) {
            ++it;
        } else {
            it = dest.erase(it);
        }
    }
}

void restraint_from_known(std::map<Alergen, std::set<Ingredient>> & mapping, const std::vector<ReaderRetVal> & read) {
    for (const auto & read_value : read) {
        for (const auto & alergen : read_value.second) {
            restrain_mapping(mapping.find(alergen)->second, read_value.first);
        }
    }
}


std::set<Ingredient> get_safe_ingredients(
    const std::map<Alergen, std::set<Ingredient>> & mapping,
    const std::set<Ingredient> & all_ingredients) {

    std::set<Ingredient> contaminated;
    for (const auto & [alergen, ingredients] : mapping) {
        //std::cout << alergen << ": ";

        for (const auto & ing : ingredients) {
            //std::cout << ing << " ; ";

            contaminated.insert(ing);
        }
        //std::cout << "\n";
    }

    std::set<Ingredient> safe_ingredients;
    //std::cout << "Safe: \n";
    for (const auto & ingredient : all_ingredients) {
        if (!contaminated.contains(ingredient)) {
            safe_ingredients.insert(ingredient);
            //std::cout << ingredient << "\n";
        }
    }
    return safe_ingredients;
}


std::set<Ingredient> find_alone_ingredients(std::map<Alergen, std::set<Ingredient>> & mapping) {
    std::set<Ingredient> retval;
    for (auto & [alergen, ingredient_list] : mapping) {
        if (ingredient_list.size() == 1) {
            retval.insert(ingredient_list.begin(), ingredient_list.end());
        }
    }
    return retval;
}

void constrain_1_to_1(std::map<Alergen, std::set<Ingredient>> & mapping) {
    bool stable;
    do {
        stable = true;

        std::set<Ingredient> alone = find_alone_ingredients(mapping);

        for (auto & [alergen, ingredient_list] : mapping) {
            if (ingredient_list.size() == 1) continue;

            size_t before = ingredient_list.size();
            restrain_mapping_not(ingredient_list, alone);

            if (before != ingredient_list.size()) stable = false;
        }

    } while (!stable);
}


Output day21(const std::vector<std::string> & lines, const DayExtraInfo &) {
    std::vector<ReaderRetVal> read = lines_transform::map<ReaderRetVal>(lines, reader);

    std::set<Ingredient> all_ingredients = find_ingredients(read);
    std::set<Alergen> all_alergens = find_alergens(read);
    std::map<Alergen, std::set<Ingredient>> mapping = make_initial_mapping(all_ingredients, all_alergens);
    restraint_from_known(mapping, read);
    constrain_1_to_1(mapping);

    std::set<Ingredient> safe_ingredients = get_safe_ingredients(mapping, all_ingredients);

    test::Value r1 = 0;
    for (const auto & [inggg, rrrrr] : read) {
        for (const auto & ing : inggg) {
            if (safe_ingredients.contains(ing)) {
                ++r1;
            }
        }
    }

    for (const auto & [x, y] : mapping) {
        if (!safe_ingredients.contains(x)) {
            std::cout << *y.begin();
        }
        std::cout << ",";
    }
    std::cout << "\n";

    return Output(r1, 0);
}
