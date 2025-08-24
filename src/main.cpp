#include <SFML/Graphics.hpp>
#include "Snake.h"
#include "Wall.h"
#include "Board.h"
#include "Banner.h"

int main() {
    const int cellSize = 30;      // Board + Snake size
    const int miniCellSize = 15;  // Banner letter size

    sf::RenderWindow window(sf::VideoMode(900, 600), "Snake Game");

    Banner banner("SNAKE GAME", miniCellSize, window.getSize());
    Snake snake(cellSize, window.getSize());
    Wall walls(cellSize);
    Board board(cellSize, window.getSize());

    sf::Clock clock;
    bool gameOver = false;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) 
                window.close();

            if (e.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, e.size.width, e.size.height);
                window.setView(sf::View(visibleArea));

                walls.update(e.size.width, e.size.height);
                board.updateGrid(e.size.width, e.size.height);
                snake.setCenter({e.size.width, e.size.height});
                banner.update(window.getSize());
            }
        }

        float delta = clock.restart().asSeconds();

        if (!gameOver) {
            snake.updateColor(delta);

            for (auto& w : walls.getWalls()) {
                if (snake.getHead().getGlobalBounds().intersects(w.getGlobalBounds())) {
                    gameOver = true;
                    break;
                }
            }
        }

        window.clear();

        // ✅ Draw navbar/banner
        banner.draw(window);

        // ✅ Shift game UI below banner
        sf::View gameView(sf::FloatRect(
            0, banner.getBannerHeight(),
            window.getSize().x, 
            window.getSize().y - banner.getBannerHeight()
        ));
        window.setView(gameView);

        board.draw(window);
        walls.draw(window);
        snake.draw(window);

        // ✅ Reset back to full view (for banner next frame)
        window.setView(sf::View(sf::FloatRect(
            0, 0, window.getSize().x, window.getSize().y
        )));

        window.display();
    }

    return 0;
}
