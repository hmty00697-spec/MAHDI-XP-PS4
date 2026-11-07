#include <graphics.h>
#include <unistd.h>

#define WIDTH 1920
#define HEIGHT 1080

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    Scene2D scene(WIDTH, HEIGHT, 4);

    if (!scene.Init(0xC000000, 2))
    {
        for (;;) {}
    }

    Color white = {255, 255, 255};
    Color blue = {0, 90, 220};

    for (;;)
    {
        scene.FrameBufferFill(white);

        // خطوط عمودی جدول
        scene.DrawRectangle(826, 140, 8, 800, blue);
        scene.DrawRectangle(1086, 140, 8, 800, blue);

        // خطوط افقی جدول
        scene.DrawRectangle(560, 400, 800, 8, blue);
        scene.DrawRectangle(560, 660, 800, 8, blue);

        scene.SubmitFlip(0);
        scene.FrameWait(0);
        scene.FrameBufferSwap();
    }

    return 0;
}
