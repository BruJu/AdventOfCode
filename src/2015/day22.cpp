#include "../advent_of_code.hpp"
#include <algorithm>
#include <optional>
#include <cstring>

// https://adventofcode.com/2015/day/21

namespace {
    struct GameSituation {
        explicit GameSituation(int32_t p_perma_poison) : perma_poison(p_perma_poison) {}

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

    std::optional<int32_t> GameSituation::try_recharge(std::optional<int32_t> mana_cap) const {
        if (turns_of_recharge > 0) return mana_cap;
        if (my_mana < 229) return mana_cap;

        auto copy = *this;
        copy.turns_of_recharge = 5;
        copy.my_mana -= 229;
        copy.used_mana += 229;
        return copy.defeat_boss(mana_cap);
    }

    std::optional<int32_t> GameSituation::try_poison(std::optional<int32_t> mana_cap) const {
        if (turns_of_poison > 0) return mana_cap;
        if (my_mana < 173) return mana_cap;

        auto copy = *this;
        copy.turns_of_poison = 6;
        copy.my_mana -= 173;
        copy.used_mana += 173;
        return copy.defeat_boss(mana_cap);
    }

    std::optional<int32_t> GameSituation::try_shield(std::optional<int32_t> mana_cap) const {
        if (turns_of_shield > 0) return mana_cap;
        if (my_mana < 113) return mana_cap;

        auto copy = *this;
        copy.turns_of_shield = 6;
        copy.my_mana -= 113;
        copy.used_mana += 113;
        return copy.defeat_boss(mana_cap);
    }

    std::optional<int32_t> GameSituation::try_drain(std::optional<int32_t> mana_cap) const {
        if (my_mana < 73) return mana_cap;

        auto copy = *this;
        copy.my_mana -= 73;
        copy.used_mana += 73;
        copy.my_hp += 2;
        copy.boss_hp -= 2; // manage later if ennemy is dead
        return copy.defeat_boss(mana_cap);
    }

    std::optional<int32_t> GameSituation::try_magic_missile(std::optional<int32_t> mana_cap) const {
        if (my_mana < 53) return mana_cap;

        auto copy = *this;
        copy.my_mana -= 53;
        copy.used_mana += 53;
        copy.boss_hp -= 4; // manage later if ennemy is dead
        return copy.defeat_boss(mana_cap);
    }
}


Output day_2015_22(const std::vector<std::string> &, const DayExtraInfo &) {
    const auto a = GameSituation(0).defeat_boss().value();
    const auto b = GameSituation(1).defeat_boss().value();

    return Output(a, b);
}
