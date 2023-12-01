#include "../../common/advent_of_code.hpp"
#include "../../util/instruction_reader.hpp"
#include <numeric>

enum class MovementType {
  Forward, Down, Up
};

struct Movement {
  MovementType type;
  int qtt;
};

Output day_2021_02(const std::vector<std::string> & lines, const DayExtraInfo &) {
  bj::InstructionReader<Movement> converter;

  converter.add_handler(
    R"(forward ([0-9]+))",
    [](const std::vector<std::string> & values) -> Movement { return { MovementType::Forward, std::stoi(values[0]) }; }
  );

  converter.add_handler(
    R"(down ([0-9]+))",
    [](const std::vector<std::string> & values) -> Movement { return { MovementType::Down, std::stoi(values[0]) }; }
  );
  
  converter.add_handler(
    R"(up ([0-9]+))",
    [](const std::vector<std::string> & values) -> Movement { return { MovementType::Up, std::stoi(values[0]) }; }
  );

  const auto instructions = converter(lines).value();
  
  long long int a_h = 0;
  long long int a_depth = 0;
  
  long long int b_aim = 0;
  long long int b_depth = 0;
  long long int b_h = 0;

  for (const auto & instruction : instructions) {
    if (instruction.type == MovementType::Forward) {
      a_h += instruction.qtt;
      b_h += instruction.qtt;
      b_depth += b_aim * instruction.qtt;
    } else if (instruction.type == MovementType::Down) {
      a_depth += instruction.qtt;
      b_aim += instruction.qtt;
    } else if (instruction.type == MovementType::Up) {
      a_depth -= instruction.qtt;
      b_aim -= instruction.qtt;
    }
  }

  long long int part_a = a_h * a_depth;
  long long int part_b = b_h * b_depth; 
  
  return Output(part_a, part_b);
}
