#include <iostream>
#include <conio.h>
#include <windows.h>
#include <deque>
#include <vector>
using namespace std;

Snake(int startX = 1, int startY = 1)
    {
        dir = STOP;
        body.clear();
        // PDF Requirement: Snake starts as a line with 3 cells
        body.push_back({startX, startY}); // head
        body.push_back({startX, startY + 1});
        body.push_back({startX, startY + 2}); // tail
    }