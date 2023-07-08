#include <raylib.h>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

wstring tetromino[7];

unsigned char *field = nullptr;

bool forceDown = false;
bool gameOver = false;

int fieldWidth = 12;    
int fieldHeight = 22;

int speed = 20;
int speedCounter = 0;
int toggle = 0;

int currentPiece = 0;
int currentRotation = 0;
int currentX = 2;
int currentY = 1;

int pieceCount = 0;
int Score = 0;

const Color green = {47, 230, 23, 255};
const Color red = {232, 18, 18, 255};
const Color orange = {226, 116, 17, 255};
const Color yellow = {237, 234, 4, 255};
const Color purple = {166, 0, 247, 255};
const Color cyan = {21, 204, 209, 255};
const Color blue = {13, 64, 216, 255};
const Color lightBlue = {59, 85, 162, 255};

std::vector<Color> GetCellColors()
{
    return {RAYWHITE, green, red, orange, yellow, purple, cyan, blue, lightBlue};
}

std::vector<Color> colors = GetCellColors();

int screenWidth = 240;
int screenHeight = 440;

vector<int> vLines;

bool rotateHold = true;

int Rotate(int px, int py, int r)
{
    switch (r % 4)
    {
    case 0:
        return py * 4 + px;
    case 1:
        return 12 + py - (px * 4);
    case 2:
        return 15 - (py * 4) - px;
    case 3:
        return 3 - py + (px * 4);
    }
    return 0;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
    // All Field cells >0 are occupied
    for (int px = 0; px < 4; px++)
        for (int py = 0; py < 4; py++)
        {
            // Get index into piece
            int pi = Rotate(px, py, nRotation);

            // Get index into field
            int fi = (nPosY + py) * fieldWidth + (nPosX + px);

            if (nPosX + px >= 0 && nPosX + px < fieldWidth)
            {
                if (nPosY + py >= 0 && nPosY + py < fieldHeight)
                {
                    // In Bounds so do collision check
                    if (tetromino[nTetromino][pi] != L'.' && field[fi] != 0)
                        return false; // fail on first hit
                }
            }
        }

    return true;
}

int main()
{

    InitWindow(screenWidth, screenHeight, "tetris");

    // these comments were made after i made the assets so i probably got sum wrong
    tetromino[0].append(L"..X...X...X...X."); // line
    tetromino[1].append(L"..X..XX...X....."); // Z Block
    tetromino[2].append(L".....XX..XX....."); // cube
    tetromino[3].append(L"..X..XX..X......"); // S block
    tetromino[4].append(L".X...XX...X....."); // T block
    tetromino[5].append(L".X...X...XX....."); // L block (maybe J)
    tetromino[6].append(L"..X...X..XX....."); // J block (maybe L)

    field = new unsigned char[fieldWidth * fieldHeight];
    for (int x = 0; x < fieldWidth; x++)
        for (int y = 0; y < fieldHeight; y++)
            field[y * fieldWidth + x] = (x == 0 || x == fieldWidth - 1 || y == fieldHeight - 1) ? 9 : 0;

    SetTargetFPS(50);

    while (!WindowShouldClose())
    {
        if (!gameOver)
        {
            BeginDrawing();

            // timing
            this_thread::sleep_for(50ms);
            speedCounter++;
            forceDown = (speedCounter == speed);
            // input

            currentX += (IsKeyDown(68) && DoesPieceFit(currentPiece, currentRotation, currentX + 1, currentY)) ? 1 : 0;
            currentX -= (IsKeyDown(65) && DoesPieceFit(currentPiece, currentRotation, currentX - 1, currentY)) ? 1 : 0;
            currentY += (IsKeyDown(83) && DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1)) ? 1 : 0;
            currentRotation += (IsKeyPressed(87) && DoesPieceFit(currentPiece, currentRotation + 1, currentX, currentY)) ? 1 : 0;

            if (forceDown)
            {
                if (DoesPieceFit(currentPiece, currentRotation, currentX, currentY + 1))
                    currentY++;
                else
                {

                    for (int px = 0; px < 4; px++)
                        for (int py = 0; py < 4; py++)
                            if (tetromino[currentPiece][Rotate(px, py, currentRotation)] == L'X')
                                field[(currentY + py) * fieldWidth + (currentX + px)] = currentPiece + 1;

                    // makes a line
                    for (int py = 0; py < 4; py++)
                        if (currentY + py < fieldHeight - 1)
                        {
                            bool bLine = true;
                            for (int px = 1; px < fieldWidth - 1; px++)
                                bLine &= (field[(currentY + py) * fieldWidth + px]) != 0;

                            if (bLine)
                            {
                                for (int px = 1; px < fieldWidth - 1; px++)
                                    field[(currentY + py) * fieldWidth + px] = 8;
                                vLines.push_back(currentY + py);
                            }
                        }

                    Score += 25;
                    if (!vLines.empty())
                        Score += (1 << vLines.size()) * 100;

                    currentY = 1;
                    currentX = 3;
                    currentRotation = 0;
                    currentPiece = rand() % 7;

                    gameOver = !DoesPieceFit(currentPiece, currentRotation, currentX, currentY);
                }
                speedCounter = 0;
            }

            ClearBackground(BLACK);
            // grid
            for (int x = 0; x < fieldWidth; x++)
            {
                for (int y = 0; y < fieldHeight; y++)
                {
                    int id = field[y * fieldWidth + x];
                    DrawRectangle(x * 20 + 2, y * 20 + 2, 20 - 2, 20 - 2, colors[id]);
                }
            }

            if (!vLines.empty())
            {
                this_thread::sleep_for(400ms);

                for (auto &v : vLines)
                    for (int px = 1; px < fieldWidth - 1; px++)
                    {
                        for (int py = v; py > 0; py--)
                            field[py * fieldWidth + px] = field[(py - 1) * fieldWidth + px];
                        field[px] = 0;
                    }

                vLines.clear();
            }

            // piece
            for (int px = 0; px < 4; px++)
            {
                for (int py = 0; py < 4; py++)
                {
                    if (tetromino[currentPiece][Rotate(px, py, currentRotation)] != L'.')
                    {
                        DrawRectangle((currentX + px) * 20 + 2, (currentY + py) * 20 + 2, 20 - 2, 20 - 2, colors[currentPiece + 1]);
                    }
                }
            }



            DrawText(TextFormat("SCORE: %i", Score), 0, 0, 10, BLACK);

            EndDrawing();
        }
        else
        {
            CloseWindow();
        }
    }
    CloseWindow();
    return 0;
}