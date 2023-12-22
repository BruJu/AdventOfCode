#include "../../common/advent_of_code.hpp"
#include <vector>
#include <map>
#include "../../util/instruction_reader.hpp"
#include <queue>
#include <set>
#include <memory>

using i64 = std::int64_t;

i64 compute_lcm(std::span<const long long int> values);

// https://adventofcode.com/2023/day/20


class Module;

using SignalQueue = std::queue<std::tuple<bool, Module *, Module *>>;

class Module {
public:
  std::string name;
  Module(std::string name) : name(name) {}
  Module(const Module &) = delete;
  Module & operator=(const Module &) = delete;
  virtual ~Module() = default;

  std::vector<Module *> target;

  virtual void receive_pulse(SignalQueue & queue, bool high_pulse, Module *) = 0;

  void send_pulse(SignalQueue & queue, bool high_pulse) {
    for (Module * module : target) {
      queue.emplace(high_pulse, this, module);
    }
  }
};

class Broadcast : public Module {
public:
  Broadcast(std::string name) : Module(name) {}

  void receive_pulse(SignalQueue & queue, bool high_pulse, Module *) override {
    send_pulse(queue, high_pulse);
  }
};

class FlipFlop : public Module {
public:
  FlipFlop(std::string name) : Module(name) {}

  bool state = false;

  void receive_pulse(SignalQueue & queue, bool high_pulse, Module *) override {
    if (high_pulse) return;

    if (!state) {
      send_pulse(queue, true);
      state = true;
    } else {
      send_pulse(queue, false);
      state = false;
    }
  }
};

class Conjunction : public Module {
public:
  Conjunction(std::string name) : Module(name) {}

  std::map<Module *, bool> memory;

  void receive_pulse(SignalQueue & queue, bool high_pulse, Module * from) override {
    memory[from] = high_pulse;

    bool all_high = true;
    for (const auto & [_, prev_signal] : memory) {
      if (!prev_signal) {
        all_high = false;
        break;
      }
    }
    
    send_pulse(queue, !all_high);
  }
};


std::map<std::string, std::vector<int>> presses;

class Network {
public:
  i64 number_of_high = 0;
  i64 number_of_low  = 0;
  Module * button;
  std::map<std::string, std::unique_ptr<Module>> all_modules;

  static Network parse(const std::vector<std::string> & lines);

  template <typename Consumer>
  void press_button(Consumer consumer) {
    SignalQueue signal_queue;
    signal_queue.emplace(false, button, button);

    while (!signal_queue.empty()) {
      const auto [high_signal, from, to] = signal_queue.front();
      signal_queue.pop();

      if (high_signal) {
        ++number_of_high;
      } else {
        ++number_of_low;
      }

      consumer(from, to, high_signal);

      to->receive_pulse(signal_queue, high_signal, from);
    }
  }
};

Network Network::parse(const std::vector<std::string> & lines) {
  Network retval;

  std::map<Module *, std::vector<std::string>> links;

  const auto add_module = [&](std::string name) -> Module * {
    std::unique_ptr<Module> ptr;
    if (name[0] == '%') {
      name = name.substr(1);
      ptr = std::make_unique<FlipFlop>(name);
    } else if (name[0] == '&') {
      name = name.substr(1);
      ptr = std::make_unique<Conjunction>(name);
    } else {
      ptr = std::make_unique<Broadcast>(name);
    }

    return retval.all_modules.emplace(name, std::move(ptr)).first->second.get();
  };

  for (const std::string & line : lines) {
    const auto vec = bj::string_split(line, " ");

    std::string name = vec[0];

    Module * module = add_module(name);

    for (const std::string & sub : std::span(vec.begin() + 2, vec.end())) {
      if (sub.find(",") != std::string::npos) {
        links[module].emplace_back(sub.substr(0, sub.size() - 1));
      } else {
        links[module].emplace_back(sub);
      }
    }
  }

  const auto find_or_create_module = [&](const std::string & name) -> Module * {
    const auto it = retval.all_modules.find(name);
    if (it != retval.all_modules.end()) return it->second.get();

    return retval.all_modules.emplace(name, std::make_unique<Broadcast>(name)).first->second.get();
  };

  for (const auto & [from, tos] : links) {
    for (const auto & to_name : tos) {
      Module * to = find_or_create_module(to_name);
      from->target.emplace_back(to);

      if (Conjunction * conj = dynamic_cast<Conjunction *>(to)) {
        conj->memory[from] = false;
      }
    }

    for (const auto * dep : from->target) {
      if (!dep) throw std::runtime_error("Parsing error: null dependency");
    }
  }

  retval.button = retval.all_modules["broadcaster"].get();

  return retval;
}




Output day_2023_20(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  Network cartoon = Network::parse(lines);
  for (int i = 0; i != 1000; ++i) {
    cartoon.press_button([](auto, auto, auto) {});
  }

  const i64 part_a = cartoon.number_of_high * cartoon.number_of_low;


  i64 part_b = 0;
  if (!dei.can_skip_part_B) {
    cartoon = Network::parse(lines);
    std::map<std::string, i64> first_high_signal;

    // From the observation of the input, we can deduce that we need to find the first
    // time when hf, pm, pk and mk emits a high signal and then compute the ppcm to find
    // when they all send a high signal so rx also receives a high signal through vi

    i64 nb_of_presses = 0;

    while (first_high_signal.size() != 4) {
      ++nb_of_presses;
      cartoon.press_button([&](const Module * sender, const Module *, bool high_signal) {
        if (!high_signal) return;

        if (sender->name == "hf" || sender->name == "pm" || sender->name == "pk" || sender->name == "mk") {
          first_high_signal.emplace(sender->name, nb_of_presses); // emplace so if there is already a data it is not replaced
        }
      });

      std::vector<long long int> vals;
      for (const auto & [_, val] : first_high_signal) {
        vals.emplace_back(val);
      }

      part_b = compute_lcm(vals);
    }
  }

  return Output(part_a, part_b);
}
