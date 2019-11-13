#include "display.h"

#include <windows.h>

int disp_init()
{
    COORD size;
    size.X = DISPLAY_WIDTH;
    size.Y = DISPLAY_HEIGHT;

    SMALL_RECT rect;
    rect.Top = 0;
    rect.Left = 0;
    rect.Right = DISPLAY_WIDTH - 1;
    rect.Bottom = DISPLAY_HEIGHT - 1;

    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleScreenBufferSize(console, size);
    SetConsoleWindowInfo(console, TRUE, &rect);

    return 0;
}
