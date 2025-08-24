#pragma once
#include <SFML/Graphics.hpp>

class Board {
public:
    Board(int cellSize, sf::Vector2u windowSize);
    void draw(sf::RenderWindow& window);
    void updateGrid(unsigned int width, unsigned int height);
private:
    int cellSize;
    sf::VertexArray grid;
};
