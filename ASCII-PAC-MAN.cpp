#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <windows.h>

using namespace std;

char** map = nullptr;

static const char PACMAN_CHAR = 'Y';
static const char BLINKY_CHAR = 'B';
static const char PINKY_CHAR = 'P';
static const char INKY_CHAR = 'I';
static const char CLYDE_CHAR = 'C';

int rows = 0;
int columns = 0;

bool gameOver = false;
bool frightenedMode = false;
bool switchMode = false;
int  frightenedModeMovesCounter = 0;
int  currentScore = 0;

char blinkyPrevious = ' ';
char pinkyPrevious = ' ';
char inkyPrevious = ' ';
char clydePrevious = ' ';

int blinkyPrevDr = 0, blinkyPrevDc = 0;
int pinkyPrevDr = 0, pinkyPrevDc = 0;
int inkyPrevDr = 0, inkyPrevDc = 0;
int clydePrevDr = 0, clydePrevDc = 0;

bool isValidPosition(int r, int c) {
    return (r >= 0 && r < rows && c >= 0 && c < columns);
}

bool isValidMovePacMan(int r, int c) {
    if (!isValidPosition(r, c)) return false;
    return (map[r][c] == ' ' || map[r][c] == '-' || map[r][c] == '@');
}

bool isValidMoveGhost(int r, int c)
{
    if (!isValidPosition(r, c)) return false;

    if (map[r][c] == '#') return false;

    if (map[r][c] == BLINKY_CHAR || map[r][c] == PINKY_CHAR || map[r][c] == INKY_CHAR || map[r][c] == CLYDE_CHAR) {
        return false;
    }
    return true;
}

int pelletsSum() {
    int total = 0;
    for (int r = 0; r < rows; r++) {

        for (int c = 0; c < columns; c++) {

            if (map[r][c] == '-') total++;
            if (map[r][c] == '@') total += 50;
        }
    }
    return total;
}

bool findCharacter(char ch, int& row, int& column) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            if (map[r][c] == ch) {
                row = r;
                column = c;
                return true;
            }
        }
    }
    return false;
}


void updateFrightenedMode() {
    if (!frightenedMode) return;
    frightenedModeMovesCounter++;
    if (frightenedModeMovesCounter >= 10) { // lasts 10 moves
        frightenedMode = false;
        frightenedModeMovesCounter = 0;
    }
}

void ghostEaten(char ghostChar, int& gRow, int& gColumn, char& prevTile, int& previousDir, int& previousDc) {
    map[gRow][gColumn] = prevTile;

    if (ghostChar == BLINKY_CHAR) {
        // top-right corner
        gRow = 1;
        gColumn = columns - 2;
    }
    else if (ghostChar == PINKY_CHAR) {
        // top-left corner
        gRow = 1;
        gColumn = 1;
    }
    else if (ghostChar == INKY_CHAR) {
        // bottom-right corner
        gRow = rows - 2;
        gColumn = columns - 2;
    }
    else if (ghostChar == CLYDE_CHAR) {
        // bottom-left corner
        gRow = rows - 2;
        gColumn = 1;
    }

    // Put ghost in corner, store old tile
    prevTile = map[gRow][gColumn];
    map[gRow][gColumn] = ghostChar;

    // Reset direction deltas
    previousDir = 0;
    previousDc = 0;
}

void calculateNewPosition(int& r, int& c, char dir, int step) {
    switch (dir) {
    case 'w': case 'W': r -= step; break;
    case 'a': case 'A': c -= step; break;
    case 's': case 'S': r += step; break;
    case 'd': case 'D': c += step; break;
    }
}

