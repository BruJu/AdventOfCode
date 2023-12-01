#include "../../common/advent_of_code.hpp"
#include "../../util/regex_helper.hpp"
#include <vector>
#include <set>
#include <span>
#include <regex>
#include <ranges>

// https://adventofcode.com/2022/day/5

// A list of stacks with crates
// And some movements of crates from one stack to the other
//
// (A) Crates are moved one by one by elves that can only carry a crate
// one at a time; order of moved crates is reveresed
// (B) Santa bought a cranelift to move all boxes at one; order of moved
// crates is kept

namespace {

// We assume that instructions are correct
struct Stack {
private:
  std::vector<char> crates;

public:
  void add_top_crate(const char crate) { crates.push_back(crate); }
  [[nodiscard]] bool empty() const noexcept { return crates.empty(); }
  [[nodiscard]] char get_top_crate() const { return crates.back(); }

  void elve_move(Stack & target, int quantity) {
    target.crates.insert(target.crates.end(), crates.rbegin(), crates.rbegin() + quantity);

    const auto from = crates.begin() + (crates.size() - quantity);
    crates.erase(from, crates.end());
  }

  void cranelift_move(Stack & target, int quantity) {
    const auto from = crates.begin() + (crates.size() - quantity);
    target.crates.insert(target.crates.end(), from, crates.end());
    crates.erase(from, crates.end());
  }

  friend std::ostream & operator<<(std::ostream & stream, const Stack & self) {
    stream << "Stack(";
    for (const char crate : self.crates) {
      stream << crate;
    }
    return stream << ")";
  }
};

template <typename Values>
std::string compute_result(const Values & stacks) {
  std::string r;

  for (const Stack & stack : stacks) {
    if (!stack.empty()) {
      r += stack.get_top_crate();
    }
  }

  return r;
}

}

Output day_2022_05(const std::vector<std::string> & lines, const DayExtraInfo &) {
  // == Read original position
  // At worst, there are 9 stacks, numbered from 1 to 9
  std::array<Stack, 10> stacks_elves;
  std::array<Stack, 10> stacks_crane;

  // Read at reverse so we can just stack instead of inserting from the floor
  for (auto it = std::find(lines.rbegin(), lines.rend(), "") + 2; // skip empty line and stack numbering
    it != lines.rend();
    ++it
  ) {
    const std::string & line = *it;

    for (size_t i = 1; i < line.size(); i += 4) {
      if (line[i] == ' ') continue;

      const char crate = line[i];
      const size_t stack_id = 1 + i / 4;

      stacks_elves[stack_id].add_top_crate(crate);
      stacks_crane[stack_id].add_top_crate(crate);
    }
  }


  // == Compute and run instructions 
  std::regex regex_ { R"(move ([0-9]*) from ([0-9]*) to ([0-9]*))" };
  std::smatch smatch;

  const auto instructions_start = std::find(lines.begin(), lines.end(), "") + 1;
  for (auto it = instructions_start; it != lines.end(); ++it) {
    std::regex_search(*it, smatch, regex_);

    const int quantity = std::stoi(smatch[1].str());
    const int from     = std::stoi(smatch[2].str());
    const int to       = std::stoi(smatch[3].str());

    stacks_elves[from].elve_move     (stacks_elves[to], quantity);
    stacks_crane[from].cranelift_move(stacks_crane[to], quantity);
  }

  const std::string part_a = compute_result(stacks_elves);
  const std::string part_b = compute_result(stacks_crane);

  return Output(part_a, part_b);
}
