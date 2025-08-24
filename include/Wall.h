#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Wall {
public:
    Wall(int cellSize);

    void update(unsigned int width, unsigned int height);
    void draw(sf::RenderWindow& window);
    const std::vector<sf::ConvexShape>& getWalls() const;

private:
    int cellSize;
    std::vector<sf::ConvexShape> walls;
};
