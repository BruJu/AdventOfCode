#include "../advent_of_code.hpp"
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <ostream>
#include "../util/position.hpp"



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


const auto split_universe = {
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

long long int foo() {
  std::map<Config, long long int> cases;
  long long int myWin = 0;
  long long int hiswin = 0;

  cases[{ 0, 0, 10 - 1, 7 - 1 }] = 1;

  while (!cases.empty()) {
    std::cout << cases.size() << '\n';
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
  return Output(0, foo());
}


Output day_2021_21_a(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::array<int, 2> positions;
  positions[0] = 10;
  positions[1] = 7;

  std::array<int, 2> scores;
  scores[0] = 0;
  scores[1] = 0;

  int turnOf = 0;
  int diceRolls = 0;
  int nextRoll = 0;
  while (true) {

    int add = 3 * (nextRoll % 100) + 6;
    diceRolls += 3;
    nextRoll += 3;

    positions[turnOf] = (positions[turnOf] - 1 + add) % 10 + 1;
    scores[turnOf] += positions[turnOf];


    turnOf = 1 - turnOf;
    if (scores[1 - turnOf] >= 1000) break;
  }

  

  std::cout << scores[turnOf] << ", " << diceRolls << '\n';

  const long long int part_a = scores[turnOf] * diceRolls;


  const long long int part_b = 0;

  return Output(part_a, part_b);
}
