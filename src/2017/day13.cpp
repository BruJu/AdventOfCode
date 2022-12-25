#include "../advent_of_code.hpp"
#include <vector>

// https://adventofcode.com/2017/day/13

struct Firewall {
  std::vector<int> layers;

  void set_scanner(int id, int height) {
    while (std::cmp_greater_equal(id, layers.size())) {
      layers.emplace_back(0);
    }

    layers[id] = height;
  }

  std::optional<int> try_to_pass(int wait_before) {
    bool caught = false;
    int severity = 0;
    int me = 0;
    while (std::cmp_less(me, layers.size())) {
      int value = layers[me];
      
      if (value != 0) {
        const int scanner_pos = (me + wait_before) % ((value - 1) * 2);

        if (scanner_pos == 0) {
          caught = true;
          severity += me * value;
        }
      }

      ++me;
    }

    if (!caught) return std::nullopt;
    return severity;
  }
};


Output day_2017_13(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Firewall firewall;
  for (const std::string & line : lines) {
    const auto values = bj::extract_ints_from_line(line);
    firewall.set_scanner(values[0], values[1]);
  }

  const auto yolo_strat = firewall.try_to_pass(0);

  int not_caught = 0;
  while (true) {
    const auto caught = firewall.try_to_pass(not_caught);
    if (caught) {
      ++not_caught;
    } else {
      break;
    }
  }

  return Output(yolo_strat.value_or(0), not_caught);
}
