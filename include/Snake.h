#pragma once
#include <SFML/Graphics.hpp>

class Snake {
public:
    Snake(int cellSize, sf::Vector2u windowSize);

    void updateColor(float delta);
    void draw(sf::RenderWindow& window);
    sf::CircleShape getHead() const;

    void setCenter(sf::Vector2u windowSize); // NEW

private:
    int cellSize;
    sf::CircleShape head;
    sf::Color colors[3];
    int currentColor, nextColor;
    float elapsed, transitionTime;
};