// Function to check for ghost collisions
bool checkForGhostCollision(int& pacRow, int& pacColumn, char& tile) {
    if (tile == BLINKY_CHAR || tile == PINKY_CHAR || tile == INKY_CHAR || tile == CLYDE_CHAR) {
        if (!frightenedMode) {
            gameOver = true;
            cout << "Game Over! Caught by a ghost!" << endl;
            return true;
        }
        else {
            // If frightened, send the ghost to a corner
            if (tile == BLINKY_CHAR) {
                ghostEaten(BLINKY_CHAR, pacRow, pacColumn, blinkyPrevious, blinkyPrevDr, blinkyPrevDc);
            }
            else if (tile == PINKY_CHAR) {
                ghostEaten(PINKY_CHAR, pacRow, pacColumn, pinkyPrevious, pinkyPrevDr, pinkyPrevDc);
            }
            else if (tile == INKY_CHAR) {
                ghostEaten(INKY_CHAR, pacRow, pacColumn, inkyPrevious, inkyPrevDr, inkyPrevDc);
            }
            else if (tile == CLYDE_CHAR) {
                ghostEaten(CLYDE_CHAR, pacRow, pacColumn, clydePrevious, clydePrevDr, clydePrevDc);
            }
            return false;  // Pac-Man continues moving
        }
    }
    return false;  // No collision
}

// Function to handle power-up logic (frightened mode)
void checkFrightenedMode(int& pacRow, int& pacColumn, char& tile) {
    if (tile == '@') {
        frightenedMode = true;
        frightenedModeMovesCounter = 0;
        switchMode = true;
    }
}

// Function to update the map after moving Pac-Man
void updateMap(int& pacRow, int& pacColumn, int nr, int nc) {
    map[pacRow][pacColumn] = ' ';
    map[nr][nc] = PACMAN_CHAR;
}

// Function to move Pac-Man
void movePacMan(int& pacRow, int& pacColumn, char inputDir, int& score) {
    // Pac-Man moves 2 squares if frightened, else 1
    int steps = (frightenedMode ? 2 : 1);

    while (steps-- > 0) {
        int nr = pacRow;
        int nc = pacColumn;
        calculateNewPosition(nr, nc, inputDir, 1);

        if (!isValidMovePacMan(nr, nc)) {
            break;  // Can't move into wall or out-of-bounds
        }

        char tile = map[nr][nc];

        // Check for ghost collision
        if (checkForGhostCollision(nr, nc, tile)) {
            return;
        }

        // Handle power-up logic
        checkFrightenedMode(nr, nc, tile);

        // Update the map with Pac-Man's new position
        updateMap(pacRow, pacColumn, nr, nc);

        // Increase score if Pac-Man eats a pellet or power-up
        if (tile == '-') score++; 
        if (tile == '@') score += 50;
            
        pacRow = nr;
        pacColumn = nc;

        updateFrightenedMode();
        if (gameOver) return;
    }
}

// Function to evaluate the best direction for the ghost to move towards the target
bool evaluateBestDirection(int gRow, int gColumn, int targetRow, int targetColumn,
    int previousDr, int previousDc, int& chosenDr, int& chosenDc) {
    struct Direction { int dr, dc; };
    Direction directions[4] = {
        { -1,  0 },  // Up
        {  0, -1 },  // Left
        {  1,  0 },  // Down
        {  0,  1 }   // Right
    };

    int oppositeDr = -previousDr, oppositeDc = -previousDc;
    double bestDist = 1e9;  // Initialize with a large value
    bool foundValid = false;

    // Evaluate each direction
    for (int i = 0; i < 4; i++) {
        int dr = directions[i].dr;
        int dc = directions[i].dc;

        // Skip immediate reverse direction
        if (dr == oppositeDr && dc == oppositeDc) {
            continue;
        }

        int nr = gRow + dr;
        int nc = gColumn + dc;

        // Check if the move is valid
        if (!isValidMoveGhost(nr, nc)) {
            continue;
        }

        // Calculate distance to target
        double dist = sqrt(pow(targetRow - nr, 2) + pow(targetColumn - nc, 2));
        if (dist < bestDist) {
            bestDist = dist;
            chosenDr = dr;
            chosenDc = dc;
            foundValid = true;
        }
    }

    // If no valid direction found, try the reverse direction or stand still
    if (!foundValid) {
        int nr = gRow + oppositeDr;
        int nc = gColumn + oppositeDc;
        if (isValidMoveGhost(nr, nc)) {
            chosenDr = oppositeDr;
            chosenDc = oppositeDc;
            foundValid = true;
        }
        else {
            chosenDr = 0;
            chosenDc = 0;
            foundValid = true;
        }
    }

    return foundValid;
}

