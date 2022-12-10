#include "../advent_of_code.hpp"

// https://adventofcode.com/2022/day/10

Output day_2022_10(const std::vector<std::string> & lines, const DayExtraInfo &) {
  int cycle_number = 1;
  int register_V = 1;

  // Part B
  std::string line_buffer = "";
  const auto draw_crt = [&]() {
    // Decide pixel
    if (std::abs(((cycle_number - 1) % 40) - register_V) <= 1) {
      line_buffer += '#';
    } else {
      line_buffer += '.';
    }

    // Full line = print
    if (cycle_number % 40 == 0) {
      std::cout << line_buffer << '\n';
      line_buffer = "";
    }
  };

  // Part A
  int signal_strength_sum = 0;

  const auto next_cycle = [&]() {
    ++cycle_number;

    if (cycle_number % 40 == 20) {
      signal_strength_sum += cycle_number * register_V;
    }
  };

  // Read instructions
  for (const std::string & line : lines) {
    draw_crt();
    next_cycle();
    
    if (line != "noop") {
      draw_crt();
      const auto space_pos = line.find(' ');
      const int addx_value = std::stoi(line.substr(space_pos + 1));
      register_V += addx_value;
      next_cycle();
    }
  }
  
  return Output(signal_strength_sum, "Oui");
}
