#include "libs.hpp"
#include "libs_ensemblist.hpp"

#include <array>
#include <vector>
#include <map>
#include <set>
#include <deque>

// https://adventofcode.com/2020/day/22

class Game {
private:
    using Card = size_t;
    using Deck = std::deque<Card>;
    Deck m_player_1;
    Deck m_player_2;

    std::set<std::pair<Deck, Deck>> m_previous_rounds;

    [[nodiscard]] bool already_saw_this() const {
        const auto situation = std::pair<Deck, Deck>(m_player_1, m_player_2);
        return m_previous_rounds.contains(situation);
    }

public:
    explicit Game(const std::vector<std::string> & lines) {
        bool on_player_1 = true;
        for (const auto & line : lines) {
            if (line == "") continue;
            if (line.substr(0, 6) == "Player") {
                on_player_1 = (line[7] - '0') == 1;
            } else {
                const int card = std::stoul(line);

                if (on_player_1) {
                    m_player_1.push_back(card);
                } else {
                    m_player_2.push_back(card);
                }
            }
        }
    }

    Game(const Game & original, size_t cards_p1, size_t cards_p2) {
        for (size_t i = 0 ; i != cards_p1 ; ++i) {
            m_player_1.push_back(original.m_player_1[i]);
        }

        for (size_t i = 0 ; i != cards_p2 ; ++i) {
            m_player_2.push_back(original.m_player_2[i]);
        }
    }

    [[nodiscard]] bool ended() const noexcept {
        return m_player_1.empty() || m_player_2.empty();
    }

    [[nodiscard]] test::Value get_winner_score() const noexcept {
        const auto & winner_deck = m_player_1.empty() ? m_player_2 : m_player_1;

        test::Value s = 0;
        for (size_t i = 0 ; i != winner_deck.size() ; ++i) {
            s += (winner_deck.size() - i) * winner_deck[i];
        }
        return s;
    }

    template <typename WinnerRule>
    void play(WinnerRule winner_rule) {
        if (already_saw_this()) {
            m_player_2.clear();
            return;
        }

        m_previous_rounds.insert(std::pair(m_player_1, m_player_2));

        const Card card_1 = m_player_1.front();    m_player_1.pop_front();
        const Card card_2 = m_player_2.front();    m_player_2.pop_front();

        const bool player_1_win = winner_rule(*this, card_1, card_2);

        if (player_1_win) {
            m_player_1.push_back(card_1);
            m_player_1.push_back(card_2);
        } else {
            m_player_2.push_back(card_2);
            m_player_2.push_back(card_1);
        }
    }

    static bool regular(const Game &, Card card_1, Card card_2) {
        return card_1 > card_2;
    }

    static bool recursive_combat(const Game & game, Card card_1, Card card_2) {
        if (game.m_player_1.size() >= card_1 && game.m_player_2.size() >= card_2) {
            Game new_game { game, card_1, card_2 } ;
            while (!new_game.ended()) {
                new_game.play(Game::recursive_combat);
            }

            return !new_game.m_player_1.empty();
        } else {
            return card_1 > card_2;
        }
    }
};

Output day22(const std::vector<std::string> & lines, const DayExtraInfo &) {
    Game game { lines };
    Game recursive_game = game;

    while (!game.ended()) {
        game.play(Game::regular);
    }

    while (!recursive_game.ended()) {
        recursive_game.play(Game::recursive_combat);
    }

    return Output(
        game.get_winner_score(),
        recursive_game.get_winner_score()
    );
}
