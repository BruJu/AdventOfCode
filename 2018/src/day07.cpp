#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <bitset>
#include <set>
#include <string>
#include <span>
#include <map>
#include <ranges>

// https://adventofcode.com/2018/day/07


namespace {

struct Input {
  std::set<char> all_steps;
  std::vector<std::pair<char, char>> requirements;
  
  std::map<char, std::vector<char>> prev_to_next;
  std::map<char, std::vector<char>> next_to_prev;

  explicit Input(const std::vector<std::string> & lines);
};

Input::Input(const std::vector<std::string> & lines) {
  for (const std::string & line : lines) {
    // Step C must be finished before step A can begin.
    // 012345678901234567890123456789012345678901234567
    //      ^ 5                            ^ 36

    requirements.emplace_back(std::pair<char, char>(line[5], line[36]));
    all_steps.emplace(line[5]);
    all_steps.emplace(line[36]);

    
    for (const auto & [required, next_task] : requirements) {
      prev_to_next[required].emplace_back(next_task);
      next_to_prev[next_task].emplace_back(required);
    }
  }
}


}

struct WorkInProgress {
  std::string order;
  int time = 0;
  std::set<char> completed_tasks;
  std::set<char> possible_tasks_to_do_now;
  size_t nb_available_workers;
  std::vector<std::pair<int, char>> currently_worked_on;

  explicit WorkInProgress(size_t nb_workers) {
    nb_available_workers = nb_workers;
  }

  void add_initial_states(const Input & input);
  bool tick(const Input & input, int buffed_time);

private:
  void add_new_tasks(int buffed_time);
  void resolve_ended_tasks(const Input & input);
  void add_new_possible_tasks(const Input & input, char finished_task);
};

void WorkInProgress::add_initial_states(const Input & input) {
  for (const char step : input.all_steps) {
    if (!input.next_to_prev.contains(step)) {
      possible_tasks_to_do_now.emplace(step);
    }
  }
}

bool WorkInProgress::tick(const Input & input, const int buffed_time) {
  if (possible_tasks_to_do_now.empty() && currently_worked_on.empty()) return false;
  add_new_tasks(buffed_time);
  ++time;
  resolve_ended_tasks(input);
  return true;
}


void WorkInProgress::add_new_tasks(int buffed_time) {
  while (nb_available_workers > 0 && !possible_tasks_to_do_now.empty()) {
    --nb_available_workers;

    const char do_now = *possible_tasks_to_do_now.begin();
    possible_tasks_to_do_now.erase(possible_tasks_to_do_now.begin());
    currently_worked_on.emplace_back(time + (do_now - 'A' + 1) + buffed_time, do_now);
    order += do_now;
  }
}

void WorkInProgress::resolve_ended_tasks(const Input & input) {
  auto it = currently_worked_on.begin();

  while (it != currently_worked_on.end()) {
    if (it->first != time) {
      ++it;
      continue;
    }

    add_new_possible_tasks(input, it->second);
    it = currently_worked_on.erase(it);
    ++nb_available_workers;
  }
}


void WorkInProgress::add_new_possible_tasks(const Input & input, const char finished_task) {
  completed_tasks.emplace(finished_task);

  // What can we do now?
  const auto nexts = input.prev_to_next.find(finished_task);
  if (nexts == input.prev_to_next.end()) return;

  for (const char possible_next : nexts->second) {
    const auto & previouses = input.next_to_prev.find(possible_next)->second;

    const bool all_previous_done = std::all_of(
      previouses.begin(), previouses.end(),
      [&](const char required) {
        return completed_tasks.contains(required);
      }
    );

    if (all_previous_done) {
      possible_tasks_to_do_now.emplace(possible_next);
    }
  }
}

static std::pair<std::string, int> part_a(const Input & input, size_t nb_workers, int buffed_time) {
  WorkInProgress wip(nb_workers);
  wip.add_initial_states(input);
  while (wip.tick(input, buffed_time));
  return { wip.order, wip.time };
}


template<typename T>
class CacheRegister {
private:
  std::vector<T> stored_values;

public:
  [[nodiscard]] bool contains(const T & value) const {
    const auto it = std::find(stored_values.begin(), stored_values.end(), value);
    return it != stored_values.end();
  }

  [[nodiscard]] const T & operator[](const size_t index) const {
    return stored_values[index];
  }

  [[nodiscard]] size_t operator[](const T & value) const {
    const auto it = std::find(stored_values.begin(), stored_values.end(), value);
    if (it == stored_values.end()) {
      const auto retval = stored_values.size();
      stored_values.emplace_back(value);
      return retval;
    } else {
      return it - stored_values.begin();
    }
  }
};


Output day_2018_07(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  Input input(lines);

  const std::string part_a_ = part_a(input, 1, 0).first;
  const int part_b_ = part_a(input, dei.part_b_extra_param, dei.part_b_extra_param == 5 ? 60 : 0).second;

  return Output(part_a_, part_b_);
}
