#include "../../common/advent_of_code.hpp"
#include <map>
#include <variant>
#include <string>
#include <optional>
#include <exception>

// https://adventofcode.com/2022/day/7

// A disk content discovered with cd and dir
// (A) Some computation about subfolders size
// (B) Find the smallest folder whose size is at least something


// A folder
class Folder {
  // The content of a folder = association from names to folder|file
  std::map<std::string, std::variant<Folder, long long int>> m_content;
  // Parent folder
  Folder * m_parent;

public:
  Folder(Folder * parent) : m_parent(parent) {};

  // Because we point to the parent, we can't copy this nor move this
  Folder(const Folder &) = delete;
  Folder & operator=(const Folder &) = delete;
  ~Folder() = default;

  // Return the parent folder, or this if this is the root
  [[nodiscard]] Folder * get_parent_folder() { return m_parent ? m_parent : this; }
  
  // Return the child folder whose name is name. Throws if no such folder child. ".." = parent
  Folder * get_child(const std::string & name) {
    if (name == "..") return get_parent_folder();

    auto it = m_content.find(name);
    if (it == m_content.end()) {
      throw std::runtime_error("Bad cd (1)");
    }

    Folder * child = std::get_if<Folder>(&it->second);
    if (!child) throw std::runtime_error("Bad cd (2)");

    return child;
  }

  // Adds a new folder. Do nothing if already exists
  void add_folder(const std::string & name) {
    if (m_content.count(name) > 0) return;

    m_content.emplace(name, this);
  }

  // Adds a new file. Overwrites eventual already existing folder or file
  void add_file(const std::string & name, long long int size) {
    m_content.emplace(name, size);
  }

  // Returns the space used by this folder and its subfolders
  [[nodiscard]] long long int size() const noexcept {
    long long int size = 0;

    for (const auto & [_, content] : m_content) {
      // Too lazy to see check how std::variant works, and it's overkill, and the comitee
      // should just standardize std::overloaded already
      if (const Folder * folder = std::get_if<Folder>(&content)) {
        size += folder->size();
      } else if (const long long int * file = std::get_if<long long int>(&content)) {
        size += *file;
      }
    }

    return size;
  }

  // Some dumb things
  [[nodiscard]] long long int part_a() const noexcept {
    long long int result = 0;

    // A.1: If this folder size is at most 100000, accumulate its size
    const long long int my_size = size();
    if (my_size <= 100000) result += my_size;

    // A.2: Do the same for subfolders, even though it means a file can be counted
    // several times
    for (const auto & [_, content] : m_content) {
      if (const Folder * folder = std::get_if<Folder>(&content)) {
        result += folder->part_a();
      }
    }

    return result;
  }

  // A pair with the path to a folder and its size
  struct FreePlan {
    std::string path;
    long long int size;
  };

  // Find which folder to remove to have the required free diskspace
  [[nodiscard]] std::optional<FreePlan> oui_bonjour_allo_la_police_je_fais_telle_taille_et_jai_besoin_de_tant_de_place(
    long long int total_diskspace,
    long long int required_diskspace
  ) const {
    const long long int current_unused_space = total_diskspace - size();
    const long long int need_to_free = required_diskspace - current_unused_space;
    if (need_to_free < 0) return std::nullopt;
    return plan_space_free(need_to_free);
  }

  // Find the smallest (sub) folder whose size is at least space_to_free.
  [[nodiscard]] std::optional<FreePlan> plan_space_free(long long int space_to_free) const {
    const long long int my_size = size();
    if (my_size < space_to_free) return std::nullopt;

    FreePlan plan { "/", my_size };

    for (const auto & [name, content] : m_content) {
      if (const Folder * folder = std::get_if<Folder>(&content)) {
        const auto planified_in_folder = folder->plan_space_free(space_to_free);

        if (planified_in_folder && plan.size > planified_in_folder->size) {
          plan = std::move(planified_in_folder.value());
          plan.path = "/" + name + plan.path;
        }
      }
    }

    return plan;
  }
};


Output day_2022_07(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Folder root(nullptr);

  // Fill tree
  {
    Folder * current_folder = &root;

    for (const std::string & line : lines) {
      if (line == "$ cd /") {
        current_folder = &root;
      } else if (line == "$ ls") {
        // noop
      } else if (line.substr(0, 5) == "$ cd ") {
        current_folder = current_folder->get_child(line.substr(5));
      } else if (line[0] == '$') {
        throw std::runtime_error("bad reading");
      } else {
        const auto space_pos = line.find(' ');
        if (space_pos == std::string::npos) throw std::runtime_error("bad cd reading");

        const auto size = line.substr(0, space_pos);
        const auto name = line.substr(space_pos + 1);

        if (size == "dir") {
          current_folder->add_folder(name);
        } else {
          current_folder->add_file(name, std::stoll(size));
        }
      }
    }
  } // goodbye current_folder

  // Answer the questions
  return Output(
    root.part_a(),
    root.oui_bonjour_allo_la_police_je_fais_telle_taille_et_jai_besoin_de_tant_de_place(70000000ll, 30000000ll)->size
  );
}
