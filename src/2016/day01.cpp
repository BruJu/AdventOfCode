#include "../advent_of_code.hpp"
#include <set>
#include <optional>

// https://adventofcode.com/2016/day/1

// Visting a city with a grid layout
// - Finding where I am when I follow instructions in the form
// ([Turn Left or Right] [Walk during X steps]) ...
// - Finding the first place that has been visited twice

namespace {
    /** Direction to turn to */
    enum class Direction { Left, Right };

    /** An instruction (R12 for example) */
    struct Instruction {
        Direction direction;
        int32_t steps;
    };

    /** Decodes the text into a list of instructions */
    auto string_to_instructions(const std::string & line) {
        std::vector<Instruction> instructions;

        Direction direction = Direction::Left;
        std::string buffer;

        for (size_t i = 0 ; i < line.size() ; ++i) {
            if (line[i] == 'L') {
                direction = Direction::Left;
                buffer = "";
            } else if (line[i] == 'R') {
                direction = Direction::Right;
                buffer = "";
            } else if (line[i] >= '0' && line[i] <= '9') {
                buffer += line[i];
            } else if (buffer != "") {
                instructions.push_back(Instruction{ direction, std::stoi(buffer.c_str()) });
                buffer = "";
            }
        }

        if (buffer != "") {
            instructions.push_back(Instruction{ direction, std::stoi(buffer.c_str()) });
        }

        return instructions;
    }

    /** A position */
    struct Position {
        int32_t x;
        int32_t y;

        /* For std::set */
        [[nodiscard]] bool operator<(const Position & other) const {
            if (x < other.x) return true;
            if (x > other.x) return false;
            if (y < other.y) return true;
            if (y > other.y) return false;
            return false;
        }

        [[nodiscard]] int32_t blocks_away_from_origin() const {
            int32_t sum = 0;
            if (x < 0) sum -= x; else sum += x;
            if (y < 0) sum -= y; else sum += y;
            return sum;
        }
    };

    /** Track the visited position to find the first visited twice (= hq) */
    struct PositionTracker {
        std::set<Position> visited;
        std::optional<Position> first_visited_twice;

        void operator()(Position position) {
            if (first_visited_twice) return;

            if (visited.contains(position)) {
                first_visited_twice = position;
            } else {
                visited.insert(position);
            }
        }
    };

    /** This is me. I'm somewhere and looking at something */
    struct Me {
        int8_t orientation = 0; // 0 = N, 1 = E, 2 = S, 3 = W
        Position position { 0, 0 };

        void travel(Instruction instruction, PositionTracker & tracker) {
            // Turn
            if (instruction.direction == Direction::Left) {
                if (orientation == 0) orientation = 3;
                else orientation -= 1;
            } else {
                if (orientation == 3) orientation = 0;
                else orientation += 1;
            }
            
            // Advance
            int32_t steps_left = instruction.steps;

            while (steps_left --> 0) { // Using the arrow operator for clarity
                if (orientation == 0) position.y += 1;
                if (orientation == 1) position.x += 1;
                if (orientation == 2) position.y -= 1;
                if (orientation == 3) position.x -= 1;
                tracker(position);
            }
        }
    };
}

Output day_2016_01(const std::vector<std::string> & lines, const DayExtraInfo &) {
    // Here I come in the city
    Me me;

    // Getting the instructions
    const std::string & line = lines[0];
    const auto instructions = string_to_instructions(line);

    // Turning on my GPS
    PositionTracker tracker;
    tracker(me.position);

    // Visiting the city
    for (const auto instruction : instructions) {
        me.travel(instruction, tracker);
    }

    // Looking back at my phone
    const auto hq = tracker.first_visited_twice;

    // Sending a message
    return Output(
        me.position.blocks_away_from_origin(),
        hq ? hq->blocks_away_from_origin() : 0
    );
}
