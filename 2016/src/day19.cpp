#include "../../common/advent_of_code.hpp"
#include <algorithm>
#include <vector>
#include <list>


Output day_2016_19(const std::vector<std::string> &, const DayExtraInfo & dei) {
  std::list<int> elves;

  for (int i = 0; i != dei.part_a_extra_param; ++i) {
    elves.push_back(i + 1);
  }


  auto elve = elves.begin();
  while (true) {
    ++elve;

    if (elve == elves.end()) {
      elve = elves.begin();
    }

    if (elves.size() == 1) {
      break;
    }

    elve = elves.erase(elve);
    if (elve == elves.end()) elve = elves.begin();
  }


  // PArt B
  int i = 1;
  while (i * 3 < dei.part_a_extra_param) {
    i *= 3;
  }

  return Output(elves.front(), dei.part_a_extra_param - i);
}
