#include "../../common/advent_of_code.hpp"
#include <vector>
#include <utility>
#include <map>
#include <algorithm>

// https://adventofcode.com/2023/day/7

using i64 = long long int;

enum class Type {
  FiveOfAKind,
  FourOfAKind,
  FullHouse,
  ThreeOfAKind,
  TwoPair,
  OnePair,
  HighCard
};

static const char * card_order_a = "AKQJT98765432";
static const char * card_order_b = "AKQT98765432J";

static Type compute_type(const std::string & str, bool joker) {
  std::map<char, int> m;
  for (const char c : str) {
    ++m[c];
  }

  if (joker) {
    // Part B: it's always better to just improve your best card
    char best_letter = str[0];
    for (const auto & [symbol, qtt] : m) {
      if (m[best_letter] < qtt || best_letter == 'J') {
        best_letter = symbol;
      }
    }

    if (best_letter != 'J') { // JJJJJ
      m[best_letter] += m['J'];
      m['J'] = 0;
    }
  }

  std::array<int, 6> vals;
  vals.fill(0);
  for (const auto & [_, qtt] : m) {
    vals[qtt] += 1;
  }

  if (vals[5] == 1) return Type::FiveOfAKind;
  if (vals[4] == 1) return Type::FourOfAKind;
  if (vals[3] == 1) {
    if (vals[2] == 1) return Type::FullHouse;
    return Type::ThreeOfAKind;
  }

  if (vals[2] == 2) return Type::TwoPair;
  if (vals[2] == 1) return Type::OnePair;
  return Type::HighCard;
}

struct Hand {
  Type type;
  i64 value;

  i64 bet;

  explicit Hand(std::string s, i64 bet, bool joker) : bet(bet) {
    type = compute_type(s, joker);

    std::string_view card_order = joker ? card_order_b : card_order_a;

    value = 0;
    for (const char c : s) {
      value = value * 15 + (std::find(card_order.begin(), card_order.end(), c) - card_order.begin());
    }
  }

  bool operator<(const Hand & other) const {
    return std::pair(type, value) > std::pair(other.type, other.value);
  }
};

Output day_2023_07(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::array<i64, 2> answers;
  answers.fill(0);

  for (size_t part = 0; part != 2; ++part) {
    bool joker_is_joker = part == 1;

    std::vector<Hand> hands;

    for (const std::string & line : lines) {
      hands.emplace_back(line.substr(0, 5), std::stoi(line.substr(6)), joker_is_joker);
    }
    
    std::sort(hands.begin(), hands.end());

    for (size_t i = 0; i != hands.size(); ++i) {
      answers[part] += hands[i].bet * (i + 1);
    }
  }

  return Output(answers[0], answers[1]);
}
