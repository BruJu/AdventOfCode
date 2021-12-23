#include "../advent_of_code.hpp"
#include <regex>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <optional>
#include <tuple>
#include <iostream>
#include <array>
#include <queue>
#include <cmath>

// https://adventofcode.com/2021/day/23

// ==== Amphipod

enum class Amphipod : std::uint8_t {
  Nobody, Amber, Bronze, Copper, Desert
};

/** Constructor */
static Amphipod char_to_amphipod(char c) {
  switch (c) {
    case 'A': return Amphipod::Amber;
    case 'B': return Amphipod::Bronze;
    case 'C': return Amphipod::Copper;
    case 'D': return Amphipod::Desert;
    default:
      std::cout << "Bad char " << c << '\n';
      return Amphipod::Nobody;
  }
}

/** Info of a frog in the lake */
struct AmphipodInfo {
  std::array<Amphipod, 2> * ptr_to_room;
  int room_y;
  int energy_per_move;
};


static int move_to(const std::array<Amphipod, 11> & layout, int from, int to) {
  int steps = 0;
  while (from != to) {
    if (from > to) {
      --from;
    } else {
      ++from;
    }

    if (layout[from] != Amphipod::Nobody) return -1;
    ++steps;
  }
  return steps;
}

constexpr static auto hallway_stop_places = {
  0, 1,     3,    5,    7,    9, 10
       /**/  /**/  /**/  /**/
};

struct Grid {
  std::array<Amphipod, 11> hallway;
  std::array<Amphipod, 2> amber_room;
  std::array<Amphipod, 2> bronze_room;
  std::array<Amphipod, 2> copper_room;
  std::array<Amphipod, 2> desert_room;

  bool operator<(const Grid & grid) const {
    if (hallway < grid.hallway) return true;
    if (hallway > grid.hallway) return false;

    if (amber_room < grid.amber_room) return true;
    if (amber_room > grid.amber_room) return false;

    if (bronze_room < grid.bronze_room) return true;
    if (bronze_room > grid.bronze_room) return false;

    if (copper_room < grid.copper_room) return true;
    if (copper_room > grid.copper_room) return false;

    if (desert_room < grid.desert_room) return true;
    if (desert_room > grid.desert_room) return false;

    return false;
  }

  explicit Grid(const std::vector<std::string> & lines) {
    hallway.fill(Amphipod::Nobody);
    amber_room[0] = char_to_amphipod(lines[2][3]);
    amber_room[1] = char_to_amphipod(lines[3][3]);
    bronze_room[0] = char_to_amphipod(lines[2][5]);
    bronze_room[1] = char_to_amphipod(lines[3][5]);
    copper_room[0] = char_to_amphipod(lines[2][7]);
    copper_room[1] = char_to_amphipod(lines[3][7]);
    desert_room[0] = char_to_amphipod(lines[2][9]);
    desert_room[1] = char_to_amphipod(lines[3][9]);
  }

  AmphipodInfo get_info(Amphipod type) {
    switch (type) {
      case Amphipod::Amber:
        return AmphipodInfo { &amber_room, 2, 1 };
      case Amphipod::Bronze:
        return AmphipodInfo { &bronze_room, 4, 10 };
      case Amphipod::Copper:
        return AmphipodInfo { &copper_room, 6, 100 };
      case Amphipod::Desert:
        return AmphipodInfo { &desert_room, 8, 1000 };
      default:
        std::cout << "Bad type in get_info\n";
        return AmphipodInfo { &desert_room, 8, 10000 };
    }
  }

  [[nodiscard]] bool is_final_state() const {
    return amber_room[0] == Amphipod::Amber
      && amber_room[1] == Amphipod::Amber
      && bronze_room[0] == Amphipod::Bronze
      && bronze_room[1] == Amphipod::Bronze
      && copper_room[0] == Amphipod::Copper
      && copper_room[1] == Amphipod::Copper
      && desert_room[0] == Amphipod::Desert
      && desert_room[1] == Amphipod::Desert;
  }

  template <typename Consumer>
  void for_each_next_state(Consumer consumer) const {
    for (const auto hallway_position : hallway_stop_places) {
      if (hallway[hallway_position] == Amphipod::Nobody) {
        go_out(consumer, hallway_position, Amphipod::Amber);
        go_out(consumer, hallway_position, Amphipod::Bronze);
        go_out(consumer, hallway_position, Amphipod::Copper);
        go_out(consumer, hallway_position, Amphipod::Desert);
      } else {
        go_home(consumer, hallway_position);
      }
    }
  }


