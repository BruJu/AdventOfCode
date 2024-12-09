#include "../../common/advent_of_code.hpp"
#include <map>
#include <span>
#include <ranges>
#include <deque>
#include <functional>

// https://adventofcode.com/2024/day/9

namespace {

// A block, either empty or with a file ID number
struct Block {
  int data; // -1 = empty
  size_t size;

  static Block EmptyBlock(size_t size) {
    return { -1, size };
  }

  static Block FileBlock(int id, size_t size) {
    return { id, size };
  }

  bool is_file() const {
    return data != -1;
  }
};

// Convert a disk into its checksum
struct ChecksumComputer {
  long long int current_checksum = 0;
  size_t current_i = 0;

  void add_block(const Block & block) {
    if (block.is_file()) {
      for (size_t i = 0; i != block.size; ++i) {
        current_checksum += block.data * current_i;
        ++current_i;
      }
    } else {
      current_i += block.size;
    }
  }

  static long long int get_checksum(std::span<const Block> disk) {
    ChecksumComputer checksum;
    for (const Block & block : disk) {
      checksum.add_block(block);
    }
    return checksum.current_checksum;
  }
};

// Convert the input into a list of blocks
std::vector<Block> read_input(const std::string & line) {
  std::vector<Block> retval;
  for (size_t i = 0; i != line.size(); ++i) {
    size_t size = line[i] - '0';

    if (i % 2 == 1) {
      retval.emplace_back(Block::EmptyBlock(size));
    } else {
      retval.emplace_back(Block::FileBlock(i / 2, size));
    }
  }
  return retval;
}

// Defrag according to part a (allow segmented files)
std::vector<Block> defrag_part_a(std::span<const Block> original_disk_span) {
  std::deque remaining_blocks(original_disk_span.begin(), original_disk_span.end());
  if (remaining_blocks.empty()) return {};

  std::vector<Block> output;

  while (true) {
    if (remaining_blocks.empty()) return output;

    Block todo = remaining_blocks.front();
    remaining_blocks.pop_front();

    if (todo.is_file()) {
      // Next block is a file: don't touch it
      output.emplace_back(todo);
      continue;
    }

    // We have bytes to fill
    size_t bytes_to_fill = todo.size;

    while (bytes_to_fill > 0 && !remaining_blocks.empty()) {
      // Take last block of remaining blocks
      Block & end_block = remaining_blocks.back();

      if (!end_block.is_file()) {
        // Empty block -> discard
        remaining_blocks.pop_back();
        continue;
      }

      // A file
      if (end_block.size > bytes_to_fill) {
        // Big enough to fill the block: take the required bytes to fill the blank
        output.push_back(Block::FileBlock(end_block.data, bytes_to_fill));
        end_block.size -= bytes_to_fill;
        bytes_to_fill = 0;
      } else {
        // Too small (or same size): take the whole block
        bytes_to_fill -= end_block.size;
        output.push_back(end_block);
        remaining_blocks.pop_back();
      }
    }
  }
}

// Defrag according to part b (don't allow segmented files)
std::vector<Block> defrag_part_b(std::span<const Block> original_disk) {
  // "It would be easier to have the adress of each file only"
  struct DiskState {
    struct FileInfo {
      bool moved = false;
      int data;
      size_t size;

      FileInfo(Block block) : data(block.data), size(block.size) {}
    };

    // Note: we are using std::map basically as a sorted std::list
    // Map is reverted so we can erase moved blocks and go to the next block to consider
    // (Last file is moved first)
    std::map<size_t /* address */, FileInfo, std::greater<>> files;

    explicit DiskState(std::span<const Block> original_disk) {
      size_t curr_address = 0;
      for (const Block & block : original_disk) {
        if (block.is_file()) {
          files.emplace(curr_address, block);
        }
        curr_address += block.size;
      }
    }

    [[nodiscard]] std::vector<Block> to_vector() const {
      std::vector<Block> retval;
      
      size_t expected_next = 0;

      for (const auto & [address, file_info] : files | std::views::reverse) {
        if (address != expected_next) {
          retval.emplace_back(Block::EmptyBlock(address - expected_next));
        }

        retval.emplace_back(Block::FileBlock(file_info.data, file_info.size));
        expected_next = address + file_info.size;
      }

      return retval;
    }


    // Return the smallest address to which the given file could be moved.
    [[nodiscard]] std::optional<size_t> find_new_address(FileInfo fileInfo) const {
      if (fileInfo.moved) return std::nullopt;

      auto rev_it = files.rbegin();
      auto rev_it_p1 = files.rbegin(); ++rev_it_p1;

      while (rev_it != files.rend() && rev_it_p1 != files.rend() && rev_it->second.data != fileInfo.data) {
        const size_t start_of_free_space = rev_it->first + rev_it->second.size;
        const size_t size_of_free_space = rev_it_p1->first - start_of_free_space;

        if (size_of_free_space >= fileInfo.size) {
          return start_of_free_space;
        }

        ++rev_it;
        ++rev_it_p1;
      }

      return std::nullopt;
    }

    // Defrag the disk = try to move each block to the smallest possible address, starting from the end
    DiskState & defrag() {
      auto it_block = files.begin(); // Addresses are sorted from greatest to smallest
      while (it_block != files.end()) {
        std::optional<size_t> new_address = find_new_address(it_block->second);

        if (new_address) {
          it_block->second.moved = true;
          files.emplace(*new_address, it_block->second);
          it_block = files.erase(it_block);
        } else {
          ++it_block;
        }
      }

      return *this;
    }
  };

  // Improvment idea: use the address of empty blocks? (but then, there is the question of merging empty blocks)

  return DiskState(original_disk).defrag().to_vector();
}

[[maybe_unused]] void print(const std::span<const Block> & blocks) {
  for (const Block & block : blocks) {
    for (size_t i = 0; i != block.size; ++i) {
      if (block.data == -1)
        printf(".");
      else
        printf("%d", block.data);
    }
  }
  printf("\n");
}

}

Output day_2024_09(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::vector<Block> disk = read_input(lines[0]);
  const std::vector<Block> defragged_part_a = defrag_part_a(disk);
  const std::vector<Block> defragged_part_b = defrag_part_b(disk);

  return Output(
    ChecksumComputer::get_checksum(defragged_part_a),
    ChecksumComputer::get_checksum(defragged_part_b)
    ); 
}
