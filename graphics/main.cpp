#include <stdio.h>
#include <string.h>
#include <graphics.h>
#include <log.h>

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define FRAME_DEPTH 4

int frameID = 0;

void drawRect(Scene2D *scene, int x, int y,
              int width, int height, Color color)
{
    for (int py = y; py < y + height; py++)
    {
        for (int px = x; px < x + width; px++)
        {
            scene->DrawPixel(px, py, color);
        }
    }
}

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    Scene2D *scene =
        new Scene2D(FRAME_WIDTH, FRAME_HEIGHT, FRAME_DEPTH);

    if (!scene->Init(0xC000000, 2))
    {
        for (;;) {}
    }

    Color white = {255, 255, 255};
    Color blue = {0, 90, 220};

    for (;;)
    {
        scene->FrameBufferClear();

        drawRect(scene, 826, 140, 8, 800, blue);
        drawRect(scene, 1086, 140, 8, 800, blue);

        drawRect(scene, 560, 400, 800, 8, blue);
        drawRect(scene, 560, 660, 800, 8, blue);

        scene->SubmitFlip(frameID);
        scene->FrameWait(frameID);
        scene->FrameBufferSwap();

        frameID++;
    }

    return 0;
}