  /**
   * Put a frog in the room for frogs of the given type in the hallway at position hallway_x
   */
  template<typename Consumer>
  void go_out(Consumer consumer, int hallway_x, Amphipod type) const {
    Grid copy = *this;

    // Look for a frog in the room of type
    auto [room, x_src, _] = copy.get_info(type);

    int frog_pos = 2;
    if ((*room)[0] != Amphipod::Nobody) {
      if ((*room)[0] == type && (*room)[1] == type) return;
      frog_pos = 0;
    } else if ((*room)[1] != Amphipod::Nobody) {
      if ((*room)[1] == type) return;
      frog_pos = 1;
    } else {
      // Both empty
      return;
    }

    // Go to hallway
    const auto this_is_me = (*room)[frog_pos];
    (*room)[frog_pos] = Amphipod::Nobody;

    auto [_1, _2, energy_to_move] = copy.get_info(this_is_me);
    int used_energy = energy_to_move * (1 + frog_pos);

    // Go to hallway_x
    int steps = move_to(copy.hallway, x_src, hallway_x);
    if (steps == -1) return;

    copy.hallway[hallway_x] = this_is_me;
    used_energy += energy_to_move * steps;
    consumer(copy, used_energy);
  }

  template <typename Consumer>
  void go_home(Consumer consumer, int position) const {
    if (hallway[position] == Amphipod::Nobody) return;

    Grid copy = *this;
    auto [destination, destination_y, energy_per_move]
      = copy.get_info(hallway[position]);

    long long int used_energy = 0;

    // y movement
    int room_slot = -1;
    if ((*destination)[1] == Amphipod::Nobody) {
      room_slot = 1;
      used_energy += energy_per_move * 2;
    } else if ((*destination)[1] != hallway[position]) {
      // not a friend, won't go in front of them
      return;
    } else if ((*destination)[0] == Amphipod::Nobody) {
      room_slot = 0;
      used_energy += energy_per_move;
    } else {
      return;
    }

    // x movement
    const auto this_is_me = copy.hallway[position];
    copy.hallway[position] = Amphipod::Nobody;

    const auto steps = move_to(copy.hallway, position, destination_y);
    if (steps == -1) return;
    used_energy += steps * energy_per_move;

    // Both x and y movements are ok
    (*destination)[room_slot] = this_is_me;

    consumer(copy, used_energy);
  }

  void draw() const {
    constexpr auto draw_ = [](Amphipod a) {
      if (a == Amphipod::Nobody) std::cout << ".";
      else if (a == Amphipod::Amber) std::cout << 'A';
      else if (a == Amphipod::Bronze) std::cout << 'B';
      else if (a == Amphipod::Copper) std::cout << 'C';
      else if (a == Amphipod::Desert) std::cout << 'D';
      else std::cout << '?';
    };
    std::cout << "#############\n";

    std::cout << "#";
    for (int i = 0; i < 11; ++i) {
      draw_(hallway[i]);
    }
    std::cout << "#\n";

    std::cout << "###";
    draw_(amber_room[0]);
    std::cout << "#";
    draw_(bronze_room[0]);
    std::cout << "#";
    draw_(copper_room[0]);
    std::cout << "#";
    draw_(desert_room[0]);
    std::cout << "###\n";

    std::cout << "  #";
    draw_(amber_room[1]);
    std::cout << "#";
    draw_(bronze_room[1]);
    std::cout << "#";
    draw_(copper_room[1]);
    std::cout << "#";
    draw_(desert_room[1]);
    std::cout << "#  \n";

    std::cout << "  #########  \n";
  }
};



Output day_2021_23(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Grid grid(lines);

  std::optional<int> min_energy = std::nullopt;
  std::map<Grid, long long int> energy_to_reach;
  std::queue<Grid> not_explored_yet;
  std::map<Grid, Grid> parent;

  energy_to_reach[grid] = 0;
  not_explored_yet.push(grid);

  Grid last = grid;

  while (!not_explored_yet.empty()) {
    const auto state = not_explored_yet.front();
    not_explored_yet.pop();

    const auto energy_to_now = energy_to_reach.find(state)->second;

    state.for_each_next_state([&](const Grid & grid, int energy) {
      energy += energy_to_now;
      last = grid;
      if (min_energy && *min_energy <= energy) {
        return;
      }

      if (grid.is_final_state()) {
        std::cout << "final!\n";
        if (!min_energy || *min_energy > energy) {
          min_energy = energy;
          last = grid;
          parent.emplace(grid, state);
          return;
        }
      }

      auto x = energy_to_reach.find(grid);
      if (x != energy_to_reach.end()) {
        if (x->second < energy) return;
      }

      energy_to_reach[grid] = energy;
      parent.insert_or_assign(grid, state);
      not_explored_yet.push(grid);
    });
  }

  std::cout << energy_to_reach.size() << " explored states\n";


//  for (const auto & p : energy_to_reach) {
//    p.first.draw();
//    std::cout << p.second << "\n\n";
//  }

//  last.draw();
//
//  while (true) {
//    auto it = parent.find(last);
//    if (it == parent.end()) break;
//    last = it->second;
//    last.draw();
//  }

  return Output(min_energy ? *min_energy : -1, 0);
}
