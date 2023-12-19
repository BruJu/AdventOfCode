#include "../../common/advent_of_code.hpp"
#include <vector>
#include <map>
#include <queue>
#include "../../util/instruction_reader.hpp"

using i64 = std::int64_t;

// https://adventofcode.com/2023/day/18

namespace {

struct Rule {
  char tested_rating;
  bool inferior;
  int  value;
  std::string next_workflow;
};

struct Workflow {
  std::string       name;
  std::vector<Rule> rules;
  std::string       final_rule;
};

class WorkflowChart {
private:
  std::map<std::string, Workflow> m_map;

public:
  explicit WorkflowChart(std::span<const Workflow> workflows) {
    for (const Workflow & workflow : workflows) {
      m_map.emplace(workflow.name, workflow);
    }
  }

  [[nodiscard]] const Workflow & get(const std::string & workflow_name) const noexcept(false) {
    const auto it = m_map.find(workflow_name);
    if (it == m_map.end()) {
      std::cerr << "Unknown workflow " << workflow_name << "\n";
      throw std::runtime_error("Unknown workflow");
    }

    return it->second;
  }
};

struct MachinePart {
  int x;
  int m;
  int a;
  int s;

  [[nodiscard]] bool test(const Rule & sub) const {
    int me = sub.tested_rating == 'x' ? x :
             sub.tested_rating == 'm' ? m :
             sub.tested_rating == 'a' ? a :
                               s;
    
    if (sub.inferior) {
      return me < sub.value;
    } else {
      return me > sub.value;
    }
  }

  [[nodiscard]] std::string find_next_workflow(const Workflow & instruction) const {
    for (const Rule & rule : instruction.rules) {
      if (test(rule)) {
        return rule.next_workflow;
      }
    }
    return instruction.final_rule;
  }

  [[nodiscard]] i64 get_total_rating() const noexcept {
    return x + m + a + s;
  }
};

struct Interval {
  int min;
  int max;

  i64 get_width() const {
    return max - min;
  }

  [[nodiscard]] std::pair<std::optional<Interval>, std::optional<Interval>> splice(bool inferior, int value) const {
    using ReturnType = std::pair<std::optional<Interval>, std::optional<Interval>>;

    if (inferior) {
      if (value <= min) {
        return ReturnType(std::nullopt, *this);
      } else if (value < max) {
        Interval yes = *this;
        Interval no = *this;
        yes.max = value;
        no.min = value;
        
        return ReturnType(yes, no);
      } else {
        return ReturnType(*this, std::nullopt);
      }
    } else {
      if (value < min) {
        return ReturnType(*this, std::nullopt);
      } else if (value < max - 1) {
        Interval yes = *this;
        Interval no = *this;
        no.max = value + 1;
        yes.min = value + 1;
        
        return ReturnType(yes, no);
      } else {
        return ReturnType(std::nullopt, *this);
      }
    }
  }

  [[maybe_unused]] static void test() {
    Interval interval{ 4, 8 };

    for (int i = 0; i != 10; ++i) {
      std::cout << "< "  << i << " : " << interval.splice(true, i) << "\n";
    }
    
    for (int i = 0; i != 10; ++i) {
      std::cout << "> "  << i << " : " << interval.splice(false, i) << "\n";
    }
  }

  friend std::ostream & operator<<(std::ostream & stream, Interval interval) {
    return stream << '[' << interval.min << ", " << interval.max << "[";
  }

  friend std::ostream & operator<<(std::ostream & stream, std::optional<Interval> interval) {
    if (interval) {
      stream << *interval;
    } else {
      stream << "0";
    }

    return stream;
  }

  friend std::ostream & operator<<(std::ostream & stream, std::pair<std::optional<Interval>, std::optional<Interval>> intervals) {
    return stream << intervals.first << " / " << intervals.second;
  }
};

struct GroupOfMachineParts {
  std::array<Interval, 4> ratings; // x m a s

  [[nodiscard]] std::pair<std::optional<GroupOfMachineParts>, std::optional<GroupOfMachineParts>> splice(const Rule & rule) const {
    size_t index = rule.tested_rating == 'x' ? 0 :
                   rule.tested_rating == 'm' ? 1 :
                   rule.tested_rating == 'a' ? 2 :
                                               3;
    
    const auto [accepted, rejected] = ratings[index].splice(rule.inferior, rule.value);

    return std::pair(from_spliced_interval(index, accepted), from_spliced_interval(index, rejected));
  }
  
