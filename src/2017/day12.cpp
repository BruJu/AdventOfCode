#include "../advent_of_code.hpp"
#include <map>
#include <vector>
#include <set>
#include <stack>

// https://adventofcode.com/2017/day/11


std::set<int> discover_group_of(const std::map<int, std::vector<int>> & programs, int leader) {
  std::set<int> known_connected;
  std::stack<int> to_visit;

  known_connected.emplace(leader);
  to_visit.emplace(leader);

  while (!to_visit.empty()) {
    int program = to_visit.top();
    to_visit.pop();

    const std::vector<int> & connecteds = programs.find(program)->second;

    for (const int connected_program : connecteds) {
      if (!known_connected.contains(connected_program)) {
        known_connected.emplace(connected_program);
        to_visit.emplace(connected_program);
      }
    }
  }

  return known_connected;
}



Output day_2017_12(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::map<int, std::vector<int>> programs;

  for (const std::string & line : lines) {
    std::vector<int> numbers = bj::extract_ints_from_line(line);

    int what = numbers[0];
    numbers.erase(numbers.begin());
    programs[what] = numbers;
  }

  size_t size_of_group_of_0 = discover_group_of(programs, 0).size();

  std::vector<int> programs_that_lead;
  std::set<int> grouped_programs;

  for (const auto & [program_id, _] : programs) {
    if (grouped_programs.contains(program_id)) continue;

    const auto my_group = discover_group_of(programs, program_id);

    programs_that_lead.emplace_back(program_id);

    for (const auto lead_program : my_group) {
      grouped_programs.emplace(lead_program);
    }
  }

  return Output(size_of_group_of_0, programs_that_lead.size());
}
