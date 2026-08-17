#include <iostream>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <string>
#include <fstream>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;



class Position {
public:
    int x, y;
    
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator==(const Position& other) const {  //bool is used to check self collision or food does not appear on snake body
        return x == other.x && y == other.y;  //check and compare two positions are equal or not
    }
};



class Food {
protected:
    Position pos;
    char symbol;
    int points;
    
public:
    Food(char sym = 'O', int pts = 10) : symbol(sym), points(pts) {}
    
    // Takes every body on the board, however many there are, so that food placement
    // does not depend on how many snakes exist.
    virtual void spawn(int width, int height, const vector<const deque<Position>*>& bodies) {   // virtual allow child class to override it
        bool validPosition;
        do {
            validPosition = true;
            pos.x = rand() % (width - 2) + 1;   //means food should be inside border
            pos.y = rand() % (height - 2) + 1;

            for (const auto* body : bodies) {
                for (const auto& segment : *body) {
                    if (pos == segment) {
                        validPosition = false;
                        break;
                    }
                }  // check that snake body and food position are not same
                if (!validPosition) break;
            }
        } while (!validPosition);
    }
    
    Position getPosition() const { return pos; }
    char getSymbol() const { return symbol; }
    virtual int getPoints() const { return points; }
    
    virtual ~Food() {}  //used to delete both food everytime after eating
};

// Derived class demonstrating inheritance and polymorphism
class SpecialFood : public Food {
public:
    SpecialFood() : Food('$', 25) {}
    
    int getPoints() const override {
        return points + rand() % 10; // Bonus points
    }
};



class Snake {
private:
    deque<Position> body;
    char direction;
    bool growing;
    
public:
    Snake(int startX, int startY) : direction('R'), growing(false) {
        body.push_back(Position(startX, startY));
        body.push_back(Position(startX - 1, startY));
        body.push_back(Position(startX - 2, startY));
    }
    
    void setDirection(char newDir) {
        // Prevent 180-degree turns
        if ((direction == 'U' && newDir == 'D') || 
            (direction == 'D' && newDir == 'U') ||
            (direction == 'L' && newDir == 'R') || 
            (direction == 'R' && newDir == 'L')) {
            return;
        }
        direction = newDir; //it does not go down if going up
    }
    
    char getDirection() const { return direction; }
    
    Position getHead() const { return body.front(); }
    
    const deque<Position>& getBody() const { return body; }
    
    Position getTail() const { return body.back(); }
    
    void move() {
        Position head = body.front();
        
        switch (direction) {
            case 'U': head.y--; break;
            case 'D': head.y++; break;
            case 'L': head.x--; break;
            case 'R': head.x++; break;
        }
        
        body.push_front(head);
        
        if (!growing) {
            body.pop_back();
        } else {
            growing = false;
        }
    }
    
    void grow() {
        growing = true;
    }
    
    bool checkSelfCollision() const {
        Position head = body.front();
        for (size_t i = 1; i < body.size(); i++) {
            if (head == body[i]) {
                return true;
            }
        }
        return false;
    }

    bool checkCollisionWith(const Snake& other) const {
        Position head = body.front();
        for (const auto& segment : other.body) {
            if (head == segment) {
                return true;
            }
        }
        return false;
    }
    
    int getLength() const { return body.size(); }
};


class GameBoard {
private:
    // The number of snakes on the board is written down here and nowhere else.
    // Every other member and method derives it from snakes.size().
    static const int SNAKE_COUNT = 2;

    int width, height;
    vector<int> scores;
    int highScore;
    vector<Snake*> snakes;
    Food* currentFood;
    bool gameOver;
    int loser;
    int level;
    int foodEaten;
    bool useSpecialFood;
    vector<Position> oldTails;
    Position oldFoodPos;
    bool firstDraw;
    steady_clock::time_point lastMoveTime;

    vector<const deque<Position>*> snakeBodies() const {
        vector<const deque<Position>*> bodies;
        for (size_t s = 0; s < snakes.size(); s++) {
            bodies.push_back(&snakes[s]->getBody());
        }
        return bodies;
    }

