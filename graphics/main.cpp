#include <stdio.h>
#include "graphics.h"

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define FRAME_DEPTH 4

int frameID = 0;

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

    Color white = {255, 255, 255};
    Color blue = {0, 90, 220};

    for (;;)
    {
        scene->FrameBufferFill(white);

        // Vertical lines
        scene->DrawRectangle(826, 140, 8, 800, blue);
        scene->DrawRectangle(1086, 140, 8, 800, blue);

        // Horizontal lines
        scene->DrawRectangle(560, 400, 800, 8, blue);
        scene->DrawRectangle(560, 660, 800, 8, blue);

        scene->SubmitFlip(frameID);
        scene->FrameWait(frameID);
        scene->FrameBufferSwap();

        frameID++;
    }

    return 0;
}
