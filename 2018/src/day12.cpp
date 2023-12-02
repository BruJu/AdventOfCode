#include "../../common/advent_of_code.hpp"
#include <span>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <cstring>

// https://adventofcode.com/2018/day/12

static std::vector<std::int64_t> to_state(const std::string & line) {
  std::vector<std::int64_t> retval;
  for (size_t i = 0; i != line.size(); ++i) {
    if (line[i] == '#') {
      retval.emplace_back(static_cast<std::int64_t>(i));
    }
  }
  return retval;
}

static size_t get_id_of(bool ll, bool l, bool m, bool r, bool rr) {
  return (ll ? 1 << 4 : 0)
    + (l ? 1 << 3 : 0)
    + (m ? 1 << 2 : 0)
    + (r ? 1 << 1 : 0)
    + (rr ? 1 << 0 : 0);
}

static std::array<bool, 1 << 5> make_rules(std::span<const std::string> input) {
  std::array<bool, 1 << 5> retval;
  retval.fill(false);

  for (const std::string & str_rule : input) {
    const size_t id = get_id_of(str_rule[0] == '#', str_rule[1] == '#', str_rule[2] == '#', str_rule[3] == '#', str_rule[4] == '#');

    // ##### => #
    // 0123456789
    retval[id] = str_rule[9] == '#';
  }

  return retval;
}

[[nodiscard]] static std::vector<std::int64_t> advance_state(
  const std::vector<std::int64_t> & state,
  const std::array<bool, 1 << 5> & rules
  ) {
  if (state.empty()) return {};

  const int from = state.front() - 2;
  const int to = state.back() + 2;

  std::vector<std::int64_t> retval;

  for (int i = from; i <= to; ++i) {
    const size_t id = get_id_of(
      std::binary_search(state.begin(), state.end(), i - 2),
      std::binary_search(state.begin(), state.end(), i - 1), 
      std::binary_search(state.begin(), state.end(), i    ), 
      std::binary_search(state.begin(), state.end(), i + 1), 
      std::binary_search(state.begin(), state.end(), i + 2)
    );

    if (rules[id]) {
      retval.emplace_back(i);
    }
  }

  return retval;
}

struct When {
  std::int64_t offset;
  std::int64_t round;
};

struct LoopFinder {
  std::map<std::vector<std::int64_t>, When> patterns;

  std::optional<When> add(std::vector<std::int64_t> values, std::int64_t round) {
    std::cout << round << " " << patterns.size() << "\n";
    std::int64_t offset = normalize_values(values);

    auto it = patterns.find(values);
    if (it == patterns.end()) {
      patterns.emplace(values, When { .offset = offset, .round = round });
      return std::nullopt;
    } else {
      return When {
        .offset = offset - it->second.offset,
        .round = round - it->second.round
      };
    }
  }

  std::int64_t normalize_values(std::vector<std::int64_t> & values) {
    std::int64_t offset = values[0];

    for (std::int64_t & value : values) {
      value -= offset;
    }

    return offset;
  }
};


std::int64_t compute(
  std::vector<std::int64_t> state,
  std::array<bool, 1 << 5> & rules,
  std::int64_t bound
) {
  std::int64_t step = 0;

  LoopFinder lf;
  When loop;

  while (true) {
    auto n_state = advance_state(state, rules);
    std::swap(state, n_state);
    ++step;

    if (step == bound) return std::reduce(state.begin(), state.end());

    const auto optgap = lf.add(state, step);
    if (optgap) {
      loop = optgap.value();
      break;
    }
  }

  const std::int64_t k = (bound - step) / loop.round;
  const std::int64_t buffered_off = loop.offset * k;
  step += loop.round * k;

  for (std::int64_t & value : state) {
    value += buffered_off;
  }

  while (step < bound) {
    auto n_state = advance_state(state, rules);
    std::swap(state, n_state);
    ++step;
  }

  return std::reduce(state.begin(), state.end());
}


Output day_2018_12(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  auto state = to_state(lines[0].substr(std::strlen("initial state: ")));
  std::array<bool, 1 << 5> rules = make_rules(std::span(lines.begin() + 2, lines.end()));

  const auto part_a = compute(state, rules, 20);
  const auto part_b = dei.can_skip_part_B ? 0ll : compute(state, rules, 50000000000ll);
  return Output(part_a, part_b);
}
