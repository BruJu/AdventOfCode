#include "../advent_of_code.hpp"
#include <vector>
#include <unordered_map>
#include <tuple>

struct Config {
  int myScore;
  int hisScore;
  int myPosition;
  int hisPosition;

  void advance(int roll) {
    myPosition = (myPosition + roll) % 10;
    myScore += 1 + myPosition;
  }

  void opponentAdvance(int roll) {
    hisPosition = (hisPosition + roll) % 10;
    hisScore += 1 + hisPosition;
  }

  bool operator==(const Config & rhs) const {
    return myScore == rhs.myScore && hisScore == rhs.hisScore
      && myPosition == rhs.myPosition && hisPosition == rhs.hisPosition;
  }
};

namespace std {
  template <> struct hash<Config> {
    size_t operator()(const Config & x) const {
      return ((x.myScore * 32 + x.hisScore) * 32 + x.myPosition) * 16 + x.hisPosition;
    }
  };
}

static constexpr auto split_universe = {
  1 + 1 + 1, 1 + 1 + 2, 1 + 1 + 3,
  1 + 2 + 1, 1 + 2 + 2, 1 + 2 + 3,
  1 + 3 + 1, 1 + 3 + 2, 1 + 3 + 3,

  2 + 1 + 1, 2 + 1 + 2, 2 + 1 + 3,
  2 + 2 + 1, 2 + 2 + 2, 2 + 2 + 3,
  2 + 3 + 1, 2 + 3 + 2, 2 + 3 + 3,

  3 + 1 + 1, 3 + 1 + 2, 3 + 1 + 3,
  3 + 2 + 1, 3 + 2 + 2, 3 + 2 + 3,
  3 + 3 + 1, 3 + 3 + 2, 3 + 3 + 3,
};

static long long int part_a(int position1, int position2) {
  Config config { 0, 0, position1 - 1, position2 - 1 };

  int turnOf = 0;
  int diceRolls = 0;
  while (true) {
    int add = 3 * (diceRolls % 100) + 6;
    diceRolls += 3;

    if (turnOf == 0) {
      config.advance(add);
      if (config.myScore >= 1000) return config.hisScore * diceRolls;
    } else {
      config.opponentAdvance(add);
      if (config.hisScore >= 1000) return config.myScore * diceRolls;
    }

    turnOf = 1 - turnOf;
  }
}

using UniversalWins = std::pair<long long int, long long int>;

static UniversalWins compute_win(Config current_config, std::unordered_map<Config, UniversalWins> & cache) {
  const auto it = cache.find(current_config);
  if (it != cache.end()) {
    return it->second;
  }

  UniversalWins universal = UniversalWins(0, 0);
  for (const int roll : split_universe) {
    auto clone = current_config;
    clone.advance(roll);

    if (clone.myScore >= 21) {
      universal.first += 1;
    } else {

      for (const int opponent_roll : split_universe) {
        auto opp = clone;
        opp.opponentAdvance(opponent_roll);

        if (opp.hisScore >= 21) {
          universal.second += 1;
        } else {
          auto next = compute_win(opp, cache);
          universal.first += next.first;
          universal.second += next.second;
        }
      }
    }
  }

  cache.emplace(current_config, universal);
  return universal;
}

static long long int part_b(int position1, int position2) {
  std::unordered_map<Config, UniversalWins> cache;
  const auto [myWin, hiswin] = compute_win(
    Config{ 0, 0, position1 - 1, position2 - 1 },
    cache
  );

  return std::max(myWin, hiswin);
}

Output day_2021_21(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const int p1 = std::stoi(lines[0].substr(28));
  const int p2 = std::stoi(lines[1].substr(28));

  return Output(part_a(p1, p2), part_b(p1, p2));
}