    int bestScore() const {
        int best = 0;
        for (size_t s = 0; s < scores.size(); s++) {
            best = max(best, scores[s]);
        }
        return best;
    }

    void loadHighScore() {
        ifstream file("highscore.txt");
        if (file.is_open()) {
            file >> highScore;
            file.close();
        } else {
            highScore = 0;
        }
    }
    
    void saveHighScore() {
        if (bestScore() > highScore) {
            highScore = bestScore();
            ofstream file("highscore.txt");
            if (file.is_open()) {
                file << highScore;
                file.close();
            }
        }
    }
    
    void setCursorPosition(int x, int y) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }   // makes the snake position absolute not like one below other
    
    void setColor(int color) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }
    
    void drawCell(int x, int y, char c, int color) {
        setCursorPosition(x, y);
        setColor(color);
        cout << c;
        cout.flush();
    }
    
public:
    GameBoard(int w, int h) : width(w), height(h), gameOver(false), loser(-1),
                               level(1), foodEaten(0), useSpecialFood(false),
                               firstDraw(true), oldFoodPos(-1, -1) {
        loadHighScore();
        for (int s = 0; s < SNAKE_COUNT; s++) {
            snakes.push_back(new Snake(w * (s + 1) / (SNAKE_COUNT + 1), h / 2));
            scores.push_back(0);
        }
        currentFood = new Food();
        currentFood->spawn(width, height, snakeBodies());
        for (size_t s = 0; s < snakes.size(); s++) {
            oldTails.push_back(snakes[s]->getTail());
        }
        lastMoveTime = steady_clock::now();
    }

    ~GameBoard() {
        for (size_t s = 0; s < snakes.size(); s++) {
            delete snakes[s];
        }
        delete currentFood;
    }
    
    void drawBorder() {
        setColor(11); // Cyan
        
        // Top border
        setCursorPosition(0, 2);
        cout << (char)201; // ╔
        for (int i = 0; i < width - 2; i++) cout << (char)205; // ═
        cout << (char)187; // ╗
        
        // Side borders
        for (int y = 3; y < height + 1; y++) {
            setCursorPosition(0, y);
            cout << (char)186; // ║
            setCursorPosition(width - 1, y);
            cout << (char)186; // ║
        }
        
        // Bottom border
        setCursorPosition(0, height + 1);
        cout << (char)200; // ╚
        for (int i = 0; i < width - 2; i++) cout << (char)205; // ═
        cout << (char)188; // ╝
        
        cout.flush();
    }
    
    void draw() {
        if (firstDraw) {
            system("cls");
            
            // Draw top stats bar
            drawTopStats();
            
            drawBorder();
            
            // Draw initial snakes - all segments as circles
            for (size_t s = 0; s < snakes.size(); s++) {
                const deque<Position>& body = snakes[s]->getBody();
                for (size_t i = 0; i < body.size(); i++) {
                    drawCell(body[i].x, body[i].y + 2, (char)254, 10); // Circle for all segments
                }
            }
            
            // Draw initial food
            drawCell(currentFood->getPosition().x, currentFood->getPosition().y + 2, 
                     currentFood->getSymbol(), useSpecialFood ? 13 : 12);
            
            oldFoodPos = currentFood->getPosition();
            firstDraw = false;
        } else {
            // Erase old tails (for snakes that didn't grow)
            for (size_t s = 0; s < snakes.size(); s++) {
                if (!(oldTails[s] == snakes[s]->getTail())) {
                    drawCell(oldTails[s].x, oldTails[s].y + 2, ' ', 8);
                }
            }
            
            // Erase old food position if it changed
            if (!(oldFoodPos == currentFood->getPosition())) {
                drawCell(oldFoodPos.x, oldFoodPos.y + 2, ' ', 8);
            }
            
            // Draw new heads as circles
            for (size_t s = 0; s < snakes.size(); s++) {
                Position head = snakes[s]->getHead();
                drawCell(head.x, head.y + 2, (char)254, 10);
            }
            
            // Draw food
            drawCell(currentFood->getPosition().x, currentFood->getPosition().y + 2, 
                     currentFood->getSymbol(), useSpecialFood ? 13 : 12);
            
            oldFoodPos = currentFood->getPosition();
        }
        
        // Update stats (only when they change)
        updateStats();
        drawTopStats();

        for (size_t s = 0; s < snakes.size(); s++) {
            oldTails[s] = snakes[s]->getTail();
        }
    }

    void drawTopStats() {
        setCursorPosition(2, 0);
        for (size_t s = 0; s < scores.size(); s++) {
            setColor(15);
            cout << "P" << (s + 1) << " SCORE: ";
            setColor(14);
            cout << scores[s] << "   ";
        }

        setColor(15);
        cout << "| HIGH: ";
        setColor(14);
        cout << highScore << "   ";
        
        setColor(15);
        cout << "| LEVEL: ";
        setColor(14);
        cout << level << "   ";
        
        setColor(15);
        cout << "| LENGTH: ";
        setColor(10);
        for (size_t s = 0; s < snakes.size(); s++) {
            cout << snakes[s]->getLength() << "   ";
        }

        cout.flush();
    }
    
    void updateStats() {
        setCursorPosition(2, height + 3);
        setColor(15);
        cout << "Speed: ";
        setColor(14);
        // Display lower number = faster speed (more intuitive)
        int displaySpeed = 200 - getSpeed();
        cout << displaySpeed << "   ";
        
        cout.flush();
    }
    
    void handleInput() {
        if (_kbhit()) { // returns true if key is pressed
            char key = _getch();
            if (key == -32) { // Arrow keys
                key = _getch();
                switch (key) {
                    case 72: snakes.front()->setDirection('U'); break; // Up
                    case 80: snakes.front()->setDirection('D'); break; // Down
                    case 75: snakes.front()->setDirection('L'); break; // Left
                    case 77: snakes.front()->setDirection('R'); break; // Right
                }
            } else {
                switch (tolower(key)) {
                    case 'w': snakes[1]->setDirection('U'); break;
                    case 's': snakes[1]->setDirection('D'); break;
                    case 'a': snakes[1]->setDirection('L'); break;
                    case 'd': snakes[1]->setDirection('R'); break;
                    case 'p': pause(); break;
                }
            }
        }
    }
    
    void pause() {
        setCursorPosition(width / 2 - 6, height / 2 + 2);
        setColor(15);
        cout << "** PAUSED **";
        cout.flush();
        
        while (tolower(_getch()) != 'p') {}
        
        // Clear pause message
        setCursorPosition(width / 2 - 6, height / 2 + 2);
        cout << "             ";
        cout.flush();
        
        // Reset timer after unpause
        lastMoveTime = steady_clock::now();
    }
    
    bool shouldMove() {
        auto currentTime = steady_clock::now();
        auto elapsed = duration_cast<milliseconds>(currentTime - lastMoveTime).count();
        
        if (elapsed >= getSpeedForDirection()) {
            lastMoveTime = currentTime;
            return true;
        }
        return false;
    }
    
    void endGame(size_t loserIndex) {
        gameOver = true;
        loser = (int)loserIndex;
    }

    void update() {
        if (!shouldMove()) {
            return;
        }
        
        for (size_t s = 0; s < snakes.size(); s++) {
            snakes[s]->move();
        }

        // Check wall collision
        for (size_t s = 0; s < snakes.size(); s++) {
            Position head = snakes[s]->getHead();
            if (head.x <= 0 || head.x >= width - 1 ||
                head.y <= 0 || head.y >= height - 1) {
                endGame(s);
                return;
            }
        }

        // Check self collision
        for (size_t s = 0; s < snakes.size(); s++) {
            if (snakes[s]->checkSelfCollision()) {
                endGame(s);
                return;
            }
        }

        // Check collision with another snake
        for (size_t s = 0; s < snakes.size(); s++) {
            for (size_t o = 0; o < snakes.size(); o++) {
                if (o != s && snakes[s]->checkCollisionWith(*snakes[o])) {
                    endGame(s);
                    return;
                }
            }
        }

        // Check food collision
        for (size_t s = 0; s < snakes.size(); s++) {
            if (snakes[s]->getHead() == currentFood->getPosition()) {
                snakes[s]->grow();
                scores[s] += currentFood->getPoints();
                foodEaten++;

                // Level up every 5 foods
                if (foodEaten % 5 == 0) {
                    level++;
                }

                // Spawn special food occasionally
                delete currentFood;
                useSpecialFood = (rand() % 5 == 0);
                currentFood = useSpecialFood ? new SpecialFood() : new Food();
                currentFood->spawn(width, height, snakeBodies());
                break;
            }
        }
    }
    
    bool isGameOver() const { return gameOver; }
    
    int getSpeed() const {
        // Lower delay = faster speed
        // Start at 150ms, decrease by 10ms per level, minimum 50ms
        return max(50, 150 - (level - 1) * 10);
    }
    
    int getSpeedForDirection() const {
        int baseSpeed = getSpeed();
        // The tick delay has only ever depended on one heading; keep it on the first snake.
        char dir = snakes.front()->getDirection();
        // Vertical movement is slower (higher delay value)
        if (dir == 'U' || dir == 'D') {
            return baseSpeed + 30; // Add 30ms delay for vertical movement
        }
        return baseSpeed;
    }
    
    bool displayGameOver() {
        // Save high score before displaying game over screen
        saveHighScore();
        
        system("cls");
        
        setColor(12); // Red
        cout << "\n\n";
        cout << "  " << (char)201;
        for(int i=0; i<50; i++) cout << (char)205;
        cout << (char)187 << "\n";
        cout << "  " << (char)186 << "                                                  " << (char)186 << "\n";
        cout << "  " << (char)186 << "               GAME OVER!                         " << (char)186 << "\n";
        cout << "  " << (char)186 << "               PLAYER " << (loser + 1) << " LOST!                     " << (char)186 << "\n";
        cout << "  " << (char)186 << "                                                  " << (char)186 << "\n";
        cout << "  " << (char)200;
        for(int i=0; i<50; i++) cout << (char)205;
        cout << (char)188 << "\n\n";
        
        setColor(15);
        cout << "  " << (char)218;
        for(int i=0; i<50; i++) cout << (char)196;
        cout << (char)191 << "\n";
        
        cout << "  " << (char)179 << "  GAME STATISTICS:                               " << (char)179 << "\n";
        cout << "  " << (char)179 << "                                                  " << (char)179 << "\n";
        
        for (size_t s = 0; s < scores.size(); s++) {
            setColor(15);
            cout << "  " << (char)179 << "    P" << (s + 1) << " Final Score: ";
            setColor(14);
            cout << scores[s];
            for(int i = to_string(scores[s]).length(); i < 28; i++) cout << " ";
            setColor(15);
            cout << (char)179 << "\n";
        }

        for (size_t s = 0; s < snakes.size(); s++) {
            setColor(15);
            cout << "  " << (char)179 << "    P" << (s + 1) << " Snake Length: ";
            setColor(10);
            cout << snakes[s]->getLength();
            for(int i = to_string(snakes[s]->getLength()).length(); i < 27; i++) cout << " ";
            setColor(15);
            cout << (char)179 << "\n";
        }
        
        setColor(15);
        cout << "  " << (char)179 << "    Level Reached: ";
        setColor(14);
        cout << level;
        for(int i = to_string(level).length(); i < 29; i++) cout << " ";
        setColor(15);
        cout << (char)179 << "\n";
        
        setColor(15);
        cout << "  " << (char)179 << "    Foods Eaten: ";
        setColor(12);
        cout << foodEaten;
        for(int i = to_string(foodEaten).length(); i < 31; i++) cout << " ";
        setColor(15);
        cout << (char)179 << "\n";
        
        cout << "  " << (char)179 << "                                                  " << (char)179 << "\n";
        
        if (bestScore() == highScore && bestScore() > 0) {
            setColor(13); // Magenta
            cout << "  " << (char)179 << "    " << (char)175 << " NEW HIGH SCORE! " << (char)174 << "                         " << (char)179 << "\n";
        } else {
            setColor(15);
            cout << "  " << (char)179 << "    High Score: ";
            setColor(14);
            cout << highScore;
            for(int i = to_string(highScore).length(); i < 30; i++) cout << " ";
            setColor(15);
            cout << (char)179 << "\n";
        }
        
        cout << "  " << (char)179 << "                                                  " << (char)179 << "\n";
        cout << "  " << (char)192;
        for(int i=0; i<50; i++) cout << (char)196;
        cout << (char)217 << "\n\n";
        
        // Display options
        setColor(11); // Cyan
        cout << "  " << (char)201;
        for(int i=0; i<50; i++) cout << (char)205;
        cout << (char)187 << "\n";
        cout << "  " << (char)186 << "                                                  " << (char)186 << "\n";
        
        setColor(10);
        cout << "  " << (char)186 << "    Press 'R' to RESTART                          " << (char)186 << "\n";
        
        setColor(12);
        cout << "  " << (char)186 << "    Press 'Q' to QUIT                             " << (char)186 << "\n";
        
        setColor(11);
        cout << "  " << (char)186 << "                                                  " << (char)186 << "\n";
        cout << "  " << (char)200;
        for(int i=0; i<50; i++) cout << (char)205;
        cout << (char)188 << "\n\n";
        
        setColor(7);
        
        // Wait for user input
        while (true) {
            if (_kbhit()) {
                char choice = tolower(_getch());
                if (choice == 'r') {
                    return true; // Restart
                } else if (choice == 'q') {
                    return false; // Quit
                }
            }
            Sleep(50);
        }
    }
    
    int getScore() const { return bestScore(); }
    int getLevel() const { return level; }
    int getFoodEaten() const { return foodEaten; }
};



