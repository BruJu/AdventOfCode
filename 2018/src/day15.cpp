#include "../../common/advent_of_code.hpp"
#include <span>
#include "../../util/position.hpp"
#include <variant>

// https://adventofcode.com/2018/day/15

// Does not work

struct Empty {};

struct Wall {};

struct Entity {
  enum class Race { Golbin, Elve };
  int id;
  Race race;
  int hit_points;
};

using X = std::variant<Empty, Wall, Entity>;

struct Game {
  std::vector<std::vector<X>> m_grid;

  explicit Game(const std::vector<std::string> & lines);
  [[nodiscard]] bool finished() const;
  void round();
};

Game::Game(const std::vector<std::string> & lines) {
  int entity_id = 0;
  for (int y = 0; std::cmp_less(y, lines.size()); ++y) {
    m_grid.emplace_back();
    for (int x = 0; std::cmp_less(y, lines[y].size()); ++x) {
      const char c = lines[y][x];

      if (c == '#') {
        m_grid.back().emplace_back(Wall{});
      } else if (c == 'G') {
        m_grid.emplace_back(Entity{ ++entity_id, Entity::Race::Golbin });
      } else if (c == 'E') {
        m_grid.emplace_back(Entity{ ++entity_id, Entity::Race::Elve });
      } else {
        m_grid.emplace_back(Empty{});
      }
    }
  }
}

bool Game::finished() const {
  return std::all_of(entities.begin(), entities.end(),
    [](const Entity & e) { return e.race == Entity::Race::Golbin; }
  )
  || std::all_of(entities.begin(), entities.end(),
    [](const Entity & e) { return e.race == Entity::Race::Elve; }
  );
}

void Game::round() {
  std::sort(
    entities.begin(), entities.end(),
    [](const Entity & lhs, const Entity & rhs) {
      bj::Position lhs_pos = lhs.position;
      bj::Position rhs_pos = rhs.position;
      return std::pair(lhs_pos.y, lhs_pos.x) < std::pair(rhs_pos.y, rhs_pos.x);
    }
  );

  size_t i = 0;
  while (i != entities.size()) {


    ++i;
  }
  
  std::vector<bj::Position> look_at;


  std::vector<Person> persons;
  for (Elve & elve : elves) {
    persons.emplace_back(Person { nullptr, &elve });
  }
  
  for (Goblin & goblin : goblins) {
    persons.emplace_back(Person { &goblin, nullptr });
  }

  


}


Output day_2018_15(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Game game(lines);

  while (game.finished()) {
    game.round();
  }

  return Output(0, 0);
}
