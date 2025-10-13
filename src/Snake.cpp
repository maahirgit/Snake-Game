#include<iostream>
#include<conio.h>
#include<bits/stdc++.h>
using namespace std;

struct SnakeSegment { int x, y; };

bool gameOver;
const int width = 20;
const int height = 17;
vector<SnakeSegment> snake; // snake body
int foodX, foodY;
int score;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;


bool gameOver;
const int width = 20;
const int height = 17;
int x,y;  //head coordinates of snake

enum eDirection { STOP = 0,LEFT,RIGHT,UP, DOWN};
eDirection dir;

void handleInput()
{
    if((_kbhit)){
        switch (_getch())
        {
        case 'a':
            dir = LEFT;
            break;
        
        case 'd':
            dir = RIGHT;
            break;
        
        case 'w':
            dir = UP;
            break;
        
        case 's':
            dir = DOWN;
            break;
        
        case 'x':
            exit(0);
        }
    }
}
void Setup(){
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
}

void Logic(){
    switch(dir){
        case LEFT: x--; break;
        case RIGHT: x++; break;
        case UP : y--; break;
        case DOWN : y++; break;
        default : break;
    }

    if(x>=width) x = 0;else if (x < 0) x = width - 1;
    if(y>=height) y = 0;else if (y < 0) y = height - 1;
}

int main(){
    // Setup();
    // while(!gameOver){
    //     Logic();
    //     cout << "Snake Position : (" << x << ", " << y << ")\n";
    //     system("pause>nul");
    // }
    cout << "Press W/A/S/D to move, X to quit. \n";
    while(true)
    {
        handleInput();
        switch (dir)
        {
        case LEFT:
            cout << "LEFT\n";
            break;
        
        case RIGHT:
            cout << "RIGHT\n";
            break;
        
        case UP:
            cout << "UP\n";
            break;
        
        case DOWN:
            cout << "DOWN\n";
            break;
        
        default:
            break;
        }
        system("pause>nul");
    }
    return 0;
}