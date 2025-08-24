#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>

class Banner {
public:
    Banner(const std::string& text, int miniCellSize, sf::Vector2u windowSize);

    void buildLetters(sf::Vector2u windowSize);
    void update(sf::Vector2u windowSize);
    void draw(sf::RenderWindow& window);

    int getBannerHeight() const { return miniCellSize * 7; } // height of banner in pixels

private:
    std::string text;
    int miniCellSize;
    std::vector<sf::RectangleShape> letterShapes;
};
