#include "../advent_of_code.hpp"
#include <algorithm>
#include <cstring>

// https://adventofcode.com/2015/day/21

//! You have 100 hp and you want to defeat the monster described in the input.
//!
//! You can buy items from a shop. You have to buy 1 weapon, at most 1 armor,
//! and at most 2 rings.
//!
//! What is the cheapest build to win? And the most expensive build to win?

namespace {

// =============================================================================
// ==== AVAILABLE ITEMS

/** The different types of items */
enum class Kind { Weapon, Armor, Ring };

/** An item */
struct Item {
    Kind kind;
    const char * name;
    int32_t cost;
    int32_t attack;
    int32_t defense;

    friend std::ostream & operator<<(std::ostream & stream, const Item & self) {
        return stream
            << "Item{ " << self.name
            << ", cost=" << self.cost
            << ", attack=" << self.attack
            << ", defense=" << self.defense
            << "}";
    }
};

/** Return the list of possible items */
static std::vector<Item> get_list_of_available_items() {
    std::vector<Item> items;
    
    items.push_back(Item{ Kind::Weapon, "Dagger"    ,   8, 4, 0 });
    items.push_back(Item{ Kind::Weapon, "Shortsword",  10, 5, 0 });
    items.push_back(Item{ Kind::Weapon, "Warhammer" ,  25, 6, 0 });
    items.push_back(Item{ Kind::Weapon, "Longsword" ,  40, 7, 0 });
    items.push_back(Item{ Kind::Weapon, "Greataxe"  ,  74, 8, 0 });
    items.push_back(Item{ Kind::Armor , "Leather"   ,  13, 0, 1 });
    items.push_back(Item{ Kind::Armor , "Chainmail" ,  31, 0, 2 });
    items.push_back(Item{ Kind::Armor , "Splintmail",  53, 0, 3 });
    items.push_back(Item{ Kind::Armor , "Bandedmail",  75, 0, 4 });
    items.push_back(Item{ Kind::Armor , "Platemail" , 102, 0, 5 });
    items.push_back(Item{ Kind::Ring  , "Damage +1" ,  25, 1, 0 });
    items.push_back(Item{ Kind::Ring  , "Damage +2" ,  50, 2, 0 });
    items.push_back(Item{ Kind::Ring  , "Damage +3" , 100, 3, 0 });
    items.push_back(Item{ Kind::Ring  , "Defense +1",  20, 0, 1 });
    items.push_back(Item{ Kind::Ring  , "Defense +2",  40, 0, 2 });
    items.push_back(Item{ Kind::Ring  , "Defense +3",  80, 0, 3 });

    std::sort(items.begin(), items.end(),
        [](const Item & lhs, const Item & rhs) {
            if (lhs.cost < rhs.cost) return true;
            if (lhs.cost > rhs.cost) return false;
            return std::strcmp(lhs.name, rhs.name) < 0;
        }
    );

    return items;
}


// =============================================================================
// ==== CHARACTERS

/** Combat statistics of a character */
struct CombatStats {
    int32_t attack;
    int32_t defense;
    
    /** Adds to the stats the stats of the item */
    CombatStats & operator+=(const Item & item) {
        attack  += item.attack;
        defense += item.defense;
        return *this;
    }

    /**
    * Return the amount of damage this instance can deal with a defender with 
    * the given stats (using `this.attack` and `defender.defense`).
    */
    [[nodiscard]] int32_t damage_on(const CombatStats & defender) const {
        if (attack < defender.defense) return 1;
        return attack - defender.defense;
    }
};

/** A character */
struct Character {
    int32_t hp;
    CombatStats combat_stats;

    /**
     * Return true if when this character gets the first move, it can defeat
     * the other.
     *
     * Note that this function isn't antisymmetric.
     */
    [[nodiscard]] bool can_defeat(const Character & character) const;
};

bool Character::can_defeat(const Character & ennemy) const {
    // Compute damage dealt on each turn
    const int32_t my_damage = combat_stats.damage_on(ennemy.combat_stats);
    const int32_t his_damage = ennemy.combat_stats.damage_on(combat_stats);

    // Current healths
    int32_t my_hp = hp;
    int32_t his_hp = ennemy.hp;

    // Check if someone isn't living
    if (my_hp <= 0) return false;
    if (his_hp <= 0) return true;

    // Actual fight
    while (true) {
        his_hp -= my_damage;
        if (his_hp <= 0) return true;

        my_hp -= his_damage;
        if (my_hp <= 0) return false;
    }
}

// =============
// == Equipement

/** An equipement is a set of items equiped by the used */
class Equipement {
private:
    // Because we don't need to know what are the actual items of a player, we
    // just store the cost and the total stats provided.
    int32_t m_cost;
    CombatStats m_stats;

