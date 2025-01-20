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
int blinkyDirX, blinkyDirY, pinkyDirX, pinkyDirY, inkyDirX, inkyDirY, clydeDirX, clydeDirY;

bool blinkyActive = true, pinkyActive = false, inkyActive = false, clydeActive = false;

bool frightenedMode = false;
int frightenedModeCounter = 0;

int score = 0;

const int DX[4] = { -1, 1, 0, 0 };
const int DY[4] = { 0, 0, -1, 1 };

char pacManDirection = 'D';
int ghostQueue[MAX_SIZE * MAX_SIZE][2];
int ghostVisited[MAX_SIZE][MAX_SIZE];

bool isEven(int i) {
    return !(i % 2);
}


bool checkMapSize(int length, int heigth) {
    return !(length > 50 || heigth > 50 || length <=10 || heigth <= 10 || !isEven(length) || !isEven(heigth));
}

bool isValidMove(int x, int y, int length, int heigth) {
    return x >= 0 && x < length && y >= 0 && y < heigth && map[y][x] != '#';
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

    blinkyDirX = -1; blinkyDirY = 0;  // Moving left initially
    pinkyDirX = 0; pinkyDirY = 1;    // Moving down initially
    inkyDirX = 1; inkyDirY = 0;      // Moving right initially
    clydeDirX = 0; clydeDirY = -1;   // Moving up initially

}

