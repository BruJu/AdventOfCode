#include "../advent_of_code.hpp"
#include <vector>
#include <map>

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

  bool operator<(const Config & conf) const {
    if (myScore + hisScore < conf.myScore + conf.hisScore) return true;
    if (myScore + hisScore > conf.myScore + conf.hisScore) return false;

    if (myScore < conf.myScore) return true;
    if (myScore > conf.myScore) return false;

    if (hisScore < conf.hisScore) return true;
    if (hisScore > conf.hisScore) return false;
    
    if (myPosition < conf.myPosition) return true;
    if (myPosition > conf.myPosition) return false;
    
    if (hisPosition < conf.hisPosition) return true;
    if (hisPosition > conf.hisPosition) return false;

    return false;
  }
};

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

static long long int part_b(int position1, int position2) {
  std::map<Config, long long int> cases;
  long long int myWin = 0;
  long long int hiswin = 0;

  cases[{ 0, 0, position1 - 1, position2 - 1 }] = 1;

  while (!cases.empty()) {
    // ====
    auto it = cases.begin();

    auto config = it->first;
    auto paths = it->second;
    cases.erase(it);

    // ====

    for (const int roll : split_universe) {
      auto clone = config;
      clone.advance(roll);

      if (clone.myScore >= 21) {
        myWin += paths;
      } else {

        for (const int opponent_roll: split_universe) {
          auto opp = clone;
          opp.opponentAdvance(opponent_roll);

          if (opp.hisScore >= 21) {
            hiswin += paths;
          } else {
            auto it = cases.find(opp);
            if (it == cases.end()) cases[opp] = paths;
            else it->second += paths;
          }
        }
      }
    }
  }

  return std::max(myWin, hiswin);
}

Output day_2021_21(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const int p1 = std::stoi(lines[0].substr(28));
  const int p2 = std::stoi(lines[1].substr(28));

  return Output(part_a(p1, p2), part_b(p1, p2));
}
