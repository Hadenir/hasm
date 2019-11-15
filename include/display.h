#pragma once

#include "assembler.h"
#include "virtual_machine.h"

#define DISPLAY_WIDTH 150
#define DISPLAY_HEIGHT 50

#define CODE_BLOCK_WIDTH 88
#define MEM_BLOCK_HEIGHT 31

#define DEFAULT_COLOR (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define HIGHLIGHT_COLOR (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)
#define CHANGE_COLOR (FOREGROUND_RED | FOREGROUND_INTENSITY)

struct display
{
    void* console_handle;   // Windows (WinAPI) console handle.
    int32_t* vm_regs;       // Copy of previously displayed values in registers.
    uint8_t* vm_memory;     // Copy of previously displayed values in memory.
} display;

// Prepares console window to display virtual machine's state in a pretty way.
int disp_init(struct virtual_machine* vm);

void disp_finilize();

// Updates interface information.
// Returns different values depending on user intent:
// 0 - exit
// 1 - step forward
// 3 - continue execution till end
// 4 - restart
int disp_update(struct virtual_machine* vm, struct prog_ptr* prog_ptr);

void update_internal_vm(struct virtual_machine* vm);

// Helper functions for printing user interface.
void print_regs(struct virtual_machine* vm);

void print_mem(struct virtual_machine* vm);

void print_code(struct virtual_machine* vm, struct prog_ptr* prog_ptr);

// Clears console.
void disp_clear();

// Moves console cursor.
void disp_cursor(int column, int row);

// Changes foreground and background color attributes.
void disp_color(int color);
