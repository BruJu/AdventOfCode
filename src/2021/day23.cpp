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

////////////////////////////////////////////////////////////////////////////////
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
template <size_t Size>
struct AmphipodInfo {
  std::array<Amphipod, Size> * ptr_to_room;
  int room_y;
  int energy_per_move;
};

static char to_char(Amphipod frog) {
  switch (frog) {
    case Amphipod::Nobody: return '.';
    case Amphipod::Amber:  return 'A';
    case Amphipod::Bronze: return 'B';
    case Amphipod::Copper: return 'C';
    case Amphipod::Desert: return 'D';
    default:               return '?';
  }
}

template<size_t Size>
std::ostream & operator<<(
  std::ostream & stream, const std::array<Amphipod, Size> & column
) {
  stream << '(';

  for (size_t i = 0; i != Size; ++i) {
    if (i != 0) stream << ", ";
    stream << to_char(column[i]);
  }
  return stream << ')';
}

/**
 * Return the number of step to travail from from to to. -1 if the path is
 * obstructed by somebody
 */
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

////////////////////////////////////////////////////////////////////////////////
// ==== Grid

/** Positions where it is possible to stop in the hallway */
constexpr static auto hallway_stop_places = { 0, 1, 3, 5, 7, 9, 10 };

template <size_t Size>
struct Grid {
  std::array<Amphipod, 11> hallway;
  std::array<Amphipod, Size> amber_room;
  std::array<Amphipod, Size> bronze_room;
  std::array<Amphipod, Size> copper_room;
  std::array<Amphipod, Size> desert_room;

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
    amber_room[Size - 1] = char_to_amphipod(lines[3][3]);
    bronze_room[0] = char_to_amphipod(lines[2][5]);
    bronze_room[Size - 1] = char_to_amphipod(lines[3][5]);
    copper_room[0] = char_to_amphipod(lines[2][7]);
    copper_room[Size - 1] = char_to_amphipod(lines[3][7]);
    desert_room[0] = char_to_amphipod(lines[2][9]);
    desert_room[Size - 1] = char_to_amphipod(lines[3][9]);

