#include "../../common/advent_of_code.hpp"
#include <map>
#include <exception>
#include <ranges>
#include "../../util/instruction_reader.hpp"
#include <variant>
#include <regex>
#include <span>
#include "../../util/position.hpp"
#include <queue>
#include <set>


// https://adventofcode.com/2016/day/22

namespace {

void set_in_grid(
  std::vector<std::vector<unsigned int>> & grid,
  const bj::Position position,
  const unsigned int capacity
) {
  while (std::cmp_less_equal(grid.size(), position.x)) {
    grid.emplace_back();
  }

  auto & line = grid[position.x];
  while (std::cmp_less_equal(line.size(), position.y)) {
    line.emplace_back(0);
  }

  line[position.y] = capacity;
}

struct CapacityGrid {
  std::vector<std::vector<unsigned int>> m_grid;

  [[nodiscard]] unsigned int operator[](const bj::Position position) const {
    return m_grid[position.x][position.y];
  }

};

struct DataGrid {
  std::vector<std::vector<unsigned int>> m_grid;
  bj::Position important_data_position;

  unsigned int operator[](const bj::Position position) const {
    return m_grid[position.x][position.y];
  }
  
  unsigned int & operator[](const bj::Position position) {
    return m_grid[position.x][position.y];
  }

  [[nodiscard]] bool can_move_data_to(const CapacityGrid & capacity, const bj::Position from, const bj::Position to) const {
    const auto from_data = operator[](from);
    const auto to_data = operator[](to);
    const auto to_max = capacity[to];
    const auto to_avail = to_max - to_data;

    return from_data <= to_avail;
  }

  void move_data_inplace(const bj::Position from, const bj::Position to) {
    operator[](to) += operator[](from);
    operator[](from) = 0;
    if (important_data_position == from) {
      important_data_position = to;
    }
  }

  [[nodiscard]] DataGrid move_data(const bj::Position from, const bj::Position to) const {
    DataGrid copy = *this;
    copy.move_data_inplace(from, to);
    return copy;
  }

};

}

namespace {



}


/*
struct MoveOperation {
  bj::Position from;
  bj::Position to;
};

struct GridState {
  std::map<bj::Position, Node> nodes;
  bj::Position data_location;

  explicit GridState(std::map<bj::Position, Node> nodes) {
    this->nodes = nodes;

    data_location = bj::Position{ 0, 0 };

    for (const auto & [position, _] : nodes) {
      if (position.y == 0 && position.x > data_location.x) {
        data_location = position;
      }
    }
  }

  GridState(const GridState & base, MoveOperation move_operation) {
    nodes = base.nodes;
    data_location = move_operation.from == base.data_location ? move_operation.to : base.data_location;

    Node * from = get_node_at(move_operation.from);
    Node * to = get_node_at(move_operation.to);
    from->move_data_to(*to);
  }

  bool is_useless() const {
    const Node * target = get_node_at(bj::Position{ 0, 0 });
    const Node * current_data = get_node_at(data_location);

    return target->avail + target->used < current_data->used;
  }
  
  [[nodiscard]] const Node * get_node_at(const bj::Position position) const {
    const auto it = nodes.find(position);
    if (it == nodes.end()) return nullptr;
    return &it->second;
  }

  [[nodiscard]] Node * get_node_at(const bj::Position position) {
    const auto it = nodes.find(position);
    if (it == nodes.end()) return nullptr;
    return &it->second;
  }

  // Arbitrary definition of operator< for std::set
  [[nodiscard]] bool operator<(const GridState & other) const {
    if (data_location < other.data_location) return true;
    if (other.data_location < data_location) return false;

    if (nodes.size() < other.nodes.size()) return true;
    if (nodes.size() > other.nodes.size()) return false;

    auto it_this = nodes.begin();
    auto it_other = other.nodes.begin();
    for (
      ;
      it_this != nodes.end();
      ++it_this, ++it_other
    ) {
      if (it_this->first < it_other->first) return true;
      if (it_other->first < it_this->first) return false;

      if (it_this->second.used < it_other->second.used) return true;
      if (it_this->second.used > it_other->second.used) return false;

      if (it_this->second.avail < it_other->second.avail) return true;
      if (it_this->second.avail > it_other->second.avail) return false;
    }

    return false;
  }

};

struct StateAndSteps {
  GridState state;
  size_t steps;

  explicit StateAndSteps(std::map<bj::Position, Node> nodes) : state(nodes), steps(0) { }

  StateAndSteps(const StateAndSteps & base, MoveOperation move_operation) : state(base.state, move_operation), steps(base.steps + 1) {}

  [[nodiscard]] bool is_final_state() const noexcept {
    return state.data_location == bj::Position{ 0, 0 };
  }

  [[nodiscard]] std::vector<MoveOperation> get_nexts() const {
    std::vector<MoveOperation> retval;

    for (const auto & [src_position, src_node] : state.nodes) {
      src_position.for_each_neighbour([&](const bj::Position dest_pos) {
        const Node * dest_node = state.get_node_at(dest_pos);
        if (!dest_node) return;
        if (!src_node.can_move_data_to(*dest_node)) return;
        retval.emplace_back(MoveOperation{ src_position, dest_pos });
      });
    }

    return retval;
  }
};

long long int compute_part_b(std::map<bj::Position, Node> nodes) {
  std::set<GridState> visited;
  std::queue<StateAndSteps> next_states;
  
  {
    StateAndSteps initial_state(nodes);
    next_states.emplace(initial_state);
    visited.emplace(initial_state.state);
  }

  while (!next_states.empty()) {
    const StateAndSteps state = next_states.front();
    next_states.pop();

    const std::vector<MoveOperation> next_operations = state.get_nexts();

    for (const auto move_operation : next_operations) {
      StateAndSteps next = StateAndSteps(state, move_operation);

      if (next.is_final_state()) {
        return next.steps;
      }

      if (next.state.is_useless()) {
        continue;
      }

      if (!visited.contains(next.state)) {
        next_states.emplace(next);
        visited.emplace(next.state);
      }
    }
  }

  return 0;
}

*/