void placeSuperFood(int length, int heigth) {
    int superFCount = 0;
    srand(time(0));

    while (superFCount < 4)
    {
        int x = rand() % length;
        int y = rand() % heigth;

        if (map[y][x] == '-') {
            map[y][x] = '@';
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
    pacManX = 1;
    pacManY = 1;
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

void loadMap(int length, int heigth) {
    system("cls"); // Изчистване на екрана
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < heigth; j++) {
            cout << map[i][j];
        }
        cout << endl;
    }
    cout << "Score: " << score << endl;
}

bool movePacMan(char direction, int length, int heigth) {
    int newX = pacManX, newY = pacManY;

    if (direction == 'W') newY--;    // Move up
    else if (direction == 'S') newY++; // Move down
    else if (direction == 'A') newX--; // Move left
    else if (direction == 'D') newX++; // Move right

    // Check if the move is valid
    if (newX >= 0 && newX < length && newY >= 0 && newY < heigth && map[newY][newX] != '#') {
        pacManX = newX;
        pacManY = newY;
        return true;
    }

    return false; // Invalid move
}

// Handles Pac-Man's double movement during frightened mode
void movePacManFrightened(char direction, int length, int heigth) {
    for (int step = 0; step < 2; step++) {
        if (!movePacMan(direction, length, heigth)) {
            break; // Stop if the move fails
        }
    }
}
void resetGhostVisited() {
    for (int i = 0; i < MAX_SIZE; i++) {
        for (int j = 0; j < MAX_SIZE; j++) {
            ghostVisited[i][j] = -1;
        }
    }
}

void moveBlinky() {
    int front = 0, rear = 0;

    ghostQueue[rear][0] = blinkyX;
    ghostQueue[rear][1] = blinkyY;
    rear++;

    resetGhostVisited();
    ghostVisited[blinkyX][blinkyY] = 0;

    while (front < rear) {
        int x = ghostQueue[front][0];
        int y = ghostQueue[front][1];
        front++;

        for (int dir = 0; dir < 4; dir++) {
            int nx = x + DX[dir];
            int ny = y + DY[dir];

            if (nx >= 0 && nx < mapLength && ny >= 0 && ny < mapHeigth && map[nx][ny] != '#' && ghostVisited[nx][ny] == -1) {
                ghostVisited[nx][ny] = ghostVisited[x][y] + 1;
                ghostQueue[rear][0] = nx;
                ghostQueue[rear][1] = ny;
                rear++;
            }
        }
    }

    int bestDir = -1;
    int shortestDistance = INT_MAX;

    for (int dir = 0; dir < 4; dir++) {
        int nx = blinkyX + DX[dir];
        int ny = blinkyY + DY[dir];

        if (nx >= 0 && nx < mapLength && ny >= 0 && ny < mapHeigth && ghostVisited[nx][ny] != -1) {
            if (ghostVisited[nx][ny] < shortestDistance) {
                shortestDistance = ghostVisited[nx][ny];
                bestDir = dir;
            }
        }
    }

    if (bestDir != -1) {
        blinkyX += DX[bestDir];
        blinkyY += DY[bestDir];
    }
}

void movePinky() {
    int targetX = pacManX;
    int targetY = pacManY;

    switch (pacManDirection) {
    case 'W': // Moving Up
        targetX -= 4;
        targetY -= 4;
        break;
    case 'S': // Moving Down
        targetX += 4;
        break;
    case 'A': // Moving Left
        targetY -= 4;
        break;
    case 'D': // Moving Right
        targetY += 4;
        break;
    }

    targetX = max(0, min(targetX, mapLength - 1));
    targetY = max(0, min(targetY, mapHeigth - 1));

    int front = 0, rear = 0;

    ghostQueue[rear][0] = pinkyX;
    ghostQueue[rear][1] = pinkyY;
    rear++;

    resetGhostVisited();
    ghostVisited[pinkyX][pinkyY] = 0;

    while (front < rear) {
        int x = ghostQueue[front][0];
        int y = ghostQueue[front][1];
        front++;

        for (int dir = 0; dir < 4; dir++) {
            int nx = x + DX[dir];
            int ny = y + DY[dir];

            if (nx >= 0 && nx < mapLength && ny >= 0 && ny < mapHeigth && map[nx][ny] != '#' && ghostVisited[nx][ny] == -1) {
                ghostVisited[nx][ny] = ghostVisited[x][y] + 1;
                ghostQueue[rear][0] = nx;
                ghostQueue[rear][1] = ny;
                rear++;
            }
        }
    }

    int bestDir = -1;
    int shortestDistance = INT_MAX;

    for (int dir = 0; dir < 4; dir++) {
        int nx = pinkyX + DX[dir];
        int ny = pinkyY + DY[dir];

        if (nx >= 0 && nx < mapLength && ny >= 0 && ny < mapHeigth && ghostVisited[nx][ny] != -1) {
            if (ghostVisited[nx][ny] < shortestDistance) {
                shortestDistance = ghostVisited[nx][ny];
                bestDir = dir;
            }
        }
    }

    if (bestDir != -1) {
        pinkyX += DX[bestDir];
        pinkyY += DY[bestDir];
    }
}

void moveInky() {
    int vectorX = pacManX - blinkyX;
    int vectorY = pacManY - blinkyY;

    int targetX = pacManX + vectorX;
    int targetY = pacManY + vectorY;

    targetX = max(0, min(targetX, mapLength - 1));
    targetY = max(0, min(targetY, mapHeigth - 1));

    int front = 0, rear = 0;

    ghostQueue[rear][0] = inkyX;
    ghostQueue[rear][1] = inkyY;
    rear++;

    resetGhostVisited();
    ghostVisited[inkyX][inkyY] = 0;

    while (front < rear) {
        int x = ghostQueue[front][0];
        int y = ghostQueue[front][1];
        front++;

        for (int dir = 0; dir < 4; dir++) {
            int nx = x + DX[dir];
            int ny = y + DY[dir];

            if (nx >= 0 && nx < mapLength && ny >= 0 && ny < mapHeigth && map[nx][ny] != '#' && ghostVisited[nx][ny] == -1) {
                ghostVisited[nx][ny] = ghostVisited[x][y] + 1;
                ghostQueue[rear][0] = nx;
                ghostQueue[rear][1] = ny;
                rear++;
            }
        }
    }

    int bestDir = -1;
    int shortestDistance = INT_MAX;

    for (int dir = 0; dir < 4; dir++) {
        int nx = inkyX + DX[dir];
        int ny = inkyY + DY[dir];

        if (nx >= 0 && nx < mapLength && ny >= 0 && ny < mapHeigth && ghostVisited[nx][ny] != -1) {
            if (ghostVisited[nx][ny] < shortestDistance) {
                shortestDistance = ghostVisited[nx][ny];
                bestDir = dir;
            }
        }
    }

    if (bestDir != -1) {
        inkyX += DX[bestDir];
        inkyY += DY[bestDir];
    }
}


void reverseGhost(int& ghostDirX, int& ghostDirY) {
    ghostDirX = -ghostDirX;
    ghostDirY = -ghostDirY;
}

void activateFrightenedMode() {
    frightenedMode = true;
    frightenedModeCounter = 0;

    // Reverse all ghosts
    reverseGhost(blinkyDirX, blinkyDirY);
    reverseGhost(pinkyDirX, pinkyDirY);
    reverseGhost(inkyDirX, inkyDirY);
    reverseGhost(clydeDirX, clydeDirY);
}

bool isValidMove(int x, int y, int length, int heigth) {
    return x >= 0 && x < length && y >= 0 && y < heigth && map[x][y] != '#';
}

void moveGhostRandomly(int& ghostX, int& ghostY, int& dirX, int& dirY, int length, int heigth) {
    int possibleDirections[4][2] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };
    int newDirX, newDirY;

    do {
        int randomIndex = rand() % 4;
        newDirX = possibleDirections[randomIndex][0];
        newDirY = possibleDirections[randomIndex][1];
    } while ((newDirX == -dirX && newDirY == -dirY) ||
        !isValidMove(ghostX + newDirX, ghostY + newDirY, length, heigth));

    dirX = newDirX;
    dirY = newDirY;
    ghostX += dirX;
    ghostY += dirY;
}

