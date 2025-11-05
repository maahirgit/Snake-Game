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


enum Direction
{
    STOP = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class GameBoard
{
public:
    // PDF Requirement: NxN grid (internal cells)
    int N;
    GameBoard(int n = 20) : N(n) {}
    int width() const { return N + 2; }  // include left/right borders
    int height() const { return N + 2; } // include top/bottom borders
};

class Snake
{
public:
    int x, y;
    
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator==(const Position& other) const {  //bool is used to check self collision or food does not appear on snake body
        return x == other.x && y == other.y;  //check and compare two positions are equal or not
    }
};

class Food
{
public:
    int x, y;

    // PDF Requirement: "Unfair Food Placement: Ensure food does not spawn on the snake's body."
    void spawn(int N, const deque<pair<int, int>> &snakeBody)
    {
        bool valid = false;
        while (!valid)
        {
            x = rand() % N + 1; // 1..N
            y = rand() % N + 1;
            valid = true;
            for (auto &s : snakeBody)
            {
                if (s.first == x && s.second == y)
                {
                    valid = false; // re-spawn if generated on snake
                    break;
                }
            }
        }
    }
};
