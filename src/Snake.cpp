#include <iostream>
#include <conio.h>
#include <windows.h>
#include <deque>
#include <vector>
using namespace std;


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