bool checkGameOver() {
    if ((pacManX == blinkyX && pacManY == blinkyY) ||
        (pacManX == pinkyX && pacManY == pinkyY) ||
        (pacManX == inkyX && pacManY == inkyY) ||
        (pacManX == clydeX && pacManY == clydeY)) {
        return true;
    }
    for (int i = 0; i < mapLength; i++) {
        for (int j = 0; j < mapHeigth; j++) {
            if (map[i][j] == '-' || map[i][j] == '@') {
                return false;
            }
        }
    }
    return true;
}

void moveClyde() {
    int distanceToPacMan = abs(clydeX - pacManX) + abs(clydeY - pacManY);

    if (distanceToPacMan <= 8) {
        // Move away from Pac-Man
        for (int dir = 0; dir < 4; dir++) {
            int nx = clydeX - DX[dir];
            int ny = clydeY - DY[dir];

            if (nx >= 0 && nx < mapLength && ny >= 0 && ny < mapHeigth && map[nx][ny] != '#') {
                clydeX = nx;
                clydeY = ny;
                return;
            }
        }
    }
    else {
        // Random movement
        int randDir = rand() % 4;
        int nx = clydeX + DX[randDir];
        int ny = clydeY + DY[randDir];

        if (nx >= 0 && nx < mapLength && ny >= 0 && ny < mapHeigth && map[nx][ny] != '#') {
            clydeX = nx;
            clydeY = ny;
        }
    }
}
void checkGhostEaten(int ghostX, int ghostY, char ghost) {
    if (pacManX == ghostX && pacManY == ghostY) {
        if (frightenedMode) {
            // Pac-Man eats the ghost
            score += 200; // Example score for eating a ghost
            cout << "Ghost " << ghost << " eaten! Score: " << score << endl;

            // Send the ghost back to its starting position
            switch (ghost) {
            case 'B':
                blinkyX = mapLength - 1; // Top-right corner
                blinkyY = 0;
                break;
            case 'P':
                pinkyX = 0; // Top-left corner
                pinkyY = 0;
                break;
            case 'I':
                inkyX = mapLength - 1; // Bottom-right corner
                inkyY = mapHeigth - 1;
                break;
            case 'C':
                clydeX = 0; // Bottom-left corner
                clydeY = mapHeigth - 1;
                break;
            default:
                break;
            }
        }
        else {
            cout << "You got caught by ghost " << ghost << "! Game Over!" << endl;
            exit(0);
        }
    }
}

void resetGhostMovement(int& dirX, int& dirY, char ghost) {
    // Reset to default directions based on the ghost's starting direction
    switch (ghost) {
    case 'B': // Blinky's default direction
        dirX = -1; // Example: Moving left initially
        dirY = 0;
        break;
    case 'P': // Pinky's default direction
        dirX = 0; // Example: Moving downward initially
        dirY = 1;
        break;
    case 'I': // Inky's default direction
        dirX = 1; // Example: Moving right initially
        dirY = 0;
        break;
    case 'C': // Clyde's default direction
        dirX = 0; // Example: Moving upward initially
        dirY = -1;
        break;
    default:
        dirX = 0;
        dirY = 0;
        break;
    }
}



void terminateFrightenedMode() {
    frightenedMode = false;
    resetGhostMovement(blinkyDirX, blinkyDirY, 'B');
    resetGhostMovement(pinkyDirX, pinkyDirY, 'P');
    resetGhostMovement(inkyDirX, inkyDirY, 'I');
    resetGhostMovement(clydeDirX, clydeDirY, 'C');
}

