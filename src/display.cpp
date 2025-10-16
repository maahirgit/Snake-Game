#include <iostream>
using namespace std;

const int width = 20;
const int height = 17;

void drawBoard(int x, int y) {
    system("cls"); // Clear console

    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) cout << "#";

            if (i == y && j == x)
                cout << "O";  // Snake head
            else
                cout << " ";

            if (j == width - 1) cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;
}

int main() {
    int x = 5, y = 5;
    while (true) {
        drawBoard(x, y);
        x++;
        if (x >= width) x = 0;
        system("pause>nul");
    }
}
