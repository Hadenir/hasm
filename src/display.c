#include "display.h"

#include <conio.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "assembler.h"
#include "instruction.h"

int disp_init(struct virtual_machine* vm, struct program* program)
{
    display.vm_regs = malloc(16 * 4);
    display.vm_memory = malloc(vm->mem_sz);
    display.status = malloc(DISPLAY_WIDTH - 40);
    display.mem_scroll = 0;
    display.mem_max_scroll = vm->mem_sz / 16 - 10;
    display.code_scroll = 0;
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

    CONSOLE_CURSOR_INFO cursor_info;
    cursor_info.dwSize = 100;
    cursor_info.bVisible = FALSE;
    SetConsoleCursorInfo(display.console_handle, &cursor_info);

    disp_clear();

    return 0;
}

void disp_finilize()
{
    free(display.vm_regs);
    free(display.vm_memory);
}

void disp_status(const char* status)
{
    strcpy_s(display.status, DISPLAY_WIDTH - 40, status);
}

int disp_update(struct virtual_machine* vm, struct program* program)
{
    int action = -1;

    print_regs(vm);
    print_mem(vm);
    print_code(vm, program);

    // Waiting for user input.
    while(action < 0)
    {
        print_grid();

        int ch = getch();
        // printf("%u ", ch);
        switch(ch)
        {
            case ESCAPE_KEY:
                action = 0;
                break;
            case SPACE_KEY:
                action = 1;
                break;
            case RETURN_KEY:
                action = 2;
                break;
            case 'w':
                if(display.code_scroll > 0)
                {
                    display.code_scroll -= 1;
                    print_code(vm, program);
                }
                break;
            case 's':
                display.code_scroll += 1;
                print_code(vm, program);
                break;
            default:
                ch = getch();
                if(ch == ARROW_UP_KEY && display.mem_scroll > 0)
                {
                    display.mem_scroll -= 1;
                    print_mem(vm);
                }
                else if(ch == ARROW_DOWN_KEY && display.mem_scroll < display.mem_max_scroll)
                {
                    display.mem_scroll += 1;
                    print_mem(vm);
                }
                break;
        }
        Sleep(1);
    }

    update_internal_vm(vm);
    return action;
}

void update_internal_vm(struct virtual_machine* vm)
{
    memcpy(display.vm_regs, vm->regs, 16 * 4);
    memcpy(display.vm_memory, vm->memory, vm->mem_sz);
}

void print_grid()
{
    for(int y = 0; y < DISPLAY_HEIGHT - 1; ++y)
    {
        disp_cursor(CODE_BLOCK_WIDTH, y);
        putchar(219);
    }

    disp_cursor(CODE_BLOCK_WIDTH + 1, MEM_BLOCK_HEIGHT);
    for(int i = 0; i < DISPLAY_WIDTH - CODE_BLOCK_WIDTH - 1; ++i)
        putchar(254);

    // Scrollbars
    for(int y = 0; y < MEM_BLOCK_HEIGHT; ++y)
    {
        disp_cursor(DISPLAY_WIDTH - 1, y);
        putchar(' ');
    }
    disp_cursor(DISPLAY_WIDTH - 1, (MEM_BLOCK_HEIGHT - 1) * ((float) display.mem_scroll / display.mem_max_scroll));
    putchar(177);

    // Status bar
    DWORD written;
    COORD status_bar;
    status_bar.X = 0;
    status_bar.Y = DISPLAY_HEIGHT - 1;
    FillConsoleOutputAttribute(display.console_handle, HIGHLIGHT_COLOR, DISPLAY_WIDTH, status_bar, &written);

    disp_color(HIGHLIGHT_COLOR);
    disp_cursor(0, DISPLAY_HEIGHT - 1);
    printf(display.status);
    disp_cursor(DISPLAY_WIDTH - 40, DISPLAY_HEIGHT - 1);
    printf("SPACE=step  ENTER=continue  ESCAPE=exit");
    disp_color(DEFAULT_COLOR);
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
        // putchar(179);
    }

    // Vertical labels.
    for(int i = 0; i < 27; ++i)
    {
        disp_cursor(CODE_BLOCK_WIDTH + 4, i + 3);
        printf("%04X:", (i + display.mem_scroll) * 16);
    }

    // Memory contents.
    for(int i = 0; i < 27 * 16; ++i)
    {
        int x = i % 16;
        int y = i / 16;

        int idx = i + display.mem_scroll * 16;
        disp_cursor(CODE_BLOCK_WIDTH + 3 * x + 11, y + 3);
        if(idx < vm->mem_sz)
        {
            if(vm->memory[idx] != display.vm_memory[idx])
                disp_color(CHANGE_COLOR);
            printf("%02X", vm->memory[idx]);
            disp_color(DEFAULT_COLOR);
        }
        else
        {
            printf("   ");
        }
    }
}

void print_code(struct virtual_machine* vm, struct program* program)
{
    struct source_code* curr_line = program->source;
    for(int i = 0; i < display.code_scroll && curr_line != NULL; ++i)
        curr_line = curr_line->next;

    for(int line = display.code_scroll; line < DISPLAY_HEIGHT + display.code_scroll - 1; ++line)
    {
        disp_cursor(0, line - display.code_scroll);
        printf("%4d", line + 1);
        putchar(179);

        if(curr_line == NULL)
        {
            printf("%-70s", "");
            continue;
        }

        if(curr_line->addr == vm->pc)
            disp_color(HIGHLIGHT_COLOR);

        printf(" 0x%04x ", curr_line->addr);
        printf("%-70s", curr_line->text);

        disp_color(DEFAULT_COLOR);

        curr_line = curr_line->next;
    }
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
