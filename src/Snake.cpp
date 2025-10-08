#include<iostream>
using namespace std;

bool gameOver;
const int width = 20;
const int height = 17;
int x,y;  //head coordinates of snake

enum eDirection { STOP = 0,LEFT,Right,UP, DOWN};

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
    Setup();
    while(!gameOver){
        Logic();
        cout << "Snake Position : (" << x << ", " << y << ")\n";
        system("pause>nul");
    }
    return 0;
}