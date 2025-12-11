#include "../../common/advent_of_code.hpp"
#include <vector>
#include <map>

// https://adventofcode.com/2025/day/11

namespace {

// Nodes -> next nodes
using Network = std::map<std::string, std::vector<std::string>>;

// Read the input
Network build_network(const std::vector<std::string> & lines) {
  Network map;
  // aaa: bbb ccc ddd eee

  for (const std::string & line : lines) {
    std::string from = line.substr(0, 3);

    for (size_t i = 5 /* "xxx: " */; i < line.size(); i += 4) {
      map[from].emplace_back(line.substr(i, 3));
    }
  }

  return map;
}

namespace part_a {
size_t count_paths_to(const Network & network, const std::string & current_pos, const std::string & final_pos) {
  if (current_pos == final_pos) {
    return 1;
  }

  const auto next = network.find(current_pos);
  if (next == network.end()) return 0;

  size_t acc = 0;
  for (const auto & next_pos : next->second) {
    acc += count_paths_to(network, next_pos, final_pos);
  }

  return acc;
}
}

namespace part_b {

// A current situation with pos and encountered required nodes
// I feel like we could do memoization better by storing if from this position,
// we can encounter adc and fft but this is fast enought by storing if we already
// encountered them which is the basic memoization process.
struct Situation {
  std::string pos; // Current pos?
  bool dac; // encountered dac
  bool fft; // encountered fft

  bool operator<(const Situation & other) const {
    return std::tie(pos, dac, fft) < std::tie(other.pos, other.dac, other.fft);
  }
};

using Memoizer = std::map<Situation, size_t>;

size_t count_paths_to_impl(const Network & network, Situation current_situation, const std::string & final_pos, Memoizer & memoizer) {
  // End cases
  if (current_situation.pos == final_pos) {
    if (current_situation.dac && current_situation.fft) {
      return 1;
    } else {
      return 0;
    }
  }

  // Already memoized
  const auto it_memo = memoizer.find(current_situation);
  if (it_memo != memoizer.end()) {
    return it_memo->second;
  }

  // Evaluate
  const auto evaluate = [&]() -> size_t {
    const auto next = network.find(current_situation.pos);
    if (next == network.end()) return 0;

    size_t acc = 0;
    for (const auto & next_pos : next->second) {
      Situation next_situation;
      next_situation.pos = next_pos;
      next_situation.dac = current_situation.dac || (current_situation.pos == "dac");
      next_situation.fft = current_situation.fft || (current_situation.pos == "fft");

      acc += count_paths_to_impl(network, next_situation, final_pos, memoizer);
    }

    return acc;
  };

  // Store value for further use
  const size_t evaluation = evaluate();
  memoizer[current_situation] = evaluation;
  return evaluation;
}


size_t count_paths_to(const Network & network, const std::string & current_pos, const std::string & final_pos) {
  Memoizer memoizer;
  Situation sit;
  sit.pos = current_pos;
  sit.dac = sit.fft = false;
  return count_paths_to_impl(network, sit, final_pos, memoizer);
}

}

}

Output day_2025_11(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  // Let's assume there are no cycleS.
  // Algorithm to check if no cycles: while (stable) { remove terminal nodes }
  // This should end up with the empty graph.
  
  std::map<std::string, std::vector<std::string>> network = build_network(lines);

  const std::int64_t res_part_a = dei.can_skip_part_A ? 0 : static_cast<std::int64_t>(part_a::count_paths_to(network, "you", "out"));
  const std::int64_t res_part_b = dei.can_skip_part_B ? 0 : static_cast<std::int64_t>(part_b::count_paths_to(network, "svr", "out"));

  return Output(res_part_a, res_part_b);
}
