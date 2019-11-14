#pragma once

#include "virtual_machine.h"

#define DISPLAY_WIDTH 150
#define DISPLAY_HEIGHT 50

#define CODE_BLOCK_WIDTH 88
#define MEM_BLOCK_HEIGHT 31

#define DEFAULT_COLOR (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define HIGHLIGHT_COLOR (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)
#define CHANGE_COLOR (FOREGROUND_RED | FOREGROUND_INTENSITY)

// Prepares console window to display virtual machine's state in a pretty way.
int disp_init();

// Prints interface.
void disp_print(struct virtual_machine* vm);

void disp_print_regs(struct virtual_machine* vm);

void disp_print_mem(struct virtual_machine* vm);

void disp_print_code(struct virtual_machine* vm);

// Clears console.
void disp_clear();

// Moves console cursor.
void disp_cursor(int column, int row);

// Changes foreground and background color attributes.
void disp_color(int color);
