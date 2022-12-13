#include "../advent_of_code.hpp"
#include <map>
#include <exception>
#include <ranges>
#include "../util/instruction_reader.hpp"
#include <variant>
#include <regex>
#include <span>
#include "../util/position.hpp"
#include <queue>
#include <set>


// https://adventofcode.com/2016/day/22

struct Node {
  int used;
  int avail;

  [[nodiscard]] bool can_move_data_to(const Node & other) const noexcept {
    return used <= other.avail;
  }

  void move_data_to(Node & other) {
    other.used += used;
    other.avail -= used;
    avail += used;
    used = 0;
  }
};

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

Output day_2016_22(const std::vector<std::string> & lines, const DayExtraInfo &) {
  constexpr const char * LineRegex = R"(\/dev\/grid\/node-x(\d+)-y(\d+)\s+(\d+)T\s+(\d+)T\s+(\d+)T\s+(\d+)%)";
  std::regex regex_(LineRegex);

  std::smatch smatch_;

  std::map<bj::Position, Node> nodes;
  for (size_t i = 2; i != lines.size(); ++i) {
    if (!std::regex_search(lines[i], smatch_, regex_))
      throw std::runtime_error("bad io");

    bj::Position position;
    position.x = std::stoi(smatch_[1].str());
    position.y = std::stoi(smatch_[2].str());

    Node node;
    //node.size = std::stoi(smatch_[3].str());
    node.used = std::stoi(smatch_[4].str());
    node.avail = std::stoi(smatch_[5].str());
    //node.used_percent = std::stoi(smatch_[6].str());
    nodes.emplace(position, node);
  }

  long long int viable_pairs = 0;

  for (const Node & lhs : nodes | std::views::values) {
    for (const Node & rhs : nodes | std::views::values) {
      if (&lhs == &rhs) continue;

      if (lhs.used == 0) continue;

      if (lhs.used <= rhs.avail) {
        ++viable_pairs;
      }
    }
  }

  std::string part_b;
  if (nodes.size() < 100) {
    part_b = std::to_string(compute_part_b(nodes));
  } else {
    part_b = "Part B is not intended to be solved programmatically";
  }

  return Output(viable_pairs, part_b);
}