// Function to move the ghost and handle collisions
void moveGhostChaseMode(int& gRow, int& gColumn, int& previousDr, int& previousDc, char& previousTile,
    char ghostCh, int targetRow, int targetColumn) {

    // Evaluate the best direction for the ghost to move
    int chosenDr = 0, chosenDc = 0;
    bool directionFound = evaluateBestDirection(gRow, gColumn, targetRow, targetColumn, previousDr, previousDc, chosenDr, chosenDc);

    if (!directionFound) {
        // If no valid direction is found, stand still (though this case should not happen)
        return;
    }

    // Move the ghost in the chosen direction
    map[gRow][gColumn] = previousTile;  // Reset the previous tile
    gRow += chosenDr;
    gColumn += chosenDc;

    previousTile = map[gRow][gColumn];
    map[gRow][gColumn] = ghostCh;  // Set the new ghost position

    // Update the previous movement direction for the next move
    previousDr = chosenDr;
    previousDc = chosenDc;

    // Handle collision with Pac-Man
    if (previousTile == PACMAN_CHAR) {
        if (!frightenedMode) {
            gameOver = true;
            cout << "Game Over! A ghost caught Pac-Man!" << endl;
        }
        else {
            ghostEaten(ghostCh, gRow, gColumn, previousTile, previousDr, previousDc);
        }
    }
}

void moveGhostFrightened(int& gRow, int& gColumn, int& previousDr, int& previousDc, char& previousTile, char ghostCh) {
    map[gRow][gColumn] = previousTile;

    int directions[4][2] = {
        {-1, 0}, // Up
        { 0,-1}, // Left
        { 1, 0}, // Down
        { 0, 1}  // Right
    };

    int oppDr = -previousDr, oppDc = -previousDc;
    bool moved = false;
    int tries = 10;

    while (!moved && tries-- > 0) {
        int i = rand() % 4;
        int dr = directions[i][0];
        int dc = directions[i][1];

        // skip immediate reverse
        if (dr == oppDr && dc == oppDc) {
            continue;
        }
        int nr = gRow + dr;
        int nc = gColumn + dc;

        if (!isValidMoveGhost(nr, nc)) {
            continue;
        }

        // Move
        gRow = nr;
        gColumn = nc;
        previousTile = map[gRow][gColumn];
        map[gRow][gColumn] = ghostCh;
        previousDr = dr;
        previousDc = dc;
        moved = true;

        // Collide with Pac-Man => corner
        if (previousTile == PACMAN_CHAR) {
            ghostEaten(ghostCh, gRow, gColumn, previousTile, previousDr, previousDc);
        }
    }
    // If we fail all attempts, try reversing or stand still
    if (!moved) {
        int nr = gRow + oppDr;
        int nc = gColumn + oppDc;
        if (isValidMoveGhost(nr, nc)) {
            gRow = nr;
            gColumn = nc;
            previousTile = map[gRow][gColumn];
            map[gRow][gColumn] = ghostCh;
            previousDr = oppDr;
            previousDc = oppDc;

            if (previousTile == PACMAN_CHAR) {
                ghostEaten(ghostCh, gRow, gColumn, previousTile, previousDr, previousDc);
            }
        }
        else {
            // stand still
            map[gRow][gColumn] = ghostCh;
        }
    }
}

