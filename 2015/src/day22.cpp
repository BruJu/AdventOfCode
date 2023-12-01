#include "../../common/advent_of_code.hpp"
#include <algorithm>
#include <optional>
#include <cstring>

// https://adventofcode.com/2015/day/22

// A magician must fight a boss. The magician have 5 different spells.

// We use a classic graph exploration, with some branch cutting as we are
// looking for the way to beat the boss that uses the less possible mana
// (mana_cap)

namespace {
    struct GameSituation {
        struct AlwaysPossible {
            bool operator()(const GameSituation &) const { return true; }
        };

        explicit GameSituation(int32_t p_perma_poison) : perma_poison(p_perma_poison) {}

        /**
         * A function to explore a new move (and the move it can be used after it)
         * 
         * Only the branches that uses less than mana_cap total mana are explored.
         * `mana_cost`: Cost for this move.
         * `do_action`: A function that changes the state of the given
         * GameSituation, according to the used move
         * `legality_check`: A function that returns false if this move can't
         * be used in the given GameSituation.
         * 
         * This function returns the lowest known total mana to use to defeat
         * the boss, using the given current given known lowest mana (mana_cap),
         * and the mana that have been used to reach this state (+ the mana to
         * use to beat the boss).
         *
         * In a graph context, what this function do is essentialy explore the
         * current node and its children, and return the lowest value between
         * the leaves of this node and all the previously explored leaves.
         */
        template<typename F, typename LegalityCheck = AlwaysPossible>
        std::optional<int32_t> explore_new_scenario(
            std::optional<int32_t> mana_cap,
            int32_t mana_cost,
            F do_action,
            LegalityCheck legality_check = AlwaysPossible()
        ) const;

        int32_t my_hp = 50;
        int32_t my_mana = 500;
        int32_t used_mana = 0;

        int32_t boss_hp = 55;
        int32_t boss_damage = 8;

        bool playerTurn = true;

        int32_t turns_of_shield = 0;
        int32_t turns_of_poison = 0;
        int32_t turns_of_recharge = 0;

        int32_t perma_poison = 0;

        std::optional<int32_t> defeat_boss(std::optional<int32_t> mana_cap = std::nullopt);
        
        std::optional<int32_t> try_recharge     (std::optional<int32_t> mana_cap) const;
        std::optional<int32_t> try_poison       (std::optional<int32_t> mana_cap) const;
        std::optional<int32_t> try_shield       (std::optional<int32_t> mana_cap) const;
        std::optional<int32_t> try_drain        (std::optional<int32_t> mana_cap) const;
        std::optional<int32_t> try_magic_missile(std::optional<int32_t> mana_cap) const;

        void apply_effects();
    };

    void GameSituation::apply_effects() {
        if (turns_of_poison > 0) {
            boss_hp -= 3;
            turns_of_poison -= 1;
        }

        if (turns_of_recharge > 0) {
            my_mana += 101;
            turns_of_recharge -= 1;
        }

        if (turns_of_shield > 0) {
            turns_of_shield -= 1;
        }
    }

    std::optional<int32_t> GameSituation::defeat_boss(std::optional<int32_t> mana_cap) {
        if (mana_cap && mana_cap.value() <= used_mana) return mana_cap;

        if (playerTurn) {
            my_hp -= perma_poison;
            if (my_hp <= 0) return mana_cap;
        }

        apply_effects();

        if (boss_hp <= 0) return used_mana;

        if (playerTurn) {
            playerTurn = false;

            mana_cap = try_recharge(mana_cap);
            mana_cap = try_poison(mana_cap);
            mana_cap = try_shield(mana_cap);
            mana_cap = try_drain(mana_cap);
            mana_cap = try_magic_missile(mana_cap);

            return mana_cap;
        } else {
            int32_t damage = boss_damage;
            if (turns_of_shield > 0) damage -= 7;
            if (damage <= 0) damage = 1;

            my_hp -= damage;
            if (my_hp <= 0) return mana_cap;

            playerTurn = true;
            return defeat_boss(mana_cap);
        }
    }

    /**
     * Check if this move is legal, check if enough mana, consume the mana, do
     * the effect of the action, try to beat the boss from there.
     */
    template<typename F, typename LegalityCheck>
    std::optional<int32_t> GameSituation::explore_new_scenario(
        std::optional<int32_t> mana_cap,
        int32_t mana_cost,
        F do_action,
        LegalityCheck legality_check
    ) const {
        if (!legality_check(*this)) return mana_cap;
        if (my_mana < mana_cost) return mana_cap;
        auto copy = *this;
        copy.my_mana   -= mana_cost;
        copy.used_mana += mana_cost;
        
        if (mana_cap && mana_cap.value() <= copy.used_mana) return mana_cap;
        do_action(copy);
        if (copy.boss_hp <= 0) return copy.used_mana;
        return copy.defeat_boss(mana_cap);
    }

    std::optional<int32_t> GameSituation::try_recharge(std::optional<int32_t> mana_cap) const {
        return explore_new_scenario(mana_cap, 229,
            [](      GameSituation & situation) { situation.turns_of_recharge = 5;         },
            [](const GameSituation & situation) { return situation.turns_of_recharge == 0; }
        );
    }

    std::optional<int32_t> GameSituation::try_poison(std::optional<int32_t> mana_cap) const {
        return explore_new_scenario(mana_cap, 173,
            [](      GameSituation & situation) { situation.turns_of_poison = 6;         },
            [](const GameSituation & situation) { return situation.turns_of_poison == 0; }
        );
    }

    std::optional<int32_t> GameSituation::try_shield(std::optional<int32_t> mana_cap) const {
        return explore_new_scenario(mana_cap, 113,
            [](      GameSituation & situation) { situation.turns_of_shield = 6;         },
            [](const GameSituation & situation) { return situation.turns_of_shield == 0; }
        );
    }

    std::optional<int32_t> GameSituation::try_drain(std::optional<int32_t> mana_cap) const {
        return explore_new_scenario(mana_cap, 73,
            [](GameSituation & situation) {
                situation.my_hp += 2;
                situation.boss_hp -= 2;
            }
        );
    }

    std::optional<int32_t> GameSituation::try_magic_missile(std::optional<int32_t> mana_cap) const {
        return explore_new_scenario(mana_cap, 53,
            [](GameSituation & situation) {
                situation.boss_hp -= 4;
            }
        );
    }
}


Output day_2015_22(const std::vector<std::string> &, const DayExtraInfo &) {
    const auto a = GameSituation(0).defeat_boss().value();
    const auto b = GameSituation(1).defeat_boss().value();

    return Output(a, b);
}
