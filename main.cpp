#include <stdio.h>
#include <sstream>
#include "../_common/graphics.h"

// سیستم گزارش خطا
std::stringstream debugLogStream;

// تنظیمات تصویر
#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define FRAME_DEPTH 4

int frameID = 0;

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    // ساخت صفحه گرافیکی
    Scene2D *scene = new Scene2D(
        FRAME_WIDTH,
        FRAME_HEIGHT,
        FRAME_DEPTH
    );

    // راه‌اندازی تصویر
    if (!scene->Init(0xC000000, 2))
    {
        for (;;)
        {
        }
    }

    // رنگ‌ها
    Color white = {255, 255, 255};
    Color blue = {0, 90, 220};

    // حلقه اصلی نمایش
    for (;;)
    {
        // پس‌زمینه سفید
        scene->FrameBufferFill(white);

        // دو خط عمودی
        scene->DrawRectangle(
            826, 140, 8, 800, blue
        );

        scene->DrawRectangle(
            1086, 140, 8, 800, blue
        );

        // دو خط افقی
        scene->DrawRectangle(
            560, 400, 800, 8, blue
        );

        scene->DrawRectangle(
            560, 660, 800, 8, blue
        );

        // نمایش فریم
        scene->SubmitFlip(frameID);
        scene->FrameWait(frameID);
        scene->FrameBufferSwap();

        frameID++;
    }

    return 0;
}