Output day_2016_22(const std::vector<std::string> & lines, const DayExtraInfo &) {
  CapacityGrid capacity_grid;
  DataGrid data_grid;

  constexpr const char * LineRegex = R"(\/dev\/grid\/node-x(\d+)-y(\d+)\s+(\d+)T\s+(\d+)T\s+(\d+)T\s+(\d+)%)";
  std::regex regex_(LineRegex);

  std::smatch smatch_;

  for (size_t i = 2; i != lines.size(); ++i) {
    if (!std::regex_search(lines[i], smatch_, regex_))
      throw std::runtime_error("bad io");

    bj::Position position;
    position.x = std::stoi(smatch_[1].str());
    position.y = std::stoi(smatch_[2].str());

    // avail = std::stoi(smatch_[5].str());
    // used_percent = std::stoi(smatch_[6].str());

    set_in_grid(capacity_grid.m_grid, position, std::stoul(smatch_[3].str()));
    set_in_grid(data_grid.m_grid    , position, std::stoul(smatch_[4].str()));
  }

  long long int viable_pairs = 0;

  const auto grid_area = bj::Rectangle(0, 0, capacity_grid.m_grid.size() - 1, capacity_grid.m_grid[0].size() - 1);

  grid_area.for_each_position([&](const bj::Position from) {
    if (data_grid[from] == 0) return;

    grid_area.for_each_position([&](const bj::Position to) {
      if (from != to && data_grid.can_move_data_to(capacity_grid, from, to)) {
        ++viable_pairs;
      }
    });
  });

  data_grid.important_data_position = bj::Position{ grid_area.right, 0 };

  std::string part_b = "lol";
//  if (nodes.size() < 100) {
//    part_b = std::to_string(compute_part_b(nodes));
//  } else {
//    part_b = "Part B is not intended to be solved programmatically";
//  }

  for (int y = 0; y <= grid_area.bottom; ++y) {
    for (int x = 0; x <= grid_area.right; ++x) {
      bj::Position pos {x, y};
      std::cout << data_grid[pos] << '/' << capacity_grid[pos];

      std::cout << ";";
    }
    std::cout << "\n";
  }
  // 1/ Locate the empty grid
  // 2/ travel it to x = max, y = 0, count the steps
  // 3/ add to STEPS (max - 1) * 5

  return Output(viable_pairs, part_b);
}
