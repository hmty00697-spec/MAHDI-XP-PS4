
#include <stdio.h>
#include <string.h>

#include "graphics.h"
#include <orbis/Pad.h>
#include <orbis/UserService.h>

#define MAX_LOGIN_USERS_LOCAL 4
#define INVALID_USER_ID_LOCAL (-1)

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define FRAME_DEPTH 4

int frameID = 0;

// ---------------- COLORS ----------------

Color white     = {255, 255, 255};
Color blue      = {0, 90, 220};
Color black     = {15, 15, 15};
Color red       = {220, 35, 35};
Color highlight = {0, 170, 110};

// ---------------- FONT ----------------

void getGlyph(char c, uint8_t p[7])
{
    memset(p, 0, 7);

    switch (c)
    {
        case 'A': { uint8_t a[]={14,17,17,31,17,17,17}; memcpy(p,a,7); break; }
        case 'B': { uint8_t a[]={30,17,17,30,17,17,30}; memcpy(p,a,7); break; }
        case 'C': { uint8_t a[]={14,17,16,16,16,17,14}; memcpy(p,a,7); break; }
        case 'D': { uint8_t a[]={30,17,17,17,17,17,30}; memcpy(p,a,7); break; }
        case 'E': { uint8_t a[]={31,16,16,30,16,16,31}; memcpy(p,a,7); break; }
        case 'G': { uint8_t a[]={14,17,16,23,17,17,15}; memcpy(p,a,7); break; }
        case 'I': { uint8_t a[]={31,4,4,4,4,4,31}; memcpy(p,a,7); break; }
        case 'L': { uint8_t a[]={16,16,16,16,16,16,31}; memcpy(p,a,7); break; }
        case 'M': { uint8_t a[]={17,27,21,21,17,17,17}; memcpy(p,a,7); break; }
        case 'N': { uint8_t a[]={17,25,25,21,19,19,17}; memcpy(p,a,7); break; }
        case 'O': { uint8_t a[]={14,17,17,17,17,17,14}; memcpy(p,a,7); break; }
        case 'P': { uint8_t a[]={30,17,17,30,16,16,16}; memcpy(p,a,7); break; }
        case 'R': { uint8_t a[]={30,17,17,30,20,18,17}; memcpy(p,a,7); break; }
        case 'S': { uint8_t a[]={15,16,16,14,1,1,30}; memcpy(p,a,7); break; }
        case 'T': { uint8_t a[]={31,4,4,4,4,4,4}; memcpy(p,a,7); break; }
        case 'U': { uint8_t a[]={17,17,17,17,17,17,14}; memcpy(p,a,7); break; }
        case 'W': { uint8_t a[]={17,17,17,21,21,21,10}; memcpy(p,a,7); break; }
        case 'X': { uint8_t a[]={17,17,10,4,10,17,17}; memcpy(p,a,7); break; }
        case 'Y': { uint8_t a[]={17,17,10,4,4,4,4}; memcpy(p,a,7); break; }

        case '1': { uint8_t a[]={4,12,4,4,4,4,14}; memcpy(p,a,7); break; }
        case '2': { uint8_t a[]={14,17,1,2,4,8,31}; memcpy(p,a,7); break; }
        case '.': p[6] = 4; break;
        default: break;
    }
}

void drawText(Scene2D *scene, const char *text,
              int x, int y, int scale, Color color)
{
    for (int n = 0; text[n] != '\0'; n++)
    {
        uint8_t p[7];
        getGlyph(text[n], p);

        for (int row = 0; row < 7; row++)
        {
            for (int col = 0; col < 5; col++)
            {
                if (p[row] & (1 << (4 - col)))
                {
                    scene->DrawRectangle(
                        x + n * 6 * scale + col * scale,
                        y + row * scale,
                        scale, scale, color
                    );
                }
            }
        }
    }
}

void drawCenteredText(Scene2D *scene, const char *text,
                      int y, int scale, Color color)
{
    int width = (int)strlen(text) * 6 * scale;
    int x = (FRAME_WIDTH - width) / 2;

    drawText(scene, text, x, y, scale, color);
}

// ---------------- GAME BOARD ----------------

int board[9] = {0};

int cursor = 4;
int currentPlayer = 1;
int winner = 0;
bool gameOver = false;

int cellX[3] = {560, 826, 1086};
int cellY[3] = {140, 400, 660};

int cellW[3] = {266, 260, 274};
int cellH[3] = {260, 260, 280};

void resetGame()
{
    for (int i = 0; i < 9; i++)
        board[i] = 0;

    cursor = 4;
    currentPlayer = 1;
    winner = 0;
    gameOver = false;
}

