#include "../../common/advent_of_code.hpp"
#include <vector>
#include <algorithm>
#include <numeric>
#include "../../util/position.hpp"
#include <set>
#include <map>
#include <stack>
#include <climits>
#include <queue>

// https://adventofcode.com/2025/day/10

namespace {

/// A machine = expected output, joltage and list of pressable buttons
struct Machine {
  // Expected output in part A [.##.] part in input
  std::vector<int> expected;
  // Expected output in part B {numbers} part in input
  std::vector<int> joltage;
  // List of buttons and which light they affect (numbers) part in input
  std::vector<std::vector<int>> buttons;

  /// Build a machine from the input line
  explicit Machine(const std::string & line) {
    // [.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
    size_t i_line = 1;

    while (line[i_line] != ']') {
      expected.emplace_back(line[i_line] == '#');
      ++i_line;
    }

    ++i_line; // Skip ]
    ++i_line; // slip space

    // (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
    // let's consider that we can randomly have some {} in the middle of ()
    bool in_button = false;
    bool in_joltage = false;
    int current_number = 0;

    while (i_line < line.size()) {
      if (line[i_line] == '(') {
        buttons.emplace_back();
        in_button = true;
      } else if (line[i_line] == '{') {
        in_joltage = true;
      } else if (line[i_line] == ' ') {
        // noop
      } else if (line[i_line] == '}') {
        joltage.emplace_back(current_number);
        current_number = 0;
        in_joltage = false;
      } else if (line[i_line] == ')') {
        buttons.back().emplace_back(current_number);
        current_number = 0;
        in_button = false;
      } else if (line[i_line] == ',') {
        if (in_button) {
          buttons.back().emplace_back(current_number);
        } else if (in_joltage) {
          joltage.emplace_back(current_number);
        } else {
          throw std::runtime_error("202510 bad input found , outside of brackets");
        }

        current_number = 0;
      } else if (line[i_line] >= '0' && line[i_line] <= '9') {
        current_number = current_number * 10 + (line[i_line] - '0');
      } else {
        throw std::runtime_error("202510 bad input");
      }

      ++i_line;
    }
  }

  /// Gives back the string representation
  [[nodiscard]] std::string to_string() const {
    std::string str;
    str += "[";
    for (const int v : expected) {
      if (v) {
        str += '#';
      } else {
        str += '.';
      }
    }

    str += "]";

    for (const auto & button : buttons) {
      str += " (";
      bool first = true;
      for (const auto light : button) {
        if (!first) str += ',';
        first = false;
        str += std::to_string(light);
      }
      str += ')';
    }

    str += " {";
    bool first = true;
    for (const auto & joltage1 : joltage) {
      if (!first) str += ',';
      first = false;
      str += std::to_string(joltage1);
    }
    str += "}";
    return str;
  }

};

/// A state in part A solution exploration
struct State_Part_A {
  // The machine
  const Machine * machine;
  // Lastly pressed button. We can't press button that are before
  int last_pressed_button;
  // Number of pressed buttons
  int number_of_pressed_buttons;
  // Currently lit lights
  std::vector<int> state;

  // Build a state in which we pressed nothing
  explicit State_Part_A(const Machine * machine) : machine(machine) {
    last_pressed_button = -1;
    number_of_pressed_buttons = 0;
    state.resize(machine->expected.size(), false);
  }

  // Create a state in which we pressed the given button
  State_Part_A press(int button_number) {
    State_Part_A clone = *this;
    clone.number_of_pressed_buttons += 1;
    clone.last_pressed_button = button_number;

    for (const auto light : machine->buttons[button_number]) {
      clone.state[light] = !clone.state[light];
    }
    return clone;
  }