  std::optional<GroupOfMachineParts> from_spliced_interval(size_t i, std::optional<Interval> interval) const {
    if (interval) {
      GroupOfMachineParts copy = *this;
      copy.ratings[i] = *interval;
      return copy;
    } else {
      return std::nullopt;
    }
  }

  [[nodiscard]] i64 get_number_in_group() const noexcept {
    return ratings[0].get_width() * ratings[1].get_width() * ratings[2].get_width() * ratings[3].get_width();
  }

  [[nodiscard]] static GroupOfMachineParts initial_group() {
    GroupOfMachineParts p;
    for (size_t i = 0; i != 4; ++i) {
      p.ratings[i].min = 1;
      p.ratings[i].max = 4000 + 1;
    }
    return p;
  }
};

std::pair<WorkflowChart, std::vector<MachinePart>> parse(const std::vector<std::string> & lines) {
  constexpr auto parse_workflow = [](std::string str) -> Workflow {
    Workflow workflow;
    
    size_t bracket = str.find('{');
    workflow.name = str.substr(0, bracket);

    str = str.substr(bracket + 1);
    str = str.substr(0, str.size() - 1);

    for (const std::string & substr : bj::string_split(str, ",")) {
      const size_t column = substr.find(":");

      if (column != std::string::npos) {
        Rule rule;
        rule.tested_rating = substr[0];
        if (substr[1] == '>') {
          rule.inferior = false;
        } else if (substr[1] == '<') {
          rule.inferior = true;
        } else {
          throw std::runtime_error("bad parsing of rule");
        }
        rule.value = std::stoi(substr.substr(2, column - 2));

        rule.next_workflow = substr.substr(column + 1);

        workflow.rules.emplace_back(rule);
      } else {
        workflow.final_rule = substr;
      }
    }

    return workflow;
  };

  bj::InstructionReader<MachinePart> machine_part_reader;
  machine_part_reader.add_handler(R"(\{x=(\d*),m=(\d*),a=(\d*),s=(\d*)\})",
    [](const std::vector<std::string> & vals) -> MachinePart {
      return MachinePart {
        .x = std::stoi(vals[0]),
        .m = std::stoi(vals[1]),
        .a = std::stoi(vals[2]),
        .s = std::stoi(vals[3])
      };
    });

  bool on_workflows = true;

  std::vector<Workflow> workflows;
  std::vector<MachinePart> parts;

  for (const std::string & str : lines) {
    if (str == "") {
      on_workflows = false;
    } else if (on_workflows) {
      workflows.emplace_back(parse_workflow(str));
    } else {
      parts.emplace_back(machine_part_reader(str).value());
    }
  }

  WorkflowChart workflow_chart(workflows);

  return std::pair(workflow_chart, parts);
}

i64 solve_part_a(const WorkflowChart & workflow_chart, std::span<const MachinePart> machine_parts) {
  i64 answer = 0;

  for (const MachinePart & part : machine_parts) {
    std::string current_workflow = "in";

    while (current_workflow != "A" && current_workflow != "R") {
      const Workflow & workflow = workflow_chart.get(current_workflow);
      current_workflow = part.find_next_workflow(workflow);
    }

    if (current_workflow == "A") {
      answer += part.get_total_rating();
    }
  }

  return answer;
}

i64 solve_part_b(const WorkflowChart & workflow_chart) {
  i64 answer = 0;

  std::queue<std::pair<std::string, GroupOfMachineParts>> todo;

  todo.emplace("in", GroupOfMachineParts::initial_group());

  while (!todo.empty()) {
    const auto [workflow_name, parts] = todo.front();
    todo.pop();

    if (workflow_name == "R") {
      continue;
    }
    if (workflow_name == "A") {
      answer += parts.get_number_in_group();
      continue;
    }

    const Workflow & workflow = workflow_chart.get(workflow_name);

    std::optional<GroupOfMachineParts> current = parts;
    for (const Rule & rule : workflow.rules) {
      if (!current) break;

      const auto [yes, no] = current->splice(rule);

      if (yes) {
        todo.emplace(rule.next_workflow, *yes);
      }

      current = no;
    }

    if (current) {
      todo.emplace(workflow.final_rule, *current);
    }
  }

  return answer;
}

}

Output day_2023_19(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto [workflow_chart, parts] = parse(lines);
  return Output(
    solve_part_a(workflow_chart, parts),
    solve_part_b(workflow_chart)
    );
}
