#include "../../common/advent_of_code.hpp"

// https://adventofcode.com/2022/day/10

class CathodeRay_Tube {
private:
  int cycle_number = 1;
  int register_V = 1;

  int signal_strength_sum = 0;
  std::string line_buffer = "";

public:
  void next_cycle() {
    ++cycle_number;

    if (cycle_number % 40 == 20) {
      signal_strength_sum += cycle_number * register_V;
    }
  }

  void add_value(int value) { register_V += value; }

  [[nodiscard]] auto get_signal_strength_sum() const noexcept {
    return signal_strength_sum;
  }

  void draw_crt() {
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
  }
};

Output day_2022_10(const std::vector<std::string> & lines, const DayExtraInfo &) {
  CathodeRay_Tube tube;
  
  for (const std::string & line : lines) {
    tube.draw_crt();
    tube.next_cycle();
    
    if (line != "noop") {
      tube.draw_crt();
      const auto space_pos = line.find(' ');
      const int addx_value = std::stoi(line.substr(space_pos + 1));
      tube.add_value(addx_value);
      tube.next_cycle();
    }
  }
  
  return Output(tube.get_signal_strength_sum(), "Oui");
}
