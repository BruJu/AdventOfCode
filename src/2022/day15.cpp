#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include <cmath>
#include <map>
#include <ranges>
#include <regex>
#include <stdexcept>

// https://adventofcode.com/2022/day/15

static int man_distance(const bj::Position p1, const bj::Position p2) {
  const int xDiff = std::abs(p1.x - p2.x);
  const int yDiff = std::abs(p1.y - p2.y);
  return xDiff + yDiff;
}

struct Scanner {
  bj::Position scanner;
  bj::Position nearest_beacon;

  Scanner(bj::Position scanner, bj::Position beacon)
    : scanner(scanner), nearest_beacon(beacon) {}

  [[nodiscard]] int get_distance_to_beacon() const {
    return man_distance(scanner, nearest_beacon);
  }
};

static size_t part_a(const std::vector<Scanner> & scanners, int scanned_y) {
  enum class SlotStatus { Scanner, Beacon, Scanned };

  std::map<int, SlotStatus> status;

  // Place scanner and beacon
  for (const auto & pair : scanners) {
    if (pair.scanner.y == scanned_y) {
      status[pair.scanner.x] = SlotStatus::Scanner;
    }

    if (pair.nearest_beacon.y == scanned_y) {
      status[pair.nearest_beacon.x] = SlotStatus::Beacon;
    }
  }

  // scan points in the rectangle of each scanner-beacon pairs
  for (const auto & scanner_pair : scanners) {
    const auto & scanner = scanner_pair.scanner;
    const int distance_to_scanline = std::abs(scanner_pair.scanner.y - scanned_y);
    const int distance_to_beacon = scanner_pair.get_distance_to_beacon();

    /* optimization line */ if (distance_to_scanline > distance_to_beacon) continue;

    for (int dx = -distance_to_beacon; dx <= distance_to_beacon; ++dx) {
      bj::Position scanned_point { scanner.x + dx, scanned_y };

      if (man_distance(scanner, scanned_point) > distance_to_beacon) continue;

      status.try_emplace(scanned_point.x, SlotStatus::Scanned);
    }
  }

  // Only count points where there are no beacons
  size_t answer = 0;
  for (const SlotStatus status_1 : status | std::views::values) {
    if (status_1 != SlotStatus::Beacon) {
      ++answer;
    }
  }
  return answer;
}

static bool is_scanned_point(const std::vector<Scanner> & scanners, const bj::Position point) {
  for (const auto & pair : scanners) {
    const auto to_beacon = pair.get_distance_to_beacon();
    const auto to_point = man_distance(pair.scanner, point);

    if (to_beacon >= to_point) return true;
  }

  return false;
}

static bool is_in_bound(const bj::Position point, const int max) {
  return point.x >= 0 && point.x <= max && point.y >= 0 && point.y <= max;
}

static bj::Position part_b(const std::vector<Scanner> & scanners, int max) {
  // Observation (stolen from Reddit): there is only one point that is not scanned
  // so it is one unit off the range of a scanner

  for (const auto & pair : scanners) {
    const auto just_out_of_range = pair.get_distance_to_beacon() + 1;
    const bj::Position & scanner = pair.scanner;

    for (int i = 0; i <= just_out_of_range; ++i) {
      const auto points = {
        bj::Position{ scanner.x + just_out_of_range - i, scanner.y - i },
        bj::Position{ scanner.x - just_out_of_range + i, scanner.y + i },
        bj::Position{ scanner.x + just_out_of_range - i, scanner.y + i },
        bj::Position{ scanner.x - just_out_of_range + i, scanner.y - i },
      };

      for (const bj::Position & point : points) {
        if (is_in_bound(point, max) && !is_scanned_point(scanners, point)) {
          return point;
        }
      }
    }
  }

  return bj::Position{0, 0};
} 

Output day_2022_15(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<Scanner> scanners;

  const char * regex = R"(Sensor at x=(-?\d+), y=(-?\d+): closest beacon is at x=(-?\d+), y=(-?\d+))";
  std::regex regex_ { regex };
  std::smatch smatch_;

  for (const std::string & line : lines) {
    if (!std::regex_search(line, smatch_, regex_)) {
      throw std::runtime_error("bad input");
    }

    scanners.emplace_back(
      bj::Position{
        std::stoi(smatch_[1].str()),
        std::stoi(smatch_[2].str())
      },
      bj::Position{
        std::stoi(smatch_[3].str()),
        std::stoi(smatch_[4].str())
      }
    );
  }

  const bool is_example = scanners.size() == 14;

  const auto part_a_res = part_a(scanners, is_example ? 10 : 2000000);
  const auto part_b_almost_res = part_b(scanners, is_example ? 20 : 4000000);
  const std::uint64_t part_b_res = part_b_almost_res.x * std::uint64_t(4000000) + part_b_almost_res.y;

  return Output(part_a_res, part_b_res);
}
