#include "../../common/advent_of_code.hpp"
#include <array>
#include <compare>
#include <exception>

// https://adventofcode.com/2022/day/2
// Rock-Paper-Scissors
//
// Lines the format:
// A|B|C X|Y|Z
//
// Opponent plays -> A = Rock, B = Paper, C = Scissors
// (A) We pick a symbol -> X = Rock, Y = Paper, C = Scissors
// (B) We want an outcome -> X = We want to lose, Y = we want to draw, Z = we want to win
//
// Compute for each game:
// The symbol score (Rock/Paper/Scissors = 1/2/3) + The game points (W/D/L = 6/3/0)
// Return the sum for all games in the file


namespace {

/* Janken symbols */

enum class Jajanken { Rock, Paper , Scissors };

// Converts A / B / C / X / Y / Z to Rock / Paper / Scissors
Jajanken char_to_jajanken(const char c) {
  switch (c) {
    case 'A': return Jajanken::Rock;
    case 'B': return Jajanken::Paper;
    case 'C': return Jajanken::Scissors;
    case 'X': return Jajanken::Rock;
    case 'Y': return Jajanken::Paper;
    case 'Z': return Jajanken::Scissors;
    default: throw std::runtime_error("Bad input");
  }
}

// Gives the point related to picking a symbol
[[nodiscard]] int symbol_score(const Jajanken symbol) {
  switch (symbol) {
    case Jajanken::Rock:     return 1;
    case Jajanken::Paper:    return 2;
    case Jajanken::Scissors: return 3;
  }
  throw std::runtime_error("Bad input");
}

// me vs them ; 6 = win, 3 = draw, 0 = lose
int match_outcome(const Jajanken me, const Jajanken them) {
  if (me == them) return 3;
  if (static_cast<int>(me) > static_cast<int>(them)) return 6 - match_outcome(them, me);

  if (me == Jajanken::Rock)     return them == Jajanken::Scissors ? 6 : 0;
  if (me == Jajanken::Scissors) return them == Jajanken::Paper    ? 6 : 0;
  if (me == Jajanken::Paper)    return them == Jajanken::Rock     ? 6 : 0;
  return 3;
}

// X = 0, Y = 3, Z = 6
int get_outcome_from_input(char outcome) {
  switch (outcome) {
    case 'X': return 0;
    case 'Y': return 3;
    case 'Z': return 6;
    default: throw std::runtime_error("Bad input");
  }
}

// Find the symbol to produce the wanted game outcome
Jajanken how_to(const Jajanken them, const int expected) {
  for (Jajanken symbol : { Jajanken::Paper, Jajanken::Rock, Jajanken::Scissors }) {
    if (match_outcome(symbol, them) == expected) {
      return symbol;
    }
  }

  throw std::runtime_error("Bad input");
}

}

Output day_2022_02(const std::vector<std::string> & lines, const DayExtraInfo &) {
  long long int part_a = 0;
  long long int part_b = 0;

  for (const std::string & line : lines) {
    const Jajanken opponent_symbol = char_to_jajanken(line[0]);

    // Part A
    {
      const Jajanken my_symbol = char_to_jajanken(line[2]);
      part_a += symbol_score(my_symbol) + match_outcome(opponent_symbol, my_symbol);
    }

    // Part B
    {
      const int outcome = get_outcome_from_input(line[2]);
      const Jajanken my_symbol = how_to(opponent_symbol, outcome);
      part_b += symbol_score(my_symbol) + outcome;
    }
  }

  return Output(part_a, part_b);
}