    Equipement(int32_t cost, CombatStats stats) : m_cost(cost), m_stats(stats) {}
public:
    /**
     * Constitutite an equipement from a list of buyable items and a number that
     * tells which equipement to buy. The number is used as an array of bool.

     * std::nullopt is returned if the equipement is illegal. An equipement must
     * contain 1 weapon, at most 1 armor and at most 2 rings.
     */
    [[nodiscard]] static std::optional<Equipement> build(const std::vector<Item> & shop, uint32_t chosen);

    /** Total cost to buy this equipement */
    [[nodiscard]] int32_t get_cost() const { return m_cost; }
    /** Total statistics provided by this equipement */
    [[nodiscard]] CombatStats get_statistics() const { return m_stats; }
};

std::optional<Equipement> Equipement::build(const std::vector<Item> & shop, uint32_t chosen) {    
    // Stats and cost
    CombatStats stats { 0, 0 };
    int32_t cost = 0;

    // Legality check
    bool has_weapon = false;
    bool has_armor = false;
    uint32_t rings = 0;

    // Pick items
    for (size_t i = 0 ; i != shop.size() ; ++i) {
        if (!(chosen & (1 << i))) continue;

        const auto & item = shop[i];

        // Partial check of legality
        switch (item.kind) {
            case Kind::Weapon:
                if (has_weapon) return std::nullopt;
                has_weapon = true;
                break;
            case Kind::Armor:
                if (has_armor) return std::nullopt;
                has_armor = true;
                break;
            case Kind::Ring:
                rings += 1;
                break;
        }

        stats += item;
        cost += item.cost;
    }

    // Finish to check legality
    if (!has_weapon || rings > 2) return std::nullopt;

    // Ok
    return Equipement { cost, stats };
}

// =============

/** Convert the inputs into a character */
static Character extract_character(const std::vector<std::string> & lines) {
    // "blabla: 847" -> 847
    static constexpr auto extract_value_from_line = [](const std::string & line) {
        const auto pos = line.find(":");
        return std::stoi(line.data() + pos + 1);
    };

    return Character {
        extract_value_from_line(lines[0]),
        extract_value_from_line(lines[1]),
        extract_value_from_line(lines[2]),
    };
}

/**
 * Find the "best" legal equipement that produces the `expected_outcome` against
 * the monster, consdering the player has `my_health` hp and can buy items from
 * `sold_items`.
 *
 * "Best" legal equipement is defined as the equipement that have the cost that
 * returns true when called with every other equipement cost.
 */
template<typename CostCompare>
std::optional<Equipement> find_cheapest_equipement_to_beat(
    const Character & monster,
    int32_t my_health,
    const std::vector<Item> & sold_items,
    bool expected_outcome,
    CostCompare cost_compare
) {
    std::optional<Equipement> solution = std::nullopt;

    const uint32_t max_chosen = static_cast<uint32_t>(1 << (sold_items.size() + 1));

    for (uint32_t chosen = 1 ; chosen < max_chosen ; ++chosen) {
        const auto candidate = Equipement::build(sold_items, chosen);
        if (!candidate) continue;
        if (solution && cost_compare(solution->get_cost(), candidate->get_cost())) continue;

        const Character me { my_health, candidate->get_statistics() };
        
        if (me.can_defeat(monster) == expected_outcome) {
            solution = candidate;
        }
    }

    return solution;
}

}

Output day_2015_21(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const auto shop = get_list_of_available_items();
    const Character monster = extract_character(lines);

    // Beat the monster with a cheap equipement
    const auto cheapest_to_win = find_cheapest_equipement_to_beat(
        monster, 100, shop, true,
        [](int32_t best, int32_t candidate) { return best <= candidate; }
    );

    // Waste the most possible money to die
    const auto most_expensive_to_lose = find_cheapest_equipement_to_beat(
        monster, 100, shop, false,
        [](int32_t best, int32_t candidate) { return best >= candidate; }
    );

    return Output(
        cheapest_to_win->get_cost(),
        most_expensive_to_lose->get_cost()
    );
}