void movePinky(int& pinkyRow, int& pinkyColumn, int& pinkyDirectionRow, int& pinkyDirectionColumn,
    char& pinkyPreviousTile, int pacRow, int pacColumn, char pacOrientation) {

    // Determine the target position based on Pac-Man's direction
    int targetRow = pacRow, targetColumn = pacColumn;

    // Adjust the target position based on the direction Pac-Man is facing
    if (pacOrientation == 'w' || pacOrientation == 'W') {
        targetRow -= 4;
        targetColumn -= 4;
    }
    else if (pacOrientation == 's' || pacOrientation == 'S') {
        targetRow += 4;
    }
    else if (pacOrientation == 'a' || pacOrientation == 'A') {
        targetColumn -= 4;
    }
    else if (pacOrientation == 'd' || pacOrientation == 'D') {
        targetColumn += 4;
    }

    // Clamp the target position to ensure it's within the bounds of the map
    if (targetRow < 0) targetRow = 0;
    if (targetRow >= rows) targetRow = rows - 1;
    if (targetColumn < 0) targetColumn = 0;
    if (targetColumn >= columns) targetColumn = columns - 1;

    // Move Pinky towards the target position
    moveGhostChaseMode(pinkyRow, pinkyColumn, pinkyDirectionRow, pinkyDirectionColumn, pinkyPreviousTile, PINKY_CHAR, targetRow, targetColumn);
}

void moveInky(int& inkyRow, int& inkyColumn, int& inkyDirectionRow, int& inkyDirectionColumn,
    char& inkyPreviousTile, int pacRow, int pacColumn, int blinkyRow, int blinkyColumn, char pacOrientation) {

    // Calculate Inky's target position based on Pac-Man's direction
    int referenceRow = pacRow, referenceColumn = pacColumn;

    // Adjust reference position based on Pac-Man's orientation (w, a, s, d)
    if (pacOrientation == 'w' || pacOrientation == 'W') {
        referenceRow -= 2;
        referenceColumn -= 2;
    }
    else if (pacOrientation == 's' || pacOrientation == 'S') {
        referenceRow += 2;
    }
    else if (pacOrientation == 'a' || pacOrientation == 'A') {
        referenceColumn -= 2;
    }
    else if (pacOrientation == 'd' || pacOrientation == 'D') {
        referenceColumn += 2;
    }

    // Calculate the "scattered" position for Inky based on Blinky's position
    int vectorRow = (referenceRow - blinkyRow) * 2;
    int vectorColumn = (referenceColumn - blinkyColumn) * 2;

    // Determine the target position for Inky
    int targetRow = blinkyRow + vectorRow;
    int targetColumn = blinkyColumn + vectorColumn;

    // Make sure Inky's target position is within the bounds of the map
    if (targetRow < 0) targetRow = 0;
    if (targetRow >= rows) targetRow = rows - 1;
    if (targetColumn < 0) targetColumn = 0;
    if (targetColumn >= columns) targetColumn = columns - 1;

    // Move Inky towards the target position
    moveGhostChaseMode(inkyRow, inkyColumn, inkyDirectionRow, inkyDirectionColumn, inkyPreviousTile, INKY_CHAR, targetRow, targetColumn);
}

void moveClyde(int& cRow, int& cColumn, int& cDirectionRow, int& cDirectionColumn, char& cPreviousTile, int pacRow, int pacColumn)
{
    // Calculate the Euclidean distance between Clyde and Pac-Man
    double distanceToPacMan = sqrt(pow(cColumn - pacColumn, 2) + pow(cRow - pacRow, 2));

    // If Clyde is far enough from Pac-Man, he chases Pac-Man
    if (distanceToPacMan >= 8.0) {
        moveGhostChaseMode(cRow, cColumn, cDirectionRow, cDirectionColumn, cPreviousTile, CLYDE_CHAR, pacRow, pacColumn);
    }
    else {
        // If Clyde is close enough, he retreats to the bottom-left corner
        int cornerRow = rows - 1;
        int cornerColumn = 0;

        moveGhostChaseMode(cRow, cColumn, cDirectionRow, cDirectionColumn, cPreviousTile, CLYDE_CHAR, cornerRow, cornerColumn);
    }
}

