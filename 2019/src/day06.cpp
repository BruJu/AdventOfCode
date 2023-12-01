#include "../../common/advent_of_code.hpp"
#include <string>
#include <map>

// https://adventofcode.com/2019/day/06


struct Planet {
  std::string name;
  Planet * orbits_around = nullptr;
  std::vector<Planet *> orbited_by;

  [[nodiscard]] std::vector<const Planet *> i_orbit_around() const {
    std::vector<const Planet *> planets;
    const Planet * current = this;
    while (current->orbits_around) {
      current = current->orbits_around;
      planets.emplace_back(current);
    }
    return planets;
  }
  
};


struct System {
  std::vector<Planet> planet_place;
  std::map<std::string, Planet *> names_to_planet;

  System(size_t capacity) {
    planet_place.reserve(capacity);
  }
  System(const System &) = delete;
  System & operator=(const System &) = delete;
  ~System() = default;

  Planet * get_planet(const std::string & name) {
    const auto it = names_to_planet.find(name);
    if (it == names_to_planet.end()) {
      Planet & new_planet = planet_place.emplace_back();
      new_planet.name = name;
      names_to_planet[name] = &new_planet;
      return &new_planet;
    } else {
      return it->second;
    }
  }

  Planet * get_root() {
    Planet * root = &planet_place[0];
    while (root->orbits_around) {
      root = root->orbits_around;
    }
    return root;
  }
};

Output day_2019_06(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  System planets(lines.size() * 2);

  for (const std::string & line : lines) {
    auto parenthesis = line.find(')');
    Planet * left = planets.get_planet(line.substr(0, parenthesis));
    Planet * right = planets.get_planet(line.substr(parenthesis + 1));

    left->orbited_by.emplace_back(right);
    right->orbits_around = left;
  }

  size_t a = 0;
  for (const Planet & planet : planets.planet_place) {
    a += planet.i_orbit_around().size();
  }

  size_t b = 0;
  if (!dei.can_skip_part_B) {
    Planet * you = planets.get_planet("YOU");
    Planet * san = planets.get_planet("SAN");

    // The last elements are the same
    const auto you_plan = you->i_orbit_around();
    const auto san_plan = san->i_orbit_around();

    auto it_you = you_plan.rbegin();
    auto it_san = san_plan.rbegin();

    size_t common_qtt = 0;
    while (*it_you == *it_san) {
      ++common_qtt;
      ++it_you;
      ++it_san;
    }

    // Boths are on the first different planet
    b = you_plan.size() + san_plan.size() - common_qtt - common_qtt;
  }
  
  return Output(a, b);
}

