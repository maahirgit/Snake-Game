#include <iostream>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <string>
#include <fstream>
#include <chrono>

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
    
    virtual void spawn(int width, int height, const deque<Position>& snake1, const deque<Position>& snake2) {   // virtual allow child class to override it
        bool validPosition;
        do {
            validPosition = true;
            pos.x = rand() % (width - 2) + 1;   //means food should be inside border
            pos.y = rand() % (height - 2) + 1;
            
            for (const auto& segment : snake1) {
                if (pos == segment) {
                    validPosition = false;
                    break;
                }
            }  // check that snake body and food position are not same
            if (!validPosition) continue;
            for (const auto& segment : snake2) {
                if (pos == segment) {
                    validPosition = false;
                    break;
                }
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
    Snake(int startX, int startY, char startDir = 'R') : direction(startDir), growing(false) {
        if (startDir == 'L') {
            body.push_back(Position(startX, startY));
            body.push_back(Position(startX + 1, startY));
            body.push_back(Position(startX + 2, startY));
        } else {
            body.push_back(Position(startX, startY));
            body.push_back(Position(startX - 1, startY));
            body.push_back(Position(startX - 2, startY));
        }
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
    
    bool checkSnakeCollision(const Snake& other) const {
        Position head = body.front();
        for (const auto& segment : other.getBody()) {
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
    int width, height;
    int score1, score2;
    int highScore;
    Snake* snake1;
    Snake* snake2;
    Food* currentFood;
    bool gameOver;
    string loserPlayer;
    int level;
    int foodEaten;
    bool useSpecialFood;
    Position oldTail1, oldTail2;
    Position oldFoodPos;
    bool firstDraw;
    steady_clock::time_point lastMoveTime;
    
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
        int maxScore = max(score1, score2);
        if (maxScore > highScore) {
            highScore = maxScore;
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
    GameBoard(int w, int h) : width(w), height(h), score1(0), score2(0), gameOver(false), 
                               loserPlayer(""), level(1), foodEaten(0), useSpecialFood(false), 
                               firstDraw(true), oldFoodPos(-1, -1) {
        loadHighScore();
        snake1 = new Snake(w / 4, h / 2, 'R');
        snake2 = new Snake(3 * w / 4, h / 2, 'L');
        currentFood = new Food();
        currentFood->spawn(width, height, snake1->getBody(), snake2->getBody());
        oldTail1 = snake1->getTail();
        oldTail2 = snake2->getTail();
        lastMoveTime = steady_clock::now();
    }
    
    ~GameBoard() {
        delete snake1;
        delete snake2;
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
            
            // Draw initial snake 1 (green)
            const deque<Position>& body1 = snake1->getBody();
            for (size_t i = 0; i < body1.size(); i++) {
                drawCell(body1[i].x, body1[i].y + 2, (char)254, 10);
            }
            
            // Draw initial snake 2 (cyan)
            const deque<Position>& body2 = snake2->getBody();
            for (size_t i = 0; i < body2.size(); i++) {
                drawCell(body2[i].x, body2[i].y + 2, (char)254, 11);
            }
            
            // Draw initial food
            drawCell(currentFood->getPosition().x, currentFood->getPosition().y + 2, 
                     currentFood->getSymbol(), useSpecialFood ? 13 : 12);
            
            oldFoodPos = currentFood->getPosition();
            firstDraw = false;
        } else {
            // Erase old tails
            if (!(oldTail1 == snake1->getTail())) {
                drawCell(oldTail1.x, oldTail1.y + 2, ' ', 8);
            }
            if (!(oldTail2 == snake2->getTail())) {
                drawCell(oldTail2.x, oldTail2.y + 2, ' ', 8);
            }
            
            // Erase old food position if it changed
            if (!(oldFoodPos == currentFood->getPosition())) {
                drawCell(oldFoodPos.x, oldFoodPos.y + 2, ' ', 8);
            }
            
            // Draw new heads
            Position head1 = snake1->getHead();
            drawCell(head1.x, head1.y + 2, (char)254, 10);
            
            Position head2 = snake2->getHead();
            drawCell(head2.x, head2.y + 2, (char)254, 11);
            
            // Draw food
            drawCell(currentFood->getPosition().x, currentFood->getPosition().y + 2, 
                     currentFood->getSymbol(), useSpecialFood ? 13 : 12);
            
            oldFoodPos = currentFood->getPosition();
        }
        
        // Update stats
        updateStats();
        drawTopStats();
        
        oldTail1 = snake1->getTail();
        oldTail2 = snake2->getTail();
    }
    
    void drawTopStats() {
        setCursorPosition(2, 0);
        setColor(15);
        cout << "P1 SCORE: ";
        setColor(14);
        cout << score1 << " ";
        
        setColor(15);
        cout << "| P2 SCORE: ";
        setColor(14);
        cout << score2 << " ";
        
        setColor(15);
        cout << "| HIGH: ";
        setColor(14);
        cout << highScore << " ";
        
        setColor(15);
        cout << "| LEVEL: ";
        setColor(14);
        cout << level << "   ";
        
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
            if (key == -32 || key == 0) { // Arrow keys for P1
                key = _getch();
                switch (key) {
                    case 72: snake1->setDirection('U'); break; // Up
                    case 80: snake1->setDirection('D'); break; // Down
                    case 75: snake1->setDirection('L'); break; // Left
                    case 77: snake1->setDirection('R'); break; // Right
                }
            } else {
                switch (tolower(key)) {
                    case 'w': snake2->setDirection('U'); break; // P2 Up
                    case 's': snake2->setDirection('D'); break; // P2 Down
                    case 'a': snake2->setDirection('L'); break; // P2 Left
                    case 'd': snake2->setDirection('R'); break; // P2 Right
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
    
    void update() {
        if (!shouldMove()) {
            return;
        }
        
        snake1->move();
        snake2->move();
        
        Position head1 = snake1->getHead();
        Position head2 = snake2->getHead();
        
        bool p1Lost = false;
        bool p2Lost = false;
        
        // Check wall collision
        if (head1.x <= 0 || head1.x >= width - 1 || head1.y <= 0 || head1.y >= height - 1) {
            p1Lost = true;
        }
        if (head2.x <= 0 || head2.x >= width - 1 || head2.y <= 0 || head2.y >= height - 1) {
            p2Lost = true;
        }
        
        // Check self collision
        if (snake1->checkSelfCollision()) {
            p1Lost = true;
        }
        if (snake2->checkSelfCollision()) {
            p2Lost = true;
        }
        
        // Check collision between snakes
        if (snake1->checkSnakeCollision(*snake2)) {
            p1Lost = true;
        }
        if (snake2->checkSnakeCollision(*snake1)) {
            p2Lost = true;
        }
        
        if (p1Lost || p2Lost) {
            gameOver = true;
            if (p1Lost && p2Lost) {
                loserPlayer = "Player 1 & Player 2";
            } else if (p1Lost) {
                loserPlayer = "Player 1";
            } else {
                loserPlayer = "Player 2";
            }
            return;
        }
        
        // Check food collision
        bool eaten = false;
        if (head1 == currentFood->getPosition()) {
            snake1->grow();
            score1 += currentFood->getPoints();
            eaten = true;
        } else if (head2 == currentFood->getPosition()) {
            snake2->grow();
            score2 += currentFood->getPoints();
            eaten = true;
        }
        
        if (eaten) {
            foodEaten++;
            if (foodEaten % 5 == 0) {
                level++;
            }
            delete currentFood;
            useSpecialFood = (rand() % 5 == 0);
            currentFood = useSpecialFood ? new SpecialFood() : new Food();
            currentFood->spawn(width, height, snake1->getBody(), snake2->getBody());
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
        char dir = snake1->getDirection();
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
        string msg = loserPlayer + " Lost!";
        int leftPad = (50 - (int)msg.length()) / 2;
        int rightPad = 50 - (int)msg.length() - leftPad;
        cout << "  " << (char)186 << string(leftPad, ' ') << msg << string(rightPad, ' ') << (char)186 << "\n";
        cout << "  " << (char)200;
        for(int i=0; i<50; i++) cout << (char)205;
        cout << (char)188 << "\n\n";
        
        setColor(15);
        cout << "  " << (char)218;
        for(int i=0; i<50; i++) cout << (char)196;
        cout << (char)191 << "\n";
        
        cout << "  " << (char)179 << "  GAME STATISTICS:                               " << (char)179 << "\n";
        cout << "  " << (char)179 << "                                                  " << (char)179 << "\n";
        
        setColor(15);
        cout << "  " << (char)179 << "    P1 Score: ";
        setColor(14);
        cout << score1;
        setColor(15);
        cout << " | P2 Score: ";
        setColor(14);
        cout << score2;
        int lenScoreStr = 14 + to_string(score1).length() + 14 + to_string(score2).length();
        for(int i = lenScoreStr; i < 50; i++) cout << " ";
        setColor(15);
        cout << (char)179 << "\n";
        
        setColor(15);
        cout << "  " << (char)179 << "    P1 Len: ";
        setColor(10);
        cout << snake1->getLength();
        setColor(15);
        cout << " | P2 Len: ";
        setColor(10);
        cout << snake2->getLength();
        int lenLenStr = 12 + to_string(snake1->getLength()).length() + 12 + to_string(snake2->getLength()).length();
        for(int i = lenLenStr; i < 50; i++) cout << " ";
        setColor(15);
        cout << (char)179 << "\n";
        
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
        
        int maxScore = max(score1, score2);
        if (maxScore == highScore && maxScore > 0) {
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
    
    int getScore() const { return max(score1, score2); }
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