void loadMap(const char* filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filePath << std::endl;
        return;
    }
    file >> rows >> columns;
    file.ignore();

    map = new char* [rows];
    for (int i = 0; i < rows; ++i) {
        map[i] = new char[columns + 1];
        file.getline(map[i], columns + 1);
    }
    file.close();
}

void clearMap() {
    if (!map) return;
    for (int i = 0; i < rows; i++) {
        delete[] map[i];
    }
    delete[] map;
    map = nullptr;
}

void printMap() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            char ch = map[i][j];
            switch (ch) {
            case PACMAN_CHAR: // 'Y'
                cout << ch;
                break;
            case BLINKY_CHAR: // 'B'
                cout << ch;
                break;
            case PINKY_CHAR:  // 'P'
                cout << ch;
                break;
            case INKY_CHAR:   // 'I'
                cout << ch;
                break;
            case CLYDE_CHAR:  // 'C'
                cout << ch;
                break;
            default:
                cout << ch;
                break;
            }
        }
        cout << endl;
    }
}

// Function to handle user input (movement or quitting)
bool directionInput(char& pacOrientation) {
    cout << endl << "Move (w/a/s/d) or q to quit: ";
    char moveKey;
    cin >> moveKey;

    // Handle quit or invalid input
    if (moveKey == 'q') return false;
    if (moveKey != 'w' && moveKey != 'a' && moveKey != 's' && moveKey != 'd') {
        return true; // Skip invalid keys
    }

    pacOrientation = moveKey; // Update Pac-Man's direction
    return true; // Valid input
}

// Function to move Pac-Man
void movePacManIfValid(int& pacRow, int& pacColumn, char pacOrientation, int& score) {
    movePacMan(pacRow, pacColumn, pacOrientation, score);
    if (gameOver) {
        cout << "Game Over! Caught by a ghost!" << endl;
    }
}

// Function to move ghosts in frightened mode
void moveGhostsInFrightenedMode(int& bRow, int& bColumn, int& pRow, int& pColumn,
    int& iRow, int& iColumn, int& cRow, int& cColumn) {
    moveGhostFrightened(bRow, bColumn, blinkyPrevDr, blinkyPrevDc, blinkyPrevious, BLINKY_CHAR);
    if (gameOver) return;

    moveGhostFrightened(pRow, pColumn, pinkyPrevDr, pinkyPrevDc, pinkyPrevious, PINKY_CHAR);
    if (gameOver) return;

    moveGhostFrightened(iRow, iColumn, inkyPrevDr, inkyPrevDc, inkyPrevious, INKY_CHAR);
    if (gameOver) return;

    moveGhostFrightened(cRow, cColumn, clydePrevDr, clydePrevDc, clydePrevious, CLYDE_CHAR);
    if (gameOver) return;
}

// Function to move ghosts in normal chase mode
void moveGhostsInNormalMode(int& bRow, int& bColumn, int& pRow, int& pColumn,
    int& iRow, int& iColumn, int& cRow, int& cColumn,
    int pacRow, int pacColumn, int currentScore, char pacOrientation) {
    moveGhostChaseMode(bRow, bColumn, blinkyPrevDr, blinkyPrevDc, blinkyPrevious, BLINKY_CHAR, pacRow, pacColumn);
    if (gameOver) return;

    // Activate Pinky at 20 points
    if (currentScore >= 20) {
        movePinky(pRow, pColumn, pinkyPrevDr, pinkyPrevDc, pinkyPrevious, pacRow, pacColumn, pacOrientation);
        if (gameOver) return;
    }

    // Activate Inky at 40 points
    if (currentScore >= 40) {
        moveInky(iRow, iColumn, inkyPrevDr, inkyPrevDc, inkyPrevious, pacRow, pacColumn, bRow, bColumn, pacOrientation);
        if (gameOver) return;
    }

    // Activate Clyde at 60 points
    if (currentScore >= 60) {
        moveClyde(cRow, cColumn, clydePrevDr, clydePrevDc, clydePrevious, pacRow, pacColumn);
        if (gameOver) return;
    }
}

