
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <string>

#include "graphics.h"
#include "log.h"

std::stringstream debugLogStream;

Scene2D::Scene2D(int w, int h, int pixelDepth)
{
    this->width = w;
    this->height = h;
    this->depth = pixelDepth;

    this->video = -1;
    this->directMemOff = 0;
    this->directMemAllocationSize = 0;
    this->videoMemSP = 0;
    this->videoMem = NULL;
    this->frameBuffers = NULL;
    this->flipQueue = 0;

    this->frameBufferSize = w * h * pixelDepth;
    this->frameBufferCount = 0;
    this->activeFrameBufferIdx = 0;
}

bool Scene2D::Init(size_t memSize, int numFrameBuffers)
{
    int rc;

    if (memSize == 0 || numFrameBuffers <= 0 ||
        this->frameBufferSize <= 0)
        return false;

    this->video = sceVideoOutOpen(
        ORBIS_VIDEO_USER_MAIN,
        ORBIS_VIDEO_OUT_BUS_MAIN,
        0,
        0
    );

    if (this->video < 0)
    {
        DEBUGLOG << "Failed to open video output: "
                 << std::string(strerror(errno));
        return false;
    }

#ifdef GRAPHICS_USES_FONT
    rc = sceSysmoduleLoadModule(ORBIS_SYSMODULE_FREETYPE_OL);

    if (rc < 0)
        return false;

    rc = FT_Init_FreeType(&this->ftLib);

    if (rc != 0)
        return false;
#endif

    if (!initFlipQueue())
        return false;

    if (!allocateVideoMem(memSize, 0x200000))
        return false;

    if (!allocateFrameBuffers(numFrameBuffers))
        return false;

    sceVideoOutSetFlipRate(this->video, 0);

    return true;
}

bool Scene2D::initFlipQueue()
{
    int rc = sceKernelCreateEqueue(
        &flipQueue,
        "homebrew flip queue"
    );

    if (rc < 0)
        return false;

    rc = sceVideoOutAddFlipEvent(
        flipQueue,
        this->video,
        0
    );

    return rc == 0;
}

bool Scene2D::allocateFrameBuffers(int num)
{
    if (num <= 0 ||
        this->videoMem == NULL ||
        this->video < 0)
        return false;

    size_t requiredMemory =
        (size_t)num * (size_t)this->frameBufferSize;

    if (requiredMemory > this->directMemAllocationSize)
        return false;

    this->frameBuffers = new char*[num]();

    if (this->frameBuffers == NULL)
        return false;

    for (int i = 0; i < num; i++)
    {
        this->frameBuffers[i] =
            this->allocateDisplayMem(this->frameBufferSize);

        if (this->frameBuffers[i] == NULL)
            return false;
    }

    sceVideoOutSetBufferAttribute(
        &this->attr,
        0x80000000,
        1,
        0,
        this->width,
        this->height,
        this->width
    );

    if (sceVideoOutRegisterBuffers(
        this->video,
        0,
        (void **)this->frameBuffers,
        num,
        &this->attr
    ) != 0)
        return false;

    this->frameBufferCount = num;
    this->activeFrameBufferIdx = 0;

    return true;
}

char *Scene2D::allocateDisplayMem(size_t size)
{
    if (this->videoMem == NULL || size == 0)
        return NULL;

    char *allocatedPtr = (char *)this->videoMemSP;

    this->videoMemSP += size;

    return allocatedPtr;
}

bool Scene2D::allocateVideoMem(size_t size, int alignment)
{
    int rc;

    if (size == 0 || alignment <= 0)
        return false;

    this->directMemAllocationSize =
        (size + alignment - 1) / alignment * alignment;

    rc = sceKernelAllocateDirectMemory(
        0,
        sceKernelGetDirectMemorySize(),
        this->directMemAllocationSize,
        alignment,
        3,
        &this->directMemOff
    );

    if (rc < 0)
    {
        this->directMemAllocationSize = 0;
        return false;
    }

    rc = sceKernelMapDirectMemory(
        &this->videoMem,
        this->directMemAllocationSize,
        0x33,
        0,
        this->directMemOff,
        alignment
    );

    if (rc < 0)
    {
        sceKernelReleaseDirectMemory(
            this->directMemOff,
            this->directMemAllocationSize
        );

        this->directMemOff = 0;
        this->directMemAllocationSize = 0;

        return false;
    }

    this->videoMemSP = (uintptr_t)this->videoMem;

    return true;
}

void Scene2D::deallocateVideoMem()
{
    if (this->directMemAllocationSize > 0)
    {
        sceKernelReleaseDirectMemory(
            this->directMemOff,
            this->directMemAllocationSize
        );
    }

    this->videoMem = NULL;
    this->videoMemSP = 0;
    this->directMemOff = 0;
    this->directMemAllocationSize = 0;

    delete[] this->frameBuffers;
    this->frameBuffers = NULL;
    this->frameBufferCount = 0;
}

