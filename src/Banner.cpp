#include "Banner.h"

Banner::Banner(const std::string& text, int miniCellSize, sf::Vector2u windowSize)
    : text(text), miniCellSize(miniCellSize) {
    buildLetters(windowSize);
}

void Banner::buildLetters(sf::Vector2u windowSize) {
    letterShapes.clear();

    std::map<char, std::vector<std::string>> font = {
        {'S', {"#####","#....","#####","....#","#####"}},
        {'N', {"#...#","##..#","#.#.#","#..##","#...#"}},
        {'A', {"#####","#...#","#####","#...#","#...#"}},
        {'K', {"#...#","#..#.","###..","#..#.","#...#"}},
        {'E', {"#####","#....","#####","#....","#####"}},
        {'G', {"#####","#....","#..##","#...#","#####"}},
        {'M', {"#...#","##.##","#.#.#","#...#","#...#"}}
    };

    float totalWidth = text.size() * (miniCellSize * 6); // each letter + spacing
    float xOffset = (windowSize.x - totalWidth) / 2.f;   // ✅ center align
    float yOffset = miniCellSize; // padding from top

    for (char c : text) {
        if (c == ' ') {
            xOffset += miniCellSize * 6;
            continue;
        }

        auto pattern = font[c];
        for (size_t row = 0; row < pattern.size(); row++) {
            for (size_t col = 0; col < pattern[row].size(); col++) {
                if (pattern[row][col] == '#') {
                    sf::RectangleShape box(sf::Vector2f(miniCellSize - 2, miniCellSize - 2));
                    box.setFillColor(sf::Color(0, 200, 0));
                    box.setPosition(xOffset + col * miniCellSize, yOffset + row * miniCellSize);
                    letterShapes.push_back(box);
                }
            }
        }
        xOffset += miniCellSize * 6;
    }
}

void Banner::update(sf::Vector2u windowSize) {
    buildLetters(windowSize);
}

void Banner::draw(sf::RenderWindow& window) {
    for (auto& shape : letterShapes) {
        window.draw(shape);
    }
}
