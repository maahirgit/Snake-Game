#include <SFML/Graphics.hpp>
#include <vector>

int main()
{
    // Window size
    const int windowWidth = 800;
    const int windowHeight = 600;
    const int cellSize = 20;  // size of each grid cell

    // Create window
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Snake Game Board");

    // Snake head position (start in center)
    sf::RectangleShape snakeHead(sf::Vector2f(cellSize, cellSize));
    snakeHead.setFillColor(sf::Color::Green);
    int headX = windowWidth / 2 / cellSize;
    int headY = windowHeight / 2 / cellSize;
    snakeHead.setPosition(headX * cellSize, headY * cellSize);

    // Optional: grid lines
    sf::VertexArray gridLines(sf::Lines);
    for (int x = 0; x <= windowWidth; x += cellSize)
    {
        gridLines.append(sf::Vertex(sf::Vector2f(x, 0), sf::Color(50,50,50)));
        gridLines.append(sf::Vertex(sf::Vector2f(x, windowHeight), sf::Color(50,50,50)));
    }
    for (int y = 0; y <= windowHeight; y += cellSize)
    {
        gridLines.append(sf::Vertex(sf::Vector2f(0, y), sf::Color(50,50,50)));
        gridLines.append(sf::Vertex(sf::Vector2f(windowWidth, y), sf::Color(50,50,50)));
    }

    // Main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Draw everything
        window.clear(sf::Color::Black);
        window.draw(gridLines);
        window.draw(snakeHead);
        window.display();
    }

    return 0;
}
