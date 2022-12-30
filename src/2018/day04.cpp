#include "../advent_of_code.hpp"
#include <map>
#include <compare>
#include <variant>
#include <string>

// https://adventofcode.com/2018/day/04

namespace {

struct Time {
  int year;
  int month;
  int day;
  int hours;
  int minutes;

  [[nodiscard]] constexpr auto operator<=>(const Time & other) const noexcept = default;

  Time(const std::string & log);
};

struct NewGuard { int number; };
struct FallsAsleep {};
struct WakesUp {};

struct Log {
  Time time;
  std::variant<NewGuard, FallsAsleep, WakesUp> action;

  static std::optional<Log> make(const std::string & raw);
};

std::optional<Log> Log::make(const std::string & raw) {
  const Time time(raw.substr(0, 17));
  const std::string rest = raw.substr(19);

  if (rest == "falls asleep") {
    return Log { .time = time, .action = FallsAsleep{} };
  } else if (rest == "wakes up") {
    return Log { .time = time, .action = WakesUp{} };
  } else {
    std::vector<int> guard_id = bj::extract_ints_from_line(rest);
    if (guard_id.size() != 1) return std::nullopt;
    return Log { .time = time, .action = NewGuard { guard_id[0] } };
  }
}

Time::Time(const std::string & log) {
  year = std::stoi(log.substr(1, 4));
  month = std::stoi(log.substr(6, 2));
  day = std::stoi(log.substr(9, 2));
  hours = std::stoi(log.substr(12, 2));
  minutes = std::stoi(log.substr(15 , 2));
}

}

Output day_2018_04(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<Log> logs;

  for (const std::string & line : lines) {
    const auto log = Log::make(line);
    if (!log) {
      throw std::runtime_error("bad log");
    }

    logs.emplace_back(log.value());
  }

  std::sort(logs.begin(), logs.end(),
    [](const Log & lhs, const Log & rhs) {
      return lhs.time < rhs.time;
    }
  );

  std::map<int, std::map<int, int>> guard_sleep;

  int current_guard = 0;
  std::optional<Time> sleeping_since;

  for (const Log & log : logs) {
    if (const NewGuard * guard = std::get_if<NewGuard>(&log.action)) {
      sleeping_since = std::nullopt;
      current_guard = guard->number;
    } else if (std::holds_alternative<FallsAsleep>(log.action)) {
      sleeping_since = log.time;
    } else if (std::holds_alternative<WakesUp>(log.action)) {
      if (sleeping_since) {
        for (int minute = sleeping_since->minutes; minute != log.time.minutes; ++minute) {
          ++guard_sleep[current_guard][minute];
          ++guard_sleep[current_guard][-1];
        }

        sleeping_since = std::nullopt;
      }
    }
  }

  struct WorstGuard {
    int id;
    int occurrences;
    int chosen_minute;

    [[nodiscard]] int badness() const { return id * chosen_minute; }
  };

  WorstGuard worst_a { 0, 0, 0 };
  WorstGuard worst_b { 0, 0, 0 };

  for (const auto & [guard_id, asleep] : guard_sleep) {
    // Part A
    if (asleep.find(-1)->second > worst_a.occurrences) {
      worst_a.id = guard_id;
      worst_a.occurrences = asleep.find(-1)->second;

      int mf = 0;
      for (const auto & [minute_id, times] : asleep) {
        if (times > mf && minute_id != -1) {
          mf = times;
          worst_a.chosen_minute = minute_id;
        }
      }
    }

    // Part B
    for (const auto & [minute_id, times] : asleep) {
      if (minute_id != -1 && worst_b.occurrences < times) {
        worst_b.id = guard_id;
        worst_b.occurrences = times;
        worst_b.chosen_minute = minute_id;
      }
    }
  }

  return Output(worst_a.badness(), worst_b.badness());
}