  // True if the lights are what are expected
  [[nodiscard]] bool is_final() const {
    for (size_t i = 0; i != state.size(); ++i) {
      if (state[i] != machine->expected[i]) {
        return false;
      }
    }

    return true;
  }
};

// Return the number of button to have the wanted lights in part A
// (ignoring joltage)
[[nodiscard]]
int do_part_a(const Machine & machine) {
  // BFS
  if (State_Part_A(&machine).is_final()) {
    return 0;
  }

  // Initial state = we pressed nothing
  std::queue<State_Part_A> states;
  states.emplace(&machine);

  while (!states.empty()) {
    State_Part_A state = states.front();
    states.pop();

    // Let's press on all buttons
    for (int button_number = state.last_pressed_button + 1; button_number < machine.buttons.size(); ++button_number) {
      State_Part_A new_state = state.press(button_number);
      if (new_state.is_final()) return new_state.number_of_pressed_buttons;
      states.emplace(new_state);
    }
  }

  return -1;
}

// PART B: we can now press multiple times on a button

// Resolution state to prune cases in which we pressed too much on a button
enum class ResolutionState {
  Possible,
  Solved,
  Impossible
};

// State in part B
struct State_Part_B {
  // Still the machine
  const Machine * machine;
  // Number of pressed buttons
  int number_of_pressed_buttons;
  // Lights state
  std::vector<int> state;

  // Empty state
  explicit State_Part_B(const Machine * machine) : machine(machine) {
    number_of_pressed_buttons = 0;
    state.resize(machine->expected.size(), 0);
  }

  // Press on the button to increate joltage
  State_Part_B press(int button_number) const {
    State_Part_B clone = *this;
    clone.number_of_pressed_buttons += 1;

    for (const auto light : machine->buttons[button_number]) {
      clone.state[light] += 1;
    }
    return clone;
  }

  // Returns the resolution state:
  // - Impossible: a light is too lit
  // - Solved: all lights are perfectly lit according to expected joltage
  // - Possible: some lights are missing joltage
  [[nodiscard]] ResolutionState get_resolution_state() const {
    bool all_valid = true;
    for (size_t i = 0; i != state.size(); ++i) {
      if (state[i] > machine->joltage[i]) {
        return ResolutionState::Impossible;
      } else if (state[i] < machine->joltage[i]) {
        all_valid = false;
      }
    }

    return all_valid ? ResolutionState::Solved : ResolutionState::Possible;
  }
};

// Solve for part B = we press on buttons until we have the right joltage
// for all
[[nodiscard]]
int do_part_b(const Machine & machine) {
  if (State_Part_B(&machine).get_resolution_state() == ResolutionState::Solved) {
    return 0;
  }

  std::queue<State_Part_B> states;
  states.emplace(&machine);

  std::set<std::vector<int>> already_explored;

  while (!states.empty()) {
    State_Part_B state = states.front();
    states.pop();

    if (already_explored.contains(state.state)) {
      continue;
    } else {
      already_explored.emplace(state.state);
    }

    // Let's still press on all buttons
    for (int button_number = 0; button_number < machine.buttons.size(); ++button_number) {
      State_Part_B new_state = state;
      new_state = new_state.press(button_number);
      const auto state = new_state.get_resolution_state();
      if (state == ResolutionState::Impossible) {
        break;
      } else if (state == ResolutionState::Possible) {
        states.emplace(new_state);
      } else {
        return new_state.number_of_pressed_buttons;
      }
    }
  }

  return -1;
}

}

Output day_2025_10(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::int64_t part_a = 0, part_b = 0;

  for (const std::string & line : lines) {
    Machine machine(line);

    part_a += do_part_a(machine);
    if (lines.size() == 1) {
      // Part B doesn't scale even if:
      // - we sort the buttons to explore the buttons that
      // impact the most lights first
      // - we sort the buttons to explore the buttons that
      // affect the rarest lights first
      // - we press multiple times on a chosen button
      // (as much as we can) before moving on
      //
      // So we do it for fun on the small examples, and solve
      // the real input with z3
      part_b += do_part_b(machine);
    }
  }

  std::string part_b_answer;
  if (part_b != 0) {
    part_b_answer = std::to_string(part_b);
  } else {
    part_b_answer = "Yeah, let's z3 this";
  }
  
  return Output(part_a, part_b_answer);
}
