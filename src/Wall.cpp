#include "Wall.h"

Wall::Wall(int cellSize) : cellSize(cellSize) {}

void Wall::update(unsigned int width, unsigned int height) {
    unsigned int cols = width / cellSize;
    unsigned int rows = height / cellSize;

    walls.clear();

    for (unsigned int y = 0; y < rows; y++) {
        for (unsigned int x = 0; x < cols; x++) {
            if (x == 0 || y == 0 || x == cols - 1 || y == rows - 1) {
                sf::ConvexShape triangle;
                triangle.setPointCount(3);

                float px = x * cellSize;
                float py = y * cellSize;

                if (y == 0) {
                    // Top border → triangle points down
                    triangle.setPoint(0, sf::Vector2f(px, py));
                    triangle.setPoint(1, sf::Vector2f(px + cellSize, py));
                    triangle.setPoint(2, sf::Vector2f(px + cellSize / 2.f, py + cellSize));
                } 
                else if (y == rows - 1) {
                    // Bottom border → triangle points up
                    triangle.setPoint(0, sf::Vector2f(px, py + cellSize));
                    triangle.setPoint(1, sf::Vector2f(px + cellSize, py + cellSize));
                    triangle.setPoint(2, sf::Vector2f(px + cellSize / 2.f, py));
                } 
                else if (x == 0) {
                    // Left border → triangle points right
                    triangle.setPoint(0, sf::Vector2f(px, py));
                    triangle.setPoint(1, sf::Vector2f(px, py + cellSize));
                    triangle.setPoint(2, sf::Vector2f(px + cellSize, py + cellSize / 2.f));
                } 
                else if (x == cols - 1) {
                    // Right border → triangle points left
                    triangle.setPoint(0, sf::Vector2f(px + cellSize, py));
                    triangle.setPoint(1, sf::Vector2f(px + cellSize, py + cellSize));
                    triangle.setPoint(2, sf::Vector2f(px, py + cellSize / 2.f));
                }

                triangle.setFillColor(sf::Color(200, 40, 40)); // red
                triangle.setOutlineThickness(1);
                triangle.setOutlineColor(sf::Color(100, 0, 0));

                walls.push_back(triangle);
            }
        }
    }
}

void Wall::draw(sf::RenderWindow& window) {
    for (auto& w : walls) {
        window.draw(w);
    }
}

const std::vector<sf::ConvexShape>& Wall::getWalls() const {
    return walls;
}