void gameLoop(int length, int heigth) {
    while (!checkGameOver()) {
        loadMap(length, heigth);

        // Handle Pac-Man movement
        if (frightenedMode) {
            if (GetAsyncKeyState('W') & 0x8000) {
                movePacManFrightened('W', length, heigth);
            }
            else if (GetAsyncKeyState('S') & 0x8000) {
                movePacManFrightened('S', length, heigth);
            }
            else if (GetAsyncKeyState('A') & 0x8000) {
                movePacManFrightened('A', length, heigth);
            }
            else if (GetAsyncKeyState('D') & 0x8000) {
                movePacManFrightened('D', length, heigth);
            }
        }
        else {
            if (GetAsyncKeyState('W') & 0x8000) {
                movePacMan('W', length, heigth);
            }
            else if (GetAsyncKeyState('S') & 0x8000) {
                movePacMan('S', length, heigth);
            }
            else if (GetAsyncKeyState('A') & 0x8000) {
                movePacMan('A', length, heigth);
            }
            else if (GetAsyncKeyState('D') & 0x8000) {
                movePacMan('D', length, heigth);
            }
        }

        // Activate frightened mode on superfood consumption
        if (map[pacManY][pacManX] == '@') {
            activateFrightenedMode();
            frightenedModeCounter = 0;
            map[pacManY][pacManX] = ' '; // Remove superfood
        }

        // Handle frightened mode logic
        if (frightenedMode) {
            frightenedModeCounter++;
            if (frightenedModeCounter >= 10) {
                terminateFrightenedMode();
            }
        }

        if (frightenedMode)
        {
            // Move ghosts randomly during frightened mode
            moveGhostRandomly(blinkyX, blinkyY, blinkyDirX, blinkyDirY, length, heigth);
            moveGhostRandomly(pinkyX, pinkyY, pinkyDirX, pinkyDirY, length, heigth);
            moveGhostRandomly(inkyX, inkyY, inkyDirX, inkyDirY, length, heigth);
            moveGhostRandomly(clydeX, clydeY, clydeDirX, clydeDirY, length, heigth);
        }
        else {
            // Regular ghost movement
            moveBlinky();
            movePinky();
            moveInky();
            moveClyde();
        }

        // Check for ghost collisions and respawn if needed
        checkGhostEaten(blinkyX, blinkyY, 'B');
        checkGhostEaten(pinkyX, pinkyY, 'P');
        checkGhostEaten(inkyX, inkyY, 'I');
        checkGhostEaten(clydeX, clydeY, 'C');

        // Check game over
        if (checkGameOver()) {
            cout << "Game Over! Final Score: " << score << endl;
            break;
        }

        Sleep(100); // Adjust game speed
    }
}

/*void gameLoop() {
    while (!checkGameOver()) {
        // Clear the screen
        system("cls");

        // Display the map and score
        loadMap(mapLength, mapHeigth);

        // Input and Pac-Man movement
        char input;
        cout << "Enter move (W/A/S/D): ";
        cin >> input;
        input = toupper(input);

        if (frightenedMode) {
            movePacManFrightened(input, mapLength, mapHeigth);
        }
        else {
            movePacMan(input, mapLength, mapHeigth);
        }

        // Move ghosts
        if (blinkyActive) moveBlinky();
        if (pinkyActive) movePinky();
        if (inkyActive) moveInky();
        if (clydeActive) moveClyde();

        // Check if Pac-Man eats food
        if (map[pacManX][pacManY] == '-') {
            map[pacManX][pacManY] = ' ';
            score += 10;
        }
        else if (map[pacManX][pacManY] == '@') {
            map[pacManX][pacManY] = ' ';
            score += 50;
            activateFrightenedMode();
        }

        // Check ghost interactions
        checkGhostEaten(blinkyX, blinkyY, 'B');
        checkGhostEaten(pinkyX, pinkyY, 'P');
        checkGhostEaten(inkyX, inkyY, 'I');
        checkGhostEaten(clydeX, clydeY, 'C');

        // Update frightened mode counter
        if (frightenedMode) {
            frightenedModeCounter++;
            if (frightenedModeCounter >= 50) { // Frightened mode lasts 50 cycles
                terminateFrightenedMode();
            }
        }

        // Pause briefly to control game speed
        Sleep(200); // Adjust the duration as needed
    }

    cout << "Game Over! Final Score: " << score << endl;
}*/

int main() {
    srand(time(0)); // Seed random number generator

    cout << "Enter map dimensions (length height): ";
    cin >> mapLength >> mapHeigth;

    if (!checkMapSize(mapLength, mapHeigth)) {
        cout << "Invalid map size! Ensure the dimensions are even and between 10 and 50.\n";
        return 1;
    }

    // Generate and display the map
    generatePlayMap(mapLength, mapHeigth);
    saveMapInFile(mapFile, mapLength, mapHeigth);

    gameLoop(mapLength, mapHeigth);

    return 0;
}