#include "Board.h"

Board::Board(int cellSize_, sf::Vector2u windowSize)
    : cellSize(cellSize_)
{
    updateGrid(windowSize.x, windowSize.y);
}

void Board::updateGrid(unsigned int width, unsigned int height) {
    grid.clear();
    grid.setPrimitiveType(sf::Lines);

    // Ensure cells remain square → limit based on smaller dimension
    unsigned int cols = width / cellSize;
    unsigned int rows = height / cellSize;

    float actualWidth  = cols * cellSize;
    float actualHeight = rows * cellSize;

    // Vertical lines
    for (int x = 0; x <= (int)actualWidth; x += cellSize) {
        grid.append(sf::Vertex(sf::Vector2f(x, 0), sf::Color(80, 80, 80)));
        grid.append(sf::Vertex(sf::Vector2f(x, actualHeight), sf::Color(80, 80, 80)));
    }

    // Horizontal lines
    for (int y = 0; y <= (int)actualHeight; y += cellSize) {
        grid.append(sf::Vertex(sf::Vector2f(0, y), sf::Color(80, 80, 80)));
        grid.append(sf::Vertex(sf::Vector2f(actualWidth, y), sf::Color(80, 80, 80)));
    }
}

void Board::draw(sf::RenderWindow& window) {
    window.draw(grid);
}
