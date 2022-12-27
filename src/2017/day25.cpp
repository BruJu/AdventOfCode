#include "../advent_of_code.hpp"
#include <vector>
#include <cstring>
#include <regex>
#include <map>

// https://adventofcode.com/2017/day/25
// This input format ;-;

struct Action {
  int write;
  int dx;
  char next_state;
};

struct StateDescription {
  Action on_0;
  Action on_1;
};

Output day_2017_25(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::regex prelude_regex { R"(Begin in state ([A-Z])\.\nPerform a diagnostic checksum after (\d+) steps?\.)" };
  std::regex kill_me_regex { R"(In state ([A-Z]):\n  If the current value is 0:\n    - Write the value (0|1)\.\n    - Move one slot to the (left|right)\.\n    - Continue with state ([A-Z])\.\n  If the current value is 1:\n    - Write the value (0|1)\.\n    - Move one slot to the (left|right)\.\n    - Continue with state ([A-Z])\.)" };

  std::smatch smatch;

  const std::string prelude = lines[0] + "\n" + lines[1];
  if (!std::regex_search(prelude, smatch, prelude_regex)) {
    throw std::runtime_error("bad prelude");
  }

  const char initial_state = smatch[1].str()[0];
  const int checksum_at = std::stoi(smatch[2].str());

  std::map<char, StateDescription> states;
  size_t i = 3;
  while (i < lines.size()) {
    const std::string state_description =
      lines[i] + "\n"   // In state
      + lines[i + 1] + "\n" // If the current value is
      + lines[i + 2] + "\n" // Write
      + lines[i + 3] + "\n" // Move
      + lines[i + 4] + "\n" // Continue
      + lines[i + 5] + "\n" // If the current value is
      + lines[i + 6] + "\n" // Write
      + lines[i + 7] + "\n" // Move
      + lines[i + 8]; // Continue

    if (!std::regex_search(state_description, smatch, kill_me_regex)) {
      throw std::runtime_error("bad state description");
    }

    states[smatch[1].str()[0]] =
      StateDescription{
        .on_0 = Action {
          .write = std::stoi(smatch[2].str()),
          .dx = smatch[3].str() == "left" ? -1 : 1,
          .next_state = smatch[4].str()[0]
        },
        .on_1 = Action {
          .write = std::stoi(smatch[5].str()),
          .dx = smatch[6].str() == "left" ? -1 : 1,
          .next_state = smatch[7].str()[0]
        }
      };

    i += 10;
  }

  std::map<int, int> turing_band;
  int pc = 0;
  char current_state = initial_state;

  constexpr auto apply_transition = [](
    int & pointed, int & pc, char & current_state,
    const Action & action
  ) {
    pointed = action.write;
    pc += action.dx;
    current_state = action.next_state;
  };

  for (int i = 0; i != checksum_at; ++i) {
    int & pointed = turing_band[pc];
    const StateDescription & state = states[current_state];

    if (pointed == 0) {
      apply_transition(pointed, pc, current_state, state.on_0);
    } else {
      apply_transition(pointed, pc, current_state, state.on_1);
    }
  }

  size_t as_one = 0;
  for (const auto & [_, value] : turing_band) {
    if (value) ++as_one;
  }

  return Output(as_one, "kthxbye");
}
