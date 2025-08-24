#include "Snake.h"

Snake::Snake(int cellSize, sf::Vector2u windowSize)
    : cellSize(cellSize), currentColor(0), nextColor(1), elapsed(0), transitionTime(2.0f)
{
    head.setRadius(cellSize / 2.5f);
    head.setOrigin(head.getRadius(), head.getRadius()); 
    setCenter(windowSize); // use function
    colors[0] = sf::Color(0, 200, 180);
    colors[1] = sf::Color(0, 180, 120);
    colors[2] = sf::Color(50, 220, 200);
}

void Snake::setCenter(sf::Vector2u windowSize) {
    int xCells = windowSize.x / cellSize;
    int yCells = windowSize.y / cellSize;

    int startX = (xCells / 2) * cellSize + cellSize / 2;
    int startY = (yCells / 2) * cellSize + cellSize / 2;

    head.setPosition(startX, startY);
}

sf::Color interpolateColor(const sf::Color& c1, const sf::Color& c2, float t) {
    return sf::Color(
        c1.r + (c2.r - c1.r) * t,
        c1.g + (c2.g - c1.g) * t,
        c1.b + (c2.b - c1.b) * t
    );
}

void Snake::updateColor(float delta) {
    elapsed += delta;
    float t = elapsed / transitionTime;
    if (t >= 1.0f) {
        t = 0.0f;
        elapsed = 0.0f;
        currentColor = nextColor;
        nextColor = (nextColor + 1) % 3;
    }
    head.setFillColor(interpolateColor(colors[currentColor], colors[nextColor], t));
}

void Snake::draw(sf::RenderWindow& window) {
    window.draw(head);
}

sf::CircleShape Snake::getHead() const {
    return head;
}