class Game {
private:
    GameBoard* board;
    int sessionHighScore;  // Track high score for the session
    
    void setColor(int color) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }
    
    void clearHighScoreFile() {
        ofstream file("highscore.txt");
        if (file.is_open()) {
            file << 0;
            file.close();
        }
    }
    
public:
    Game() : board(nullptr), sessionHighScore(0) {
        // Load initial high score at game start
        ifstream file("highscore.txt");
        if (file.is_open()) {
            file >> sessionHighScore;
            file.close();
        }
    }
    
    ~Game() {
        if (board) delete board;
    }
    
    void showMenu() {
        system("cls");
        
        setColor(11); // Cyan
        cout << "\n\n";
        cout << "  " << (char)201;
        for(int i=0; i<53; i++) cout << (char)205;
        cout << (char)187 << "\n";
        cout << "  " << (char)186 << "                                                     " << (char)186 << "\n";
        
        setColor(14); // Yellow
        cout << "  " << (char)186;
        setColor(10); // Green
        cout << "     " << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << "  ";
        cout << (char)219 << (char)219 << (char)219 << "   " << (char)219 << (char)219 << "  ";
        cout << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << "  ";
        cout << (char)219 << (char)219 << "  " << (char)219 << (char)219 << " ";
        cout << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << "     ";
        setColor(11);
        cout << (char)186 << "\n";
        
        cout << "  " << (char)186;
        setColor(10);
        cout << "    " << (char)219 << (char)219 << "       ";
        cout << (char)219 << (char)219 << (char)219 << (char)219 << "  " << (char)219 << (char)219 << " ";
        cout << (char)219 << (char)219 << "   " << (char)219 << (char)219 << " ";
        cout << (char)219 << (char)219 << " " << (char)219 << (char)219 << "  ";
        cout << (char)219 << (char)219 << "          ";
        setColor(11);
        cout << (char)186 << "\n";
        
        cout << "  " << (char)186;
        setColor(10);
        cout << "    " << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << "  ";
        cout << (char)219 << (char)219 << " " << (char)219 << (char)219 << " " << (char)219 << (char)219 << " ";
        cout << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << "  ";
        cout << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << "   ";
        cout << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << "       ";
        setColor(11);
        cout << (char)186 << "\n";
        
        cout << "  " << (char)186;
        setColor(10);
        cout << "         " << (char)219 << (char)219 << "  ";
        cout << (char)219 << (char)219 << "  " << (char)219 << (char)219 << (char)219 << (char)219 << " ";
        cout << (char)219 << (char)219 << "   " << (char)219 << (char)219 << " ";
        cout << (char)219 << (char)219 << " " << (char)219 << (char)219 << "  ";
        cout << (char)219 << (char)219 << "          ";
        setColor(11);
        cout << (char)186 << "\n";
        
        cout << "  " << (char)186;
        setColor(10);
        cout << "    " << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << "  ";
        cout << (char)219 << (char)219 << "   " << (char)219 << (char)219 << (char)219 << " ";
        cout << (char)219 << (char)219 << "   " << (char)219 << (char)219 << " ";
        cout << (char)219 << (char)219 << "   " << (char)219 << (char)219 << " ";
        cout << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << (char)219 << "     ";
        setColor(11);
        cout << (char)186 << "\n";
        
        cout << "  " << (char)186 << "                                                     " << (char)186 << "\n";
        setColor(14);
        cout << "  " << (char)186 << "                Game                                 " << (char)186 << "\n";
        setColor(11);
        cout << "  " << (char)186 << "                                                     " << (char)186 << "\n";
        cout << "  " << (char)200;
        for(int i=0; i<53; i++) cout << (char)205;
        cout << (char)188 << "\n\n";
        
        setColor(15);
        cout << "  " << (char)218;
        for(int i=0; i<53; i++) cout << (char)196;
        cout << (char)191 << "\n";
        cout << "  " << (char)179 << "  CONTROLS:                                          " << (char)179 << "\n";
        setColor(10);
        cout << "  " << (char)179 << "    > Arrow Keys or W/A/S/D - Move Snake             " << (char)179 << "\n";
        cout << "  " << (char)179 << "    > P - Pause/Resume                               " << (char)179 << "\n";
        setColor(15);
        cout << "  " << (char)179 << "                                                     " << (char)179 << "\n";
        cout << "  " << (char)179 << "  SCORING:                                           " << (char)179 << "\n";
        setColor(12);
        cout << "  " << (char)179 << "    > Normal Food (O) - 10 points                    " << (char)179 << "\n";
        setColor(13);
        cout << "  " << (char)179 << "    > Special Food ($) - 25+ bonus points            " << (char)179 << "\n";
        setColor(14);
        cout << "  " << (char)179 << "    > Level increases every 5 foods (faster speed!)  " << (char)179 << "\n";
        setColor(15);
        cout << "  " << (char)192;
        for(int i=0; i<53; i++) cout << (char)196;
        cout << (char)217 << "\n\n";
        
        setColor(11);
        cout << "  Press any key to start...\n";
        setColor(7);
        _getch();
    }
    
    void run() {
        bool playAgain = true;
        
        while (playAgain) {
            showMenu();
            
            system("cls");
            
            // Hide cursor
            CONSOLE_CURSOR_INFO cursorInfo;
            GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
            cursorInfo.bVisible = false;
            SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
            
            board = new GameBoard(50, 20);
            
            // Game loop with consistent timing
            while (!board->isGameOver()) {
                board->draw();
                board->handleInput();
                board->update();
                Sleep(10); // Small sleep to prevent CPU overuse
            }
            
            // Show cursor again
            cursorInfo.bVisible = true;
            SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
            
            // Display game over screen and get user choice
            playAgain = board->displayGameOver();
            
            delete board;
            board = nullptr;
        }
        
        // Clear high score when quitting
        clearHighScoreFile();
        
        // Exit message
        system("cls");
        setColor(11);
        cout << "\n\n";
        cout << "  " << (char)201;
        for(int i=0; i<40; i++) cout << (char)205;
        cout << (char)187 << "\n";
        cout << "  " << (char)186 << "                                        " << (char)186 << "\n";
        cout << "  " << (char)186 << "     Thanks for playing Snake!         " << (char)186 << "\n";
        cout << "  " << (char)186 << "     High score cleared.               " << (char)186 << "\n";
        cout << "  " << (char)186 << "                                        " << (char)186 << "\n";
        cout << "  " << (char)200;
        for(int i=0; i<40; i++) cout << (char)205;
        cout << (char)188 << "\n\n";
        setColor(7);
        Sleep(1500);
    }
};


int main() {
    srand(static_cast<unsigned>(time(0)));
    
    // Set console size and title
    system("mode con: cols=80 lines=30");
    system("title Snake Game - IT603 Final Project");
    
    Game game;
    game.run();
    
    return 0;
}