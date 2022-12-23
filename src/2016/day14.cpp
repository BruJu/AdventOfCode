#include "../advent_of_code.hpp"
#include "../util/instruction_reader.hpp"
#include "../util/position.hpp"
#include <cstring>
#include <algorithm>
#include <optional>
#include <array>
#include <set>
#include <map>
#include <queue>
#include "../util/md5.h"

struct HashDescription {
  size_t key;
  char three_letters;
};

struct KeyFinder {
  std::string salt;
  size_t number_of_hashes = 1;
  size_t next_index_to_explore = 0;
  std::vector<size_t> validated;
  MD5 md5;

  KeyFinder(std::string salt, size_t number_of_hashes)
    : salt(salt), number_of_hashes(number_of_hashes) {

    }

  std::map<char, std::set<size_t>> candidates;

  void next();
  bool has_all_until(size_t size);
  void remove_candidates(size_t size);
  void validate_candidates(char c);
};

bool KeyFinder::has_all_until(size_t size) {
  if (validated.size() < size) return false;
  if (size == 0) return true;

  std::sort(validated.begin(), validated.end());
  const size_t s = validated[size - 1];

  for (const auto & [_, candidatess] : candidates) {
    for (const auto candidate : candidatess) {
      if (candidate < s + 1000) {
        return false;
      }
    }
  }

  return true;
}

void KeyFinder::validate_candidates(char c) {
  auto it = candidates.find(c);
  if (it == candidates.end()) return;

  validated.insert(validated.begin(), it->second.begin(), it->second.end());

  candidates.erase(it);
}

void KeyFinder::remove_candidates(size_t size) {
  auto it = candidates.begin();
  while (it != candidates.end()) {
    it->second.erase(size);

    if (it->second.empty()) {
      it = candidates.erase(it);
    } else {
      ++it;
    }
  }
}


void KeyFinder::next() {
  const std::string base = salt + std::to_string(next_index_to_explore);
  std::string hash = base;

  for (size_t i = 0; i != number_of_hashes; ++i) {
    std::string s = std::move(hash);
    hash = md5(s);
  }

  size_t c = 0;
  char last = hash[0] == 'a' ? 'b' : 'a';
  bool ok_3 = false;
  char to_validate;

  for (const char chr : hash) {
    if (last == chr) {
      ++c;

      if (c == 3 && !ok_3) {
        ok_3 = true;
        to_validate = last;
      } else if (c == 5) {
        validate_candidates(last);
      }
    } else {
      c = 1;
      last = chr;
    }
  }
  if (ok_3) {
    
    candidates[to_validate].emplace(next_index_to_explore);
  }

  if (next_index_to_explore >= 1000) {
    remove_candidates(next_index_to_explore - 1000);
  }

  ++next_index_to_explore;
}


Output day_2016_14(const std::vector<std::string> & lines, const DayExtraInfo &) {
  KeyFinder kf(lines[0], 1);

  while (!kf.has_all_until(64)) {
    kf.next();
  }

  
  KeyFinder kf2(lines[0], 2016 + 1);

  while (!kf2.has_all_until(64)) {
    kf2.next();
  }

  return Output(kf.validated[64-1], kf2.validated[64-1]);
}
