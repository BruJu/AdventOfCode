#include "../advent_of_code.hpp"
#include <vector>

// https://adventofcode.com/2018/day/09

static std::int64_t play_the_marble_game(const size_t nb_players, const int last_marble) {
  std::vector<std::int64_t> points_per_players(nb_players, 0);

  std::vector<int> circle;
  circle.emplace_back(0);

  size_t current_player = 0;
  size_t current_target = 0;
  for (int marble = 1; marble <= last_marble; ++marble) {

    if (marble % 23 != 0) {
      current_target = (current_target + 1) % circle.size();
      circle.insert(circle.begin() + current_target + 1, marble);
      ++current_target;
    } else {
      for(int i = 0; i != 7; ++i) {
        if (current_target == 0) {
          current_target = circle.size() - 1;
        } else {
          --current_target;
        }
      }

      const int earned_points = marble + circle[current_target];
      points_per_players[current_player] += earned_points;

      circle.erase(circle.begin() + current_target);
      if (current_target == circle.size()) {
        current_target = 0;
      }
    }

    current_player = (current_player + 1) % nb_players;
  }


  return *std::max_element(points_per_players.begin(), points_per_players.end());
}


Output day_2018_09(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  const std::vector<int> values = bj::extract_ints_from_line(lines[0]);

  const size_t nb_players = static_cast<size_t>(values[0]);
  const int last_marble = values[1];

  const auto part_a = play_the_marble_game(nb_players, last_marble);

  if (dei.can_skip_part_B) return Output(part_a, 0);

  return Output(
    part_a,
    play_the_marble_game(nb_players, last_marble * 100)
  );
}
