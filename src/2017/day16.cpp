#include "../advent_of_code.hpp"
#include <vector>
#include <map>

// https://adventofcode.com/2017/day/16



void dance(const std::string & instructions, std::string & sequence) {
  size_t i_ins = 0;
  
  const auto read_number = [&]() {
    int how_many = 0;
    while (i_ins < instructions.size() && instructions[i_ins] >= '0' && instructions[i_ins] <= '9') {
      how_many = how_many * 10 + (instructions[i_ins] - '0');
      ++i_ins;
    }
    return how_many;
  };

  while (i_ins < instructions.size()) {
    if (instructions[i_ins] == 's') {
      i_ins++;

      int how_many = read_number();

      sequence = sequence.substr(sequence.size() - how_many, how_many)
        + sequence.substr(0, sequence.size() - how_many);
    } else if (instructions[i_ins] == 'x') {
      i_ins++;

      int lhs = read_number();
      i_ins++;  // /
      int rhs = read_number();

      std::swap(sequence[lhs], sequence[rhs]);
    } else if (instructions[i_ins] == 'p') {
      char lhs = instructions[i_ins + 1];
      char rhs = instructions[i_ins + 3];
      i_ins += 4;

      const auto it_lhs = sequence.find(lhs);
      const auto it_rhs = sequence.find(rhs);
      std::swap(sequence[it_lhs], sequence[it_rhs]);
    } else if (instructions[i_ins] == ',') { // input is easier to parse than I expected
      ++i_ins;
    } else {
      std::cout << i_ins << " " << instructions[i_ins] << "\n";
      throw std::runtime_error("unexpected character");
    }
  }
}



Output day_2017_16(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::string instructions = lines[0];

  // Populate sequence
  std::string sequence;
  int nb = instructions.size() < 100 ? 5 : 16;
  for (int i = 0; i != nb; ++i) {
    sequence += static_cast<char>('a' + i);
  }

  // First dance
  std::uint64_t dance_id = 0;
  dance(instructions, sequence);
  const std::string after_first_dance = sequence;
  ++dance_id;

  // 1 billion dances
  if (!dei.can_skip_part_B) {
    // Find a cycle
    std::map<std::string, int> known_dances;
    known_dances[sequence] = dance_id;

    while (true) {
      dance(instructions, sequence);
      ++dance_id;

      auto it = known_dances.find(sequence);
      if (it == known_dances.end()) {
        known_dances[sequence] = dance_id;
      } else {
        // Found, simulate until before 1 billion
        const int offset = dance_id - it->second;

        while (dance_id + offset < 1000000000) {
          dance_id += offset;
        }
        
        break;
      }
    }

    // Finish
    while (dance_id < 1000000000) {
      dance(instructions, sequence);
      ++dance_id;
    }
  }

  // Ok
  return Output(after_first_dance, sequence);
}
