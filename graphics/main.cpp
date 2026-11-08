
#include <stdio.h>
#include <sstream>
#include "graphics.h"

std::stringstream debugLogStream;

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define FRAME_DEPTH 4

int frameID = 0;

void drawText(Scene2D *scene, const char *text,
              int x, int y, int scale, Color color)
{
    for (int n = 0; text[n] != '\0'; n++)
    {
        uint8_t p[7] = {0};

        switch (text[n])
        {
            case 'T': { uint8_t a[]={31,4,4,4,4,4,4}; for(int i=0;i<7;i++)p[i]=a[i]; break; }
            case 'I': { uint8_t a[]={31,4,4,4,4,4,31}; for(int i=0;i<7;i++)p[i]=a[i]; break; }
            case 'C': { uint8_t a[]={14,17,16,16,16,17,14}; for(int i=0;i<7;i++)p[i]=a[i]; break; }
            case 'A': { uint8_t a[]={14,17,17,31,17,17,17}; for(int i=0;i<7;i++)p[i]=a[i]; break; }
            case 'O': { uint8_t a[]={14,17,17,17,17,17,14}; for(int i=0;i<7;i++)p[i]=a[i]; break; }
            case 'E': { uint8_t a[]={31,16,16,30,16,16,31}; for(int i=0;i<7;i++)p[i]=a[i]; break; }
            case 'M': { uint8_t a[]={17,27,21,21,17,17,17}; for(int i=0;i<7;i++)p[i]=a[i]; break; }
            case 'H': { uint8_t a[]={17,17,17,31,17,17,17}; for(int i=0;i<7;i++)p[i]=a[i]; break; }
            case 'D': { uint8_t a[]={30,17,17,17,17,17,30}; for(int i=0;i<7;i++)p[i]=a[i]; break; }
            case 'P': { uint8_t a[]={30,17,17,30,16,16,16}; for(int i=0;i<7;i++)p[i]=a[i]; break; }
            case '0': { uint8_t a[]={14,17,19,21,25,17,14}; for(int i=0;i<7;i++)p[i]=a[i]; break; }
            case '.': p[6] = 4; break;
            default: break;
        }

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
    Color black = {15, 15, 15};
    Color red = {220, 35, 35};

    for (;;)
    {
        scene->FrameBufferFill(white);

        drawText(scene, "TIC TAC TOE", 696, 55, 8, black);

        scene->DrawRectangle(560, 140, 800, 8, blue);
        scene->DrawRectangle(560, 932, 800, 8, blue);

        scene->DrawRectangle(560, 140, 8, 800, blue);
        scene->DrawRectangle(1352, 140, 8, 800, blue);

        scene->DrawRectangle(826, 140, 8, 800, blue);
        scene->DrawRectangle(1086, 140, 8, 800, blue);

        scene->DrawRectangle(560, 400, 800, 8, blue);
        scene->DrawRectangle(560, 660, 800, 8, blue);

        drawText(scene, "MAHDI.0P", 864, 975, 6, red);

        scene->SubmitFlip(frameID);
        scene->FrameWait(frameID);
        scene->FrameBufferSwap();

        frameID++;
    }

    return 0;
}
