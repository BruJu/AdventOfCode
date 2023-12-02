#include "../../common/advent_of_code.hpp"
#include <vector>
#include <exception>
#include <span>
#include <regex>

// https://adventofcode.com/2018/day/10

struct Point {
  int x;
  int y;
  int dx;
  int dy;

  void advance() { x += dx; y += dy; }
  void rewind() { x -= dx; y -= dy; }
};

struct Rectangle {
  int left;
  int right;
  int top;
  int bottom;

  [[nodiscard]] std::int64_t area() const noexcept {
    return (static_cast<std::int64_t>(right) - left + 1) * (static_cast<std::int64_t>(bottom) - top + 1);
  }
};

class Galaxy {
private:
  std::vector<Point> m_stars;
  int time = 0;

public:
  static Galaxy from_input(std::span<const std::string> lines);
  void advance();
  void rewind();

  [[nodiscard]] std::int64_t get_galaxy_size() const { return get_bounding_rect().area(); }
  void draw(std::ostream & stream) const;

  [[nodiscard]] int get_time() const { return time; }

private:
  [[nodiscard]] Rectangle get_bounding_rect() const;

};

Galaxy Galaxy::from_input(const std::span<const std::string> lines) {
  Galaxy galaxy;

  std::regex regex { R"(^position=<\s*(-?\d*),\s*(-?\d*)> velocity=<\s*(-?\d*),\s*(-?\d*)>$)" };
  std::smatch smatch;

  for (const std::string & line : lines) {
    std::regex_search(line, smatch, regex);

    galaxy.m_stars.emplace_back(Point{
      std::stoi(smatch[1].str()),
      std::stoi(smatch[2].str()),
      std::stoi(smatch[3].str()),
      std::stoi(smatch[4].str()),
    });
  }

  return galaxy;
}


void Galaxy::advance() { for (Point & point : m_stars) { point.advance(); } ++time; }
void Galaxy::rewind()  { for (Point & point : m_stars) { point.rewind();  } --time; }

Rectangle Galaxy::get_bounding_rect() const {
  if (m_stars.empty()) return Rectangle { 0, 0, 0, 0 };

  Rectangle rectangle;
  rectangle.left = m_stars[0].x;
  rectangle.right = m_stars[0].x;
  rectangle.top = m_stars[0].y;
  rectangle.bottom = m_stars[0].y;

  for (const Point & point : m_stars) {
    rectangle.left = std::min(point.x, rectangle.left);
    rectangle.right = std::max(point.x, rectangle.right);
    rectangle.top = std::min(point.y, rectangle.top);
    rectangle.bottom = std::max(point.y, rectangle.bottom);
  }

  return rectangle;
}

void Galaxy::draw(std::ostream & stream) const {
  const Rectangle bounds = get_bounding_rect();

  std::cout << "** TIME= " << time << '\n';
  std::cout << '*';
  for (int x = bounds.left; x <= bounds.right; ++x) {
    std::cout << '-';
  }
  std::cout << '*' << '\n';

  for (int y = bounds.top; y <= bounds.bottom; ++y) {
    std::cout << '|';
    for (int x = bounds.left; x <= bounds.right; ++x) {
      const auto it = std::find_if(
        m_stars.begin(), m_stars.end(), 
        [&](const Point & point) { return point.x == x && point.y == y; }
      );
      const bool contained = it != m_stars.end();

      stream << (contained ? '#' : ' ');
    }

    stream << '|' << '\n';
  }

  std::cout << '*';
  for (int x = bounds.left; x <= bounds.right; ++x) {
    std::cout << '-';
  }
  std::cout << '*' << '\n';
}


Output day_2018_10(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  Galaxy galaxy = Galaxy::from_input(lines);

  std::int64_t last_size = galaxy.get_galaxy_size();

  while (true) {
    // galaxy.draw(std::cout);

    galaxy.advance();

    std::int64_t new_size = galaxy.get_galaxy_size();

    if (new_size <= last_size) {
      // ok
      last_size = new_size;
    } else {
      break;
    }
  }

  galaxy.rewind();
  galaxy.draw(std::cout);

  return Output("oui", galaxy.get_time());
}