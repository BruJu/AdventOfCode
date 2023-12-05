#include "../../common/advent_of_code.hpp"
#include <vector>
#include <span>
#include <utility>
#include <map>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <exception>

// https://adventofcode.com/2023/day/5


struct ElementRange {
  long long int min;
  long long int length;
};

struct Map {
  struct Range {
    long long int source;
    long long int destination;
    long long int length;
  };

  std::vector<Range> ranges;

  void sort() {
    std::sort(
      ranges.begin(),
      ranges.end(),
      [](const Range & lhs, const Range & rhs) {
        return lhs.source < rhs.source;
      }
    );
  }

  [[nodiscard]] long long int transform(long long int element) const {
    for (const Range & range : ranges) {
      if (element >= range.source && element < range.source + range.length) {
        return range.destination + element - range.source;
      }
    }

    return element;
  }

  [[nodiscard]] std::vector<ElementRange> transform(ElementRange to_transform) const {
    std::vector<long long int> seq_points;

    seq_points.emplace_back(to_transform.min);

    std::vector<ElementRange> transformed;

    for (const Range & range : ranges) {
      if (to_transform.length == 0) break;

      // range overlaps from the right
      //   AAA range
      // BBB   to_transform

      if (to_transform.min < range.source
        && to_transform.min + to_transform.length > range.source
      ) {
        long long int overlapping = range.source - to_transform.min;

        transformed.emplace_back(ElementRange{ to_transform.min, overlapping });
        
        to_transform.min = range.source;
        to_transform.length -= overlapping;
      }

      // range overlaps from the left
      // AAA   range
      //   BBB to_transform
      if (to_transform.min >= range.source
        && to_transform.min < range.source + range.length
      ) {
        long long int overlapping = range.source + range.length - to_transform.min;
        long long int really_overlapping = std::min(overlapping, to_transform.length);

        long long int distance = to_transform.min - range.source;

        transformed.emplace_back(ElementRange{ range.destination + distance, really_overlapping });
        to_transform.min += really_overlapping;
        to_transform.length -= really_overlapping;
        continue;
      }

    }

    if (to_transform.length != 0) {
      transformed.emplace_back(to_transform);
    }


    return transformed;
  }
};

std::vector<long long int> to_long_long_ints(const std::string & str) {
  std::vector<std::string> elements_str = bj::string_split(str, " ");

  std::vector<long long int> elements;
  for (const auto & str : elements_str) {
    elements.emplace_back(std::stoll(str));
  }

  return elements;
}

Output day_2023_05(const std::vector<std::string> & lines, const DayExtraInfo &) {
  // Read seeds
  std::vector<long long int> elements = to_long_long_ints(lines[0].substr(std::strlen("seeds: ")));

  std::vector<ElementRange> elements_ranges;
  for (size_t i = 0; i < elements.size(); i += 2) {
    elements_ranges.emplace_back(ElementRange{ elements[i], elements[i + 1] });
  }

  std::vector<Map> maps;

  // Read maps

  Map current_map;
  for (const std::string & line : std::vector(lines.begin() + 2, lines.end())) {
    if (line.empty()) {
      if (!current_map.ranges.empty()) {
        maps.emplace_back(current_map);
        current_map.ranges.clear();
      }
      continue;
    }

    if (line[0] >= '0' && line[0] <= '9') {
      const auto vals = to_long_long_ints(line);
      if (vals.size() != 3) throw std::runtime_error("Not 3 values");

      current_map.ranges.emplace_back(Map::Range{ vals[1], vals[0], vals[2] });
    }
  }

  if (!current_map.ranges.empty()) {
    maps.emplace_back(current_map);
  }

  // go

  for (Map & map : maps) {
    map.sort();
  }

  for (const auto & map : maps) {
    for (long long int & element : elements) {
      element = map.transform(element);
    }

    std::vector<ElementRange> result;
    for (const auto & elementRange : elements_ranges) {
      for (const auto & r : map.transform(elementRange)) {
        result.emplace_back(r);
      }
    }
    elements_ranges = result;
  }

  // find lowest

  const auto part_a = *std::min_element(elements.begin(), elements.end());

  const auto part_b = std::min_element(
    elements_ranges.begin(), elements_ranges.end(),
    [](const auto & lhs, const auto & rhs) { return lhs.min < rhs.min; }
  )->min;

  return Output(part_a, part_b);
}
