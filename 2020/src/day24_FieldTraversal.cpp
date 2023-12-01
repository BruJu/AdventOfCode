#include "../../common/advent_of_code.hpp"

#include <set>
#include <map>

// https://adventofcode.com/2020/day/24

enum class HexaDirection { e, se, sw, w, nw, ne };

static constexpr auto DIRECTIONS = {
    HexaDirection::e , HexaDirection::ne, HexaDirection::nw,
    HexaDirection::se, HexaDirection::sw, HexaDirection::w
};

static auto to_directions(std::string_view line) {
    std::vector<HexaDirection> retval;

    enum class LastSymbol { None, South, North };

    LastSymbol last = LastSymbol::None;

    for (char c : line) {
        if (c == 's') {
            last = LastSymbol::South;
        } else if (c == 'n') {
            last = LastSymbol::North;
        } else if (c == 'e') {
            if (last == LastSymbol::None)       retval.push_back(HexaDirection::e);
            else if (last == LastSymbol::South) retval.push_back(HexaDirection::se);
            else if (last == LastSymbol::North) retval.push_back(HexaDirection::ne);

            last = LastSymbol::None;
        } else if (c == 'w') {
            if (last == LastSymbol::None)       retval.push_back(HexaDirection::w);
            else if (last == LastSymbol::South) retval.push_back(HexaDirection::sw);
            else if (last == LastSymbol::North) retval.push_back(HexaDirection::nw);

            last = LastSymbol::None;
        }
    }

    return retval;
}

struct Position {
    int x = 0;
    int y = 0;

    Position() = default;

    explicit Position(const std::vector<HexaDirection> & direction_list) {
        for (auto dir : direction_list) {
            move(dir);
        }
    }

    [[nodiscard]] bool operator<(const Position & rhs) const {
        if (x < rhs.x) return true;
        if (x > rhs.x) return false;
        if (y < rhs.y) return true;
        if (y > rhs.y) return false;
        return false;
    }
    
    void move(HexaDirection direction) {
        switch (direction) {
            case HexaDirection::e:      x += 1;         break;
            case HexaDirection::se:             y += 1; break;
            case HexaDirection::sw:     x -= 1; y += 1; break;
            case HexaDirection::w:      x -= 1;         break;
            case HexaDirection::nw:             y -= 1; break;
            case HexaDirection::ne:     x += 1; y -= 1; break;
        }
    }

    template <typename Consumer>
    void for_each_adjacent(Consumer c) const {
        for (auto d : DIRECTIONS) {
            Position p  = *this;
            p.move(d);
            c(p);
        }
    }
};

static auto to_tiles(const std::vector<std::vector<HexaDirection>> & directionss) {
    std::set<Position> black_tiles;

    for (const auto & directions : directionss) {
        Position current { directions };

        if (black_tiles.contains(current)) {
            black_tiles.erase(current);
        } else {
            black_tiles.insert(current);
        }
    }

    return black_tiles;
}

static std::set<Position> game_of_life(std::set<Position> black) {
    std::map<Position, int> black_neighbours;

    for (const auto & position : black) {
        position.for_each_adjacent([&](Position p) {
            ++black_neighbours[p];
        });
    }

    std::set<Position> result;

    for (const auto & [tile, black_neighbour] : black_neighbours) {
        if (!black.contains(tile)) {
            if (black_neighbour == 2) result.insert(tile);
        } else {
            if (!(black_neighbour == 0 || black_neighbour > 2)) result.insert(tile);
        }
    }

    return result;
}

Output day_2020_24(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const auto directionss = lines_transform::map<std::vector<HexaDirection>>(lines, to_directions);

    auto black_tiles = to_tiles(directionss);
    const size_t init_black_tiles = black_tiles.size();

    for (size_t i = 0 ; i != 100 ; ++i) {
        black_tiles = game_of_life(black_tiles);
    }
    
    return Output(init_black_tiles, black_tiles.size());
}
