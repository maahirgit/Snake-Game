#include <iostream>
#include <conio.h>
#include <windows.h>
#include <deque>
#include <vector>
using namespace std;


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

    deque<pair<int, int>> body; 

    Direction dir;
Snake(int startX = 1, int startY = 1)
    {
        dir = STOP;
        body.clear();
        body.push_back({startX, startY}); 
        body.push_back({startX, startY + 1});
        body.push_back({startX, startY + 2}); 
    }


    void reset(int startX, int startY)
    {
        dir = STOP;
        body.clear();
        body.push_back({startX, startY});
        body.push_back({startX, startY + 1});
        body.push_back({startX, startY + 2});
    }

    void changeDirection(Direction newDir)
    {
        if ((dir == LEFT && newDir == RIGHT) || (dir == RIGHT && newDir == LEFT) ||
            (dir == UP && newDir == DOWN) || (dir == DOWN && newDir == UP))
            return;
        dir = newDir;
    }
void move()
    {
        if (dir == STOP)
            return;

        pair<int, int> newHead = body.front();
        switch (dir)
        {
        case LEFT:
            newHead.first--;
            break;
        case RIGHT:
            newHead.first++;
            break;
        case UP:
            newHead.second--;
            break;
        case DOWN:
            newHead.second++;
            break;
        default:
            break;
        }
        body.push_front(newHead);
        body.pop_back();
    }

    void grow()
    {
        body.push_back(body.back());
    }

    bool occupies(int x, int y) const
    {
        for (const auto &p : body)
            if (p.first == x && p.second == y)
                return true;
        return false;
    }

    bool checkSelfCollision() const
    {
        if (body.size() < 4)
            return false;
        auto head = body.front();
        for (size_t i = 1; i < body.size(); ++i)
            if (body[i].first == head.first && body[i].second == head.second)
                return true;
        return false;
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