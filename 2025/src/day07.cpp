#include "../../common/advent_of_code.hpp"
#include <vector>
#include <algorithm>
#include <numeric>
#include "../../util/position.hpp"

// https://adventofcode.com/2025/day/7

namespace {

// Number of beams in each cell of a row
struct LineBeam {
  int y;
  std::vector<std::int64_t> beams;
  std::int64_t nb_of_splits;

  LineBeam(int y, size_t length) : y(y), beams(length, 0), nb_of_splits(0) {}

  [[nodiscard]] static
  LineBeam next(const LineBeam & previous, const std::vector<std::string> & lines) {
    LineBeam retval(previous.y + 1, previous.beams.size());
    retval.nb_of_splits = previous.nb_of_splits;

    for (size_t col = 0; col != previous.beams.size(); ++col) {
      if (previous.beams[col] == 0) continue;

      if (lines[previous.y + 1][col] == '^') {
        retval.beams[col - 1] += previous.beams[col];
        retval.beams[col + 1] += previous.beams[col];
        ++retval.nb_of_splits;
      } else {
        retval.beams[col] += previous.beams[col];
      }
    }

    return retval;
  }

};

[[nodiscard]]
bj::Position locate_S(const std::vector<std::string> & lines) {
  for (size_t y = 0; y != lines.size(); ++y) {
    const size_t x = lines[y].find('S');
    if (x != std::string::npos) {
      return bj::Position { static_cast<int>(x), static_cast<int>(y) };
    }
  }

  throw std::runtime_error("202507 No starting position found");
}

}

Output day_2025_07(const std::vector<std::string> & lines, const DayExtraInfo &) {
  // Find the S, beam is on the S
  bj::Position starting_pos = locate_S(lines);

  LineBeam line(starting_pos.y, lines[0].size());
  line.beams[starting_pos.x] = 1;

  // Move down until bottom line
  while (true) {
    if (line.y + 1 >= static_cast<int>(lines.size())) {
      break;
    }

    line = LineBeam::next(line, lines);
  }

  // Return result
  return Output(
    line.nb_of_splits,
    std::reduce(line.beams.begin(), line.beams.end())
  );
}