    if (Size != 2 && Size != 4) {
      std::cout << "Unexpected Size\n";
    } else if (Size == 4) {
      // Hard coded part 2
      amber_room[1] = Amphipod::Desert;
      amber_room[2] = Amphipod::Desert;

      bronze_room[1] = Amphipod::Copper;
      bronze_room[2] = Amphipod::Bronze;

      copper_room[1] = Amphipod::Bronze;
      copper_room[2] = Amphipod::Amber;

      desert_room[1] = Amphipod::Amber;
      desert_room[2] = Amphipod::Copper;
    }
  }

  /** Return the info related to the movement of an amphipod type */
  [[nodiscard]] AmphipodInfo<Size> get_info(Amphipod type) {
    switch (type) {
      case Amphipod::Amber:
        return AmphipodInfo<Size> { &amber_room, 2, 1 };
      case Amphipod::Bronze:
        return AmphipodInfo<Size> { &bronze_room, 4, 10 };
      case Amphipod::Copper:
        return AmphipodInfo<Size> { &copper_room, 6, 100 };
      case Amphipod::Desert:
        return AmphipodInfo<Size> { &desert_room, 8, 1000 };
      default:
        std::cout << "Bad type in get_info\n";
        return AmphipodInfo<Size> { &desert_room, 8, 10000 };
    }
  }

  /** Return true if everybody is at the right place */
  [[nodiscard]] bool is_final_state() const {
    for (size_t i = 0; i != Size; ++i) {
      if (amber_room[i] != Amphipod::Amber) return false;
      if (bronze_room[i] != Amphipod::Bronze) return false;
      if (copper_room[i] != Amphipod::Copper) return false;
      if (desert_room[i] != Amphipod::Desert) return false;
    }
    return true;
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

  static size_t pos_of_first_invalid(const std::array<Amphipod, Size> & column, Amphipod expected) {
    size_t i = 0;
    while (i < Size && column[i] == Amphipod::Nobody) {
      ++i;
    }

    size_t i_save = i;

    while (i < Size && column[i] == expected) {
      ++i;
    }

    if (i < Size && column[i] == Amphipod::Nobody) {
      std::cout << "Invalid column: ";

      for (size_t j = 0 ; j != Size ; ++j) {
        std::cout << int(column[j]) << " ";
      }

      std::cout << "\n";
      exit(-1);
    }

    return i == Size ? i : i_save;
  }


  /**
   * Put a frog in the room for frogs of the given type in the hallway at position hallway_x
   */
  template<typename Consumer>
  void go_out(Consumer consumer, int hallway_x, Amphipod type) const {
    Grid copy = *this;

    // Look for a frog in the room of type
    auto [room, x_src, _] = copy.get_info(type);

    int frog_pos = int(pos_of_first_invalid(*room, type));
    if (frog_pos == Size) return;

    // Go to hallway
    const auto this_is_me = (*room)[frog_pos];
    
    // std::cout << "POP before= " << *room;
    (*room)[frog_pos] = Amphipod::Nobody;
    // std::cout << " after= " << *room << '\n';
    // std::cout << int(type) << " " << int((*room)[0]) << "+" << int((*room)[1]) << '\n';
    // std::cout << frog_pos << ' ' << int(this_is_me) << '\n';
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
    int frog_pos = int(pos_of_first_invalid(*destination, hallway[position]));
    if (frog_pos != Size) return;

    int room_slot = 0;
    while (room_slot != Size && (*destination)[room_slot] == Amphipod::Nobody) {
      ++room_slot;
    }
    --room_slot;

    used_energy += (room_slot + 1) * energy_per_move;

    // x movement
    const auto this_is_me = copy.hallway[position];
    copy.hallway[position] = Amphipod::Nobody;

    const auto steps = move_to(copy.hallway, position, destination_y);
    if (steps == -1) return;
    used_energy += steps * energy_per_move;

    // Both x and y movements are ok
    // std::cout << "before= " << *destination;
    (*destination)[room_slot] = this_is_me;
    // std::cout << "after= " << *destination << '\n';

    consumer(copy, used_energy);
  }

  friend std::ostream & operator<<(std::ostream & stream, const Grid<Size> & self) {
    stream << "#############\n";

    stream << "#";
    for (int i = 0; i < 11; ++i) {
      stream << to_char(self.hallway[i]);
    }
    stream << "#\n";

    stream << "###";
    stream << to_char(self.amber_room[0]);
    stream << "#";
    stream << to_char(self.bronze_room[0]);
    stream << "#";
    stream << to_char(self.copper_room[0]);
    stream << "#";
    stream << to_char(self.desert_room[0]);
    stream << "###\n";

    for (size_t i = 1; i != Size; ++i) {
      stream << "  #";
      stream << to_char(self.amber_room[i]);
      stream << "#";
      stream << to_char(self.bronze_room[i]);
      stream << "#";
      stream << to_char(self.copper_room[i]);
      stream << "#";
      stream << to_char(self.desert_room[i]);
      stream << "#  \n";
    }

    stream << "  #########  \n";

    return stream;
  }
};


template <size_t Size>
static long long int solve(const std::vector<std::string> & lines) {
  Grid<Size> grid(lines);

  std::optional<int> min_energy = std::nullopt;
  std::map<Grid<Size>, long long int> energy_to_reach;
  std::queue<Grid<Size>> not_explored_yet;

  energy_to_reach[grid] = 0;
  not_explored_yet.push(grid);

  Grid last = grid;

  while (!not_explored_yet.empty()) {
    const auto state = not_explored_yet.front();
    not_explored_yet.pop();

    const auto energy_to_now = energy_to_reach.find(state)->second;

    state.for_each_next_state([&](const Grid<Size> & grid, int energy) {
      energy += energy_to_now;
      last = grid;
      if (min_energy && *min_energy <= energy) {
        return;
      }

      if (grid.is_final_state()) {
        if (!min_energy || *min_energy > energy) {
          min_energy = energy;
          last = grid;
          return;
        }
      }

      auto x = energy_to_reach.find(grid);
      if (x != energy_to_reach.end()) {
        if (x->second < energy) return;
      }

      energy_to_reach[grid] = energy;
      not_explored_yet.push(grid);
    });
  }

  return min_energy ? *min_energy : -1;
}


Output day_2021_23(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const long long int part_a = solve<2>(lines);
  const long long int part_b = solve<4>(lines);

  return Output(part_a, part_b);
}