int checkWinner()
{
    const int lines[8][3] =
    {
        {0,1,2}, {3,4,5}, {6,7,8},
        {0,3,6}, {1,4,7}, {2,5,8},
        {0,4,8}, {2,4,6}
    };

    for (int i = 0; i < 8; i++)
    {
        int a = lines[i][0];
        int b = lines[i][1];
        int c = lines[i][2];

        if (board[a] != 0 &&
            board[a] == board[b] &&
            board[b] == board[c])
        {
            return board[a];
        }
    }

    return 0;
}

bool boardFull()
{
    for (int i = 0; i < 9; i++)
        if (board[i] == 0)
            return false;

    return true;
}

// ---------------- DRAW MARKS ----------------

void drawX(Scene2D *scene, int cx, int cy, Color color)
{
    for (int i = 0; i <= 100; i += 5)
    {
        scene->DrawRectangle(
            cx - 52 + i,
            cy - 52 + i,
            12, 12, color
        );

        scene->DrawRectangle(
            cx + 40 - i,
            cy - 52 + i,
            12, 12, color
        );
    }
}

void drawO(Scene2D *scene, int cx, int cy, Color color)
{
    int outer = 55;
    int inner = 39;

    for (int y = -outer; y <= outer; y++)
    {
        int outerX = 0;

        while ((outerX + 1) * (outerX + 1) + y * y
               <= outer * outer)
            outerX++;

        int innerX = -1;

        if (y * y < inner * inner)
        {
            innerX = 0;

            while ((innerX + 1) * (innerX + 1) + y * y
                   <= inner * inner)
                innerX++;
        }

        if (innerX < 0)
        {
            scene->DrawRectangle(
                cx - outerX, cy + y,
                outerX * 2 + 1, 2, color
            );
        }
        else
        {
            scene->DrawRectangle(
                cx - outerX, cy + y,
                outerX - innerX, 2, color
            );

            scene->DrawRectangle(
                cx + innerX + 1, cy + y,
                outerX - innerX, 2, color
            );
        }
    }
}

// ---------------- DRAW BOARD ----------------

void drawBoard(Scene2D *scene)
{
    scene->FrameBufferFill(white);

    drawText(scene, "TIC TAC TOE", 696, 55, 8, black);

    // Grid lines
    scene->DrawRectangle(560, 140, 800, 8, blue);
    scene->DrawRectangle(560, 932, 800, 8, blue);

    scene->DrawRectangle(560, 140, 8, 800, blue);
    scene->DrawRectangle(1352, 140, 8, 800, blue);

    scene->DrawRectangle(826, 140, 8, 800, blue);
    scene->DrawRectangle(1086, 140, 8, 800, blue);

    scene->DrawRectangle(560, 400, 800, 8, blue);
    scene->DrawRectangle(560, 660, 800, 8, blue);

    // Selected cell
    if (!gameOver)
    {
        int row = cursor / 3;
        int col = cursor % 3;

        int x = cellX[col] + 12;
        int y = cellY[row] + 12;

        int w = cellW[col] - 24;
        int h = cellH[row] - 24;
        int t = 6;

        scene->DrawRectangle(x, y, w, t, highlight);
        scene->DrawRectangle(x, y + h - t, w, t, highlight);
        scene->DrawRectangle(x, y, t, h, highlight);
        scene->DrawRectangle(x + w - t, y, t, h, highlight);
    }

    // Marks
    for (int i = 0; i < 9; i++)
    {
        if (board[i] == 0)
            continue;

        int row = i / 3;
        int col = i % 3;

        int cx = cellX[col] + cellW[col] / 2;
        int cy = cellY[row] + cellH[row] / 2;

        if (board[i] == 1)
            drawX(scene, cx, cy, red);
        else
            drawO(scene, cx, cy, black);
    }

    // Status
    if (winner == 1)
        drawCenteredText(scene, "P1 WINS", 965, 4, red);
    else if (winner == 2)
        drawCenteredText(scene, "P2 WINS", 965, 4, red);
    else if (gameOver)
        drawCenteredText(scene, "DRAW GAME", 965, 4, black);
    else if (currentPlayer == 1)
        drawCenteredText(scene, "P1 TURN", 965, 4, red);
    else
        drawCenteredText(scene, "P2 TURN", 965, 4, black);

    drawCenteredText(scene, "MAHDI.0P", 1030, 3, red);
}

// ---------------- CONTROLLERS ----------------

int padHandles[2] = {-1, -1};
int padCount = 0;

OrbisPadData padData[2];
uint32_t previousButtons[2] = {0, 0};
uint32_t pressedButtons[2] = {0, 0};