// Function to check the win condition
bool checkWinCondition(int currentScore, int totalPellets) {
    if (currentScore >= totalPellets) {
        cout << "You Win! All pellets collected!" << endl;
        return true;
    }
    return false;
}


// The main game loop
void gameLoop(int& pacRow, int& pacColumn, char& pacOrientation,
    int& bRow, int& bColumn,
    int& pRow, int& pColumn,
    int& iRow, int& iColumn,
    int& cRow, int& cColumn,
    int totalPellets) {

    while (!gameOver) {
        system("cls");
        printMap();
        cout << "Score: " << currentScore << " / " << totalPellets << endl;

        // Check for win condition
        if (checkWinCondition(currentScore, totalPellets)) break;

        // Handle user input (movement or quit)
        if (!directionInput(pacOrientation)) break;

        // Move Pac-Man
        movePacManIfValid(pacRow, pacColumn, pacOrientation, currentScore);
        if (gameOver) break;

        // Handle ghosts' movement
        if (frightenedMode) {
            if (switchMode) {
                // Reverse previous movement directions for ghosts
                blinkyPrevDr *= -1; blinkyPrevDc *= -1;
                pinkyPrevDr *= -1; pinkyPrevDc *= -1;
                inkyPrevDr *= -1; inkyPrevDc *= -1;
                clydePrevDr *= -1; clydePrevDc *= -1;
                switchMode = false; // Reset switch mode
            }
            moveGhostsInFrightenedMode(bRow, bColumn, pRow, pColumn, iRow, iColumn, cRow, cColumn);
            continue; // Skip the normal ghost chase logic
        }

        // Normal mode: Ghosts chase Pac-Man
        moveGhostsInNormalMode(bRow, bColumn, pRow, pColumn, iRow, iColumn, cRow, cColumn, pacRow, pacColumn, currentScore, pacOrientation);
    }
}

int main() {
    srand((unsigned)time(NULL));

    const char* mapFile = "map.txt";
    loadMap(mapFile);
    if (!map) {
        cerr << "Error: Could not load map." << endl;
        return 1;
    }

    int totalPellets = pelletsSum();

    int pacRow = 0, pacCol = 0;
    if (!findCharacter(PACMAN_CHAR, pacRow, pacCol)) {
        cerr << "Error: Pac-Man not found in map!" << endl;
        clearMap();
        return 1;
    }

    int blinkyRow = 0, blinkyColumn = 0;
    int pinkyRow = 0, pinkyColumn = 0;
    int inkyRow = 0, inkyColumn = 0;
    int clydeRow = 0, clydeColumn = 0;
    findCharacter(BLINKY_CHAR, blinkyRow, blinkyColumn);
    findCharacter(PINKY_CHAR, pinkyRow, pinkyColumn);
    findCharacter(INKY_CHAR, inkyRow, inkyColumn);
    findCharacter(CLYDE_CHAR, clydeRow, clydeColumn);

    // Initialize
    //blinkyPrevious = ' ';
    //pinkyPrevious = ' ';
    //inkyPrevious = ' ';
    //clydePrevious = ' ';

    char pacManOrientation = 'd'; // Default facing right

    gameLoop(pacRow, pacCol, pacManOrientation, blinkyRow, blinkyColumn, pinkyRow, pinkyColumn, 
        inkyRow, inkyColumn, clydeRow, clydeColumn, totalPellets);

    clearMap();
    return 0;
}