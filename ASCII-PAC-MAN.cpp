#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;

const char* mapFile = "map.txt";

const int MAX_SIZE = 50;
char map[MAX_SIZE][MAX_SIZE];
int mapLength, mapHeigth;
const char path = 'p';

int pacManX, pacManY;
int blinkyX, blinkyY, pinkyX, pinkyY, inkyX, inkyY, clydeX, clydeY;

bool blinkyActive = true, pinkyActive = false, inkyActive = false, clydeActive = false;

bool frightenedMode = false;

int score = 0;

bool isEven(int i) {
    return !(i % 2);
}


bool checkMapSize(int length, int heigth) {
    return !(length > 50 || heigth > 50 || length <=10 || heigth <= 10 || !isEven(length) || !isEven(heigth));
}

void generateAllWallMap(int length, int heigth) {
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < heigth; j++)
        {
            map[i][j] = '#';
        }
        cout << endl;
    }
}

void ghostsPlace(int length, int heigth) {
    int ghostZoneStartX = heigth / 2 - 1;
    int ghostZoneStartY = length / 2 - 2;

    for (int i = ghostZoneStartX; i < ghostZoneStartX + 2; i++) {
        for (int j = ghostZoneStartY; j < ghostZoneStartY + 4; j++) {
            map[i][j] = ' ';
        }
    }

    map[ghostZoneStartX][ghostZoneStartY + 1] = 'B';
    map[ghostZoneStartX][ghostZoneStartY + 2] = 'P';
    map[ghostZoneStartX + 1][ghostZoneStartY + 1] = 'I';
    map[ghostZoneStartX + 1][ghostZoneStartY + 2] = 'C';

}

void placeSuperFood(int length, int heigth) {
    int superFCount = 0;
    srand(time(0));

    while (superFCount < 4)
    {
        int x = rand() % length;
        int y = rand() % heigth;

        if (map[x][y] == '-') {
            map[x][y] = '@';
            superFCount++;
        }
    }
}

void placeFood(int length, int heigth) {
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < heigth; j++)
        {
            if (map[i][j] == path) map[i][j] = '-';
        }
    }

    placeSuperFood(length, heigth);
}

void generatePlayMap(int length, int heigth) {
    if (!checkMapSize(length, heigth))
    {
        cout << "Invalid or uneven map size!" << endl;
        return;
    }

    generateAllWallMap(length, heigth);

    int startX = rand() % (length / 2) * 2 + 1;
    int startY = rand() % (heigth / 2) * 2 + 1;
    map[startX][startY] = path;

    int stack[MAX_SIZE * MAX_SIZE][2];
    int stackSize = 0;
    stack[stackSize][0] = startX;
    stack[stackSize][1] = startY;
    stackSize++;

    const int DX[4] = { 0, 1, 0, -1 };
    const int DY[4] = { 1, 0, -1, 0 };

    while (stackSize > 0) {
        int x = stack[stackSize - 1][0];
        int y = stack[stackSize - 1][1];
        stackSize--;

        int directions[4] = { 0, 1, 2, 3 };
        for (int i = 0; i < 4; i++) {
            int randIdx = rand() % (4 - i);
            swap(directions[i], directions[randIdx]);
        }

        for (int dir : directions) {
            int nx = x + DX[dir] * 2;
            int ny = y + DY[dir] * 2;

            // Проверка за валидност на клетката и границите
            if (nx > 0 && nx < length - 1 && ny > 0 && ny < heigth / 2 && map[nx][ny] == '#') {
                // Генериране на път в първата половина
                map[x + DX[dir]][y + DY[dir]] = path;
                map[nx][ny] = path;

                // Огледално генериране
                int mirrorY1 = heigth - 1 - (y + DY[dir]);
                int mirrorY2 = heigth - 1 - ny;
                map[x + DX[dir]][mirrorY1] = path;
                map[nx][mirrorY2] = path;

                // Добавяне към стека
                stack[stackSize][0] = nx;
                stack[stackSize][1] = ny;
                stackSize++;
            }
        }
    }

    ghostsPlace(length, heigth);
    map[1][1] = 'Y'; // Поставяме Пак-Ман в горния ъгъл
    placeFood(length, heigth);
}

void saveMapInFile(const char* fileName, int length, int heigth) {

    ofstream file(fileName);
    if (!file) {
        cout << "Unable to save map!" << endl;
        return;
    }

    file << length << " " << heigth << endl;
    
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < heigth; j++)
        {
            file << map[i][j];
        }
        file << std::endl;
    }
    file.close();
}

void loadMap(int heigth, int length) {
    
}

int main()
{
    int length, heigth;
    cin >> length >> heigth;
    const char* mapFile = "map.txt";
    generatePlayMap(length, heigth);
    saveMapInFile(mapFile, length, heigth);

    return 0;
}