void initializeControllers()
{
    OrbisUserServiceInitializeParams params;
    params.priority = ORBIS_KERNEL_PRIO_FIFO_LOWEST;

    sceUserServiceInitialize(&params);

    if (scePadInit() != 0)
        return;

    OrbisUserServiceLoginUserIdList users;

    for (int i = 0; i < ORBIS_USER_SERVICE_MAX_LOGIN_USERS; i++)
        users.userId[i] = ORBIS_USER_SERVICE_USER_ID_INVALID;

    if (sceUserServiceGetLoginUserIdList(&users) == 0)
    {
        for (int i = 0;
             i < ORBIS_USER_SERVICE_MAX_LOGIN_USERS && padCount < 2;
             i++)
        {
            int userID = users.userId[i];

            if (userID == ORBIS_USER_SERVICE_USER_ID_INVALID)
                continue;

            int handle = scePadOpen(userID, 0, 0, NULL);

            if (handle >= 0)
                padHandles[padCount++] = handle;
        }
    }

    // Fallback: try the initial user if the list was unavailable.
    if (padCount == 0)
    {
        int userID = ORBIS_USER_SERVICE_USER_ID_INVALID;

        if (sceUserServiceGetInitialUser(&userID) == 0 &&
            userID != ORBIS_USER_SERVICE_USER_ID_INVALID)
        {
            int handle = scePadOpen(userID, 0, 0, NULL);

            if (handle >= 0)
                padHandles[padCount++] = handle;
        }
    }
}

void readControllers()
{
    for (int i = 0; i < 2; i++)
    {
        pressedButtons[i] = 0;

        if (i >= padCount)
            continue;

        if (scePadReadState(padHandles[i], &padData[i]) == 0)
        {
            uint32_t current = padData[i].buttons;

            pressedButtons[i] = current & ~previousButtons[i];
            previousButtons[i] = current;
        }
        else
        {
            previousButtons[i] = 0;
        }
    }
}

// ---------------- INPUT ----------------

void moveCursor(int direction)
{
    int row = cursor / 3;
    int col = cursor % 3;

    if (direction == ORBIS_PAD_BUTTON_UP)
        row = (row + 2) % 3;

    if (direction == ORBIS_PAD_BUTTON_DOWN)
        row = (row + 1) % 3;

    if (direction == ORBIS_PAD_BUTTON_LEFT)
        col = (col + 2) % 3;

    if (direction == ORBIS_PAD_BUTTON_RIGHT)
        col = (col + 1) % 3;

    cursor = row * 3 + col;
}

void processInput()
{
    if (padCount == 0)
        return;

    // Options on either controller restarts the game.
    for (int i = 0; i < padCount; i++)
    {
        if (pressedButtons[i] & ORBIS_PAD_BUTTON_OPTIONS)
        {
            resetGame();
            return;
        }
    }

    if (gameOver)
        return;

    // With two logged-in users, each player uses their own pad.
    // With one pad, both players take turns on that controller.
    int activePad = (padCount >= 2) ? currentPlayer - 1 : 0;

    uint32_t buttons = pressedButtons[activePad];

    if (buttons & ORBIS_PAD_BUTTON_UP)
        moveCursor(ORBIS_PAD_BUTTON_UP);

    else if (buttons & ORBIS_PAD_BUTTON_DOWN)
        moveCursor(ORBIS_PAD_BUTTON_DOWN);

    else if (buttons & ORBIS_PAD_BUTTON_LEFT)
        moveCursor(ORBIS_PAD_BUTTON_LEFT);

    else if (buttons & ORBIS_PAD_BUTTON_RIGHT)
        moveCursor(ORBIS_PAD_BUTTON_RIGHT);

    if (buttons & ORBIS_PAD_BUTTON_CROSS)
    {
        if (board[cursor] == 0)
        {
            board[cursor] = currentPlayer;

            winner = checkWinner();

            if (winner != 0)
            {
                gameOver = true;
            }
            else if (boardFull())
            {
                gameOver = true;
            }
            else
            {
                currentPlayer = (currentPlayer == 1) ? 2 : 1;
            }
        }
    }
}

// ---------------- MAIN ----------------

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    Scene2D *scene = new Scene2D(
        FRAME_WIDTH,
        FRAME_HEIGHT,
        FRAME_DEPTH
    );

    if (!scene->Init(0xC000000, 2))
    {
        for (;;) {}
    }

    // Start with framebuffer zero.
    scene->SetActiveFrameBuffer(0);

    initializeControllers();

    for (;;)
    {
        readControllers();
        processInput();

        drawBoard(scene);

        if (padCount == 0)
        {
            drawCenteredText(scene, "CONNECT PAD", 1000, 3, black);
        }
        else if (padCount == 1)
        {
            drawCenteredText(scene, "ONE PAD MODE", 1000, 3, black);
        }
        else
        {
            drawCenteredText(scene, "2 PADS READY", 1000, 3, black);
        }

        scene->SubmitFlip(frameID);
        scene->FrameWait(frameID);
        scene->FrameBufferSwap();

        frameID++;
    }

    return 0;
}