void Scene2D::SetActiveFrameBuffer(int index)
{
    if (index < 0 || index >= this->frameBufferCount)
        return;

    this->activeFrameBufferIdx = index;
}

void Scene2D::SubmitFlip(int frameID)
{
    if (this->video < 0 ||
        this->frameBuffers == NULL ||
        this->activeFrameBufferIdx < 0 ||
        this->activeFrameBufferIdx >= this->frameBufferCount)
        return;

    sceVideoOutSubmitFlip(
        this->video,
        this->activeFrameBufferIdx,
        ORBIS_VIDEO_OUT_FLIP_VSYNC,
        frameID
    );
}

void Scene2D::FrameWait(int frameID)
{
    if (this->video < 0)
        return;

    OrbisKernelEvent evt;
    int count;

    for (;;)
    {
        OrbisVideoOutFlipStatus flipStatus;

        sceVideoOutGetFlipStatus(video, &flipStatus);

        if (flipStatus.flipArg == frameID)
            break;

        if (sceKernelWaitEqueue(
                this->flipQueue,
                &evt,
                1,
                &count,
                0
            ) != 0)
            break;
    }
}

void Scene2D::FrameBufferSwap()
{
    if (this->frameBufferCount <= 0)
        return;

    this->activeFrameBufferIdx =
        (this->activeFrameBufferIdx + 1) %
        this->frameBufferCount;
}

void Scene2D::FrameBufferClear()
{
    Color blank = {255, 255, 255};
    FrameBufferFill(blank);
}

#ifdef GRAPHICS_USES_FONT
bool Scene2D::InitFont(
    FT_Face *face,
    const char *fontPath,
    int fontSize
)
{
    int rc;

    rc = FT_New_Face(
        this->ftLib,
        fontPath,
        0,
        face
    );

    if (rc < 0)
        return false;

    rc = FT_Set_Pixel_Sizes(*face, 0, fontSize);

    if (rc < 0)
        return false;

    return true;
}
#endif

void Scene2D::FrameBufferFill(Color color)
{
    DrawRectangle(
        0,
        0,
        this->width,
        this->height,
        color
    );
}

void Scene2D::DrawPixel(int x, int y, Color color)
{
    if (x < 0 || x >= this->width ||
        y < 0 || y >= this->height)
        return;

    if (this->frameBuffers == NULL ||
        this->activeFrameBufferIdx < 0 ||
        this->activeFrameBufferIdx >= this->frameBufferCount)
        return;

    if (this->frameBuffers[this->activeFrameBufferIdx] == NULL)
        return;

    int pixel = (y * this->width) + x;

    uint32_t encodedColor =
        0x80000000 +
        (color.r << 16) +
        (color.g << 8) +
        color.b;

    ((uint32_t *)this->frameBuffers[
        this->activeFrameBufferIdx
    ])[pixel] = encodedColor;
}

void Scene2D::DrawRectangle(
    int x,
    int y,
    int w,
    int h,
    Color color
)
{
    if (w <= 0 || h <= 0)
        return;

    int xPos, yPos;

    for (yPos = y; yPos < y + h; yPos++)
    {
        for (xPos = x; xPos < x + w; xPos++)
        {
            DrawPixel(xPos, yPos, color);
        }
    }
}

#ifdef GRAPHICS_USES_FONT
void Scene2D::DrawText(
    char *txt,
    FT_Face face,
    int startX,
    int startY,
    Color bgColor,
    Color fgColor
)
{
    int rc;
    int xOffset = 0;
    int yOffset = 0;

    FT_GlyphSlot slot = face->glyph;

    for (int n = 0; n < strlen(txt); n++)
    {
        FT_UInt glyph_index;

        glyph_index = FT_Get_Char_Index(face, txt[n]);

        rc = FT_Load_Glyph(
            face,
            glyph_index,
            FT_LOAD_DEFAULT
        );

        if (rc)
            continue;

        rc = FT_Render_Glyph(
            slot,
            ft_render_mode_normal
        );

        if (rc)
            continue;

        if (txt[n] == '\n')
        {
            xOffset = 0;
            yOffset += slot->bitmap.width * 2;
            continue;
        }

        for (int yPos = 0; yPos < slot->bitmap.rows; yPos++)
        {
            for (int xPos = 0; xPos < slot->bitmap.width; xPos++)
            {
                char pixel = slot->bitmap.buffer[
                    (yPos * slot->bitmap.width) + xPos
                ];

                int x = startX + xPos + xOffset + slot->bitmap_left;
                int y = startY + yPos + yOffset - slot->bitmap_top;

                uint8_t r = (pixel * fgColor.r) / 255;
                uint8_t g = (pixel * fgColor.g) / 255;
                uint8_t b = (pixel * fgColor.b) / 255;

                Color finalColor = {r, g, b};

                if (x < 0 || y < 0 ||
                    x >= this->width || y >= this->height)
                    continue;

                if (pixel != 0x00)
                    this->DrawPixel(x, y, finalColor);
            }
        }

        xOffset += slot->advance.x >> 6;
    }
}
#endif
