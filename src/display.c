#include "display.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int disp_init(struct virtual_machine* vm)
{
    display.vm_regs = malloc(16 * 4);
    display.vm_memory = malloc(vm->mem_sz);
    update_internal_vm(vm);

    display.console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD size;
    size.X = DISPLAY_WIDTH;
    size.Y = DISPLAY_HEIGHT;

    SMALL_RECT rect;
    rect.Top = 0;
    rect.Left = 0;
    rect.Right = DISPLAY_WIDTH - 1;
    rect.Bottom = DISPLAY_HEIGHT - 1;

    SetConsoleScreenBufferSize(display.console_handle, size);
    SetConsoleWindowInfo(display.console_handle, TRUE, &rect);

    disp_clear();

    // Prepare user interface.

    for(int y = 0; y < DISPLAY_HEIGHT - 1; ++y)
    {
        disp_cursor(CODE_BLOCK_WIDTH, y);
        putchar(219);
    }

    disp_cursor(CODE_BLOCK_WIDTH + 1, MEM_BLOCK_HEIGHT);
    for(int i = 0; i < DISPLAY_WIDTH - CODE_BLOCK_WIDTH - 1; ++i)
        putchar(254);

    // Scrollbars
    disp_cursor(CODE_BLOCK_WIDTH - 1, 0);
    putchar(177);
    disp_cursor(DISPLAY_WIDTH - 1, 0);
    putchar(177);

    // Status bar
    DWORD written;
    COORD status_bar;
    status_bar.X = 0;
    status_bar.Y = DISPLAY_HEIGHT - 1;
    FillConsoleOutputAttribute(display.console_handle, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE, DISPLAY_WIDTH, status_bar, &written);

    disp_cursor(0, 0);

    return 0;
}

void disp_finilize()
{


    // TODO: disp_clear();
}

int disp_update(struct virtual_machine* vm)
{
    print_regs(vm);
    print_mem(vm);
    print_code(vm);

    update_internal_vm(vm);

    disp_cursor(0, 0);

    return 1;
}

void update_internal_vm(struct virtual_machine* vm)
{
    memcpy(display.vm_regs, vm->regs, 16 * 4);
    memcpy(display.vm_memory, vm->memory, vm->mem_sz);
}

void print_regs(struct virtual_machine* vm)
{
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            int reg = j + 4 * i;

            if(vm->regs[reg] != display.vm_regs[reg])
                disp_color(CHANGE_COLOR);

            disp_cursor(CODE_BLOCK_WIDTH + 15 * i + 2, MEM_BLOCK_HEIGHT + 4 * j + 2);
            putchar(201);
            for(int k = 0; k < 12; ++k)
                putchar(205);
            putchar(187);

            disp_cursor(CODE_BLOCK_WIDTH + 15 * i + 2, MEM_BLOCK_HEIGHT + 4 * j + 3);
            putchar(186);
            printf("r%02u %08X", reg, vm->regs[reg]);
            putchar(186);

            disp_cursor(CODE_BLOCK_WIDTH + 15 * i + 2, MEM_BLOCK_HEIGHT + 4 * j + 4);
            putchar(200);
            for(int k = 0; k < 12; ++k)
                putchar(205);
            putchar(188);

            disp_color(DEFAULT_COLOR);
        }
    }
}

void print_mem(struct virtual_machine* vm)
{
    // Horizontal labels.
    for(int i = 0; i < 16; ++i)
    {
        disp_cursor(CODE_BLOCK_WIDTH + 3 * i + 11, 1);
        printf("%02X", i);
        putchar(179);
    }

    // Vertical labels.
    for(int i = 0; i < 27; ++i)
    {
        disp_cursor(CODE_BLOCK_WIDTH + 4, i + 3);
        printf("%04X:", i * 16);
    }

    // Memory contents.
    for(int i = 0; i < vm->mem_sz && i < 27 * 16; ++i)
    {
        int x = i % 16;
        int y = i / 16;

        disp_cursor(CODE_BLOCK_WIDTH + 3 * x + 11, y + 3);
        if(vm->memory[i] != display.vm_memory[i])
            disp_color(CHANGE_COLOR);
        printf("%02X", vm->memory[i]);

        disp_color(DEFAULT_COLOR);
    }
}

void print_code(struct virtual_machine* vm)
{

}

void disp_clear()
{
    COORD cursor;
    cursor.X = 0;
    cursor.Y = 0;

    DWORD written;
    SetConsoleCursorPosition(display.console_handle, cursor);
    FillConsoleOutputCharacter(display.console_handle, ' ', DISPLAY_WIDTH * DISPLAY_HEIGHT, cursor, &written);
    FillConsoleOutputAttribute(display.console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, DISPLAY_WIDTH * DISPLAY_HEIGHT, cursor, &written);
}

void disp_cursor(int column, int row)
{
    COORD cursor;
    cursor.X = column;
    cursor.Y = row;

    SetConsoleCursorPosition(display.console_handle, cursor);
}

void disp_color(int color)
{
    SetConsoleTextAttribute(display.console_handle, color);
}
