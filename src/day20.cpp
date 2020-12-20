#include "libs.hpp"

#include <algorithm>
#include <vector>
#include <variant>
#include <memory>
#include <set>
#include <map>
#include <regex>
#include <ostream>

// https://adventofcode.com/2020/day/20

using Raw = std::vector<std::vector<char>>;

struct Transformations {
    bool is_flipped_left_right = false;
    bool is_flipped_bottom_up = false;
    int is_rotate_right = 0;

    friend std::ostream & operator<<(std::ostream & stream, const Transformations & trans) {
        stream << "FlippedRL=" << trans.is_flipped_left_right
            << " ; FlippedTB=" << trans.is_flipped_bottom_up
            << " ; rotate= " << trans.is_rotate_right;
        return stream;
    }
};

struct Path;

struct Tile {
    int id;
    std::vector<std::vector<char>> image;

    explicit Tile(int id) : id(id) {}

    [[nodiscard]] const std::vector<char> & operator[](size_t x) const { return image[x]; }

    friend std::ostream & operator<<(std::ostream & stream, const Tile & tile) {
        stream << "== Tile " << tile.id << "\n";

        for (const auto & line : tile.image) {
            for (const auto pixel : line) {
                stream << pixel;
            }

            stream << '\n';
        }

        stream << '\n';
        return stream;
    }

    [[nodiscard]] std::vector<Path> adjacent(const Tile & other_tile) const;
};

struct Path {
    int my_rotation;
    Transformations his_transformations;

    friend std::ostream & operator<<(std::ostream & stream, const Path & path) {
        stream << "left= " << path.my_rotation << " ; " << path.his_transformations;
        return stream;
    }
};

struct AltereredTile {
    Tile representation;
    Transformations transformations;

    AltereredTile(Tile tile) : representation(std::move(tile)) {}

    // ok
    void flip_left_right() {
        transformations.is_flipped_left_right = !transformations.is_flipped_left_right;

        for (auto & line : representation.image) {
            std::vector<char> new_line { line.rbegin(), line.rend() };
            line = std::move(new_line);
        }
    }

    // ok
    void flip_bottom_up() {
        transformations.is_flipped_bottom_up = !transformations.is_flipped_bottom_up;

        std::vector<std::vector<char>> image { representation.image.rbegin(), representation.image.rend() };
        representation.image = std::move(image);
    }

    // ok
    void rotate_right() {
        transformations.is_rotate_right = (transformations.is_rotate_right + 1) % 4;

        std::vector<std::vector<char>> image;

        for (size_t y = 0 ; y != representation.image.size() ; ++y) {
            image.push_back({});
            for (size_t x = 0 ; x != representation.image[y].size() ; ++x) {
                image.back().push_back(representation[representation.image.size() - x - 1][y]);
            }
        }

        representation.image = std::move(image);
    }

    friend std::ostream & operator<<(std::ostream & stream, const AltereredTile & tile) {
        stream << tile.representation << tile.transformations << "\n\n";
        return stream;
    }
};


static bool connect(const std::vector<std::vector<char>> & lhs, const std::vector<std::vector<char>> & rhs) {
    for (size_t i = 0 ; i != lhs.size() ; ++i) {
        if (lhs[i].back() != rhs[i].front()) {
            return false;
        }
    }

    return true;
}

std::vector<Path> Tile::adjacent(const Tile & other_tile) const {
    std::vector<Path> paths;

    AltereredTile me { *this };

    std::set<Raw> my_raws;
    std::set<Raw> other_raws;

    for (size_t i = 0 ; i != 4 ; ++i, me.rotate_right()) {
        AltereredTile other { other_tile };

        if (my_raws.contains(me.representation.image)) {
            continue;
        }

        my_raws.insert(me.representation.image);

        for (size_t j = 0 ; j != 2 ; ++j) {
            other.flip_bottom_up();

            for (size_t jj = 0 ; jj != 2 ; ++jj) {
                other.flip_left_right();

                for (size_t jjj = 0 ; jjj != 4 ; ++jjj) {
                    other.rotate_right();

                    if (!other_raws.contains(other.representation.image)) {
                        other_raws.insert(other.representation.image);

                        if (connect(me.representation.image, other.representation.image)) {
                            paths.push_back(Path { me.transformations.is_rotate_right, other.transformations });
                        }
                    }
                }
            }
        }
    }

    return paths;
}


std::vector<Tile> to_tiles(const std::vector<std::string> & lines) {
    std::vector<Tile> retval;

    for (const auto & line : lines) {
        if (line == "") continue;

        if (line.rfind("Tile ", 0) == 0) {
            std::string id { line.begin() + 5, line.end() - 1 };
            retval.emplace_back(std::stoi(id));
        } else {
            retval.back().image.emplace_back();

            for (const auto c : line) {
                retval.back().image.back().emplace_back(c);
            }
        }
    }

    return retval;
}



Output day20(const std::vector<std::string> & lines, const DayExtraInfo &) {
    std::vector<Tile> tiles = to_tiles(lines);

    std::cout << tiles[0];
    std::cout << tiles[1];


    const auto paths = tiles[0].adjacent(tiles[1]);

    for (const auto & path : paths) {
        std::cout << path << "\n";
    }

    
    return Output(0, 0);
}
