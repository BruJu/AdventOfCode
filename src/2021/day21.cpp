#include "../advent_of_code.hpp"
#include <vector>
#include <map>
#include <set>
#include <ostream>
#include "../util/position.hpp"



Output day_2021_21(const std::vector<std::string> & lines, const DayExtraInfo &) {
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
