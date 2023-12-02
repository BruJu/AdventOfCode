#include "../../common/advent_of_code.hpp"
#include <span>
#include "../../util/position.hpp"

// https://adventofcode.com/2018/day/15

// Does not work

struct Goblin {
  bj::Position position;
  int hit_points = 200;
};

struct Elve {
  bj::Position position;
  int hit_points = 200;
};

struct Person {
  Goblin * g;
  Elve * e;
};

struct Game {
  std::vector<std::vector<bool>> m_walls;
  std::vector<Goblin> goblins;
  std::vector<Elve> elves;

  explicit Game(const std::vector<std::string> & lines);
  [[nodiscard]] bool finished() const;
  void round();
};

Game::Game(const std::vector<std::string> & lines) {
  for (int y = 0; std::cmp_less(y, lines.size()); ++y) {
    m_walls.emplace_back();
    for (int x = 0; std::cmp_less(y, lines[y].size()); ++x) {
      m_walls.back().emplace_back(lines[y][x] == '#');

      if (lines[y][x] == 'G') {
        goblins.emplace_back(Goblin{ bj::Position{ x, y } });
      } else if (lines[y][x] == 'E') {
        elves.emplace_back(Elve{ bj::Position{ x, y } });
      }
    }
  }
}

bool Game::finished() const {
  return goblins.empty() || elves.empty();
}

void Game::round() {
  std::vector<Person> persons;
  for (Elve & elve : elves) {
    persons.emplace_back(Person { nullptr, &elve });
  }
  
  for (Goblin & goblin : goblins) {
    persons.emplace_back(Person { &goblin, nullptr });
  }

  std::sort(
    persons.begin(), persons.end(),
    [](const Person & lhs, const Person & rhs) {
      bj::Position lhs_pos = lhs.g ? lhs.g->position : lhs.e->position;
      bj::Position rhs_pos = rhs.g ? rhs.g->position : rhs.e->position;
      return std::pair(lhs_pos.y, lhs_pos.x) < std::pair(rhs_pos.y, rhs_pos.x);
    }
  );

  


}


Output day_2018_15(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Game game(lines);

  while (game.finished()) {
    game.round();
  }

  return Output(0, 0);
}
