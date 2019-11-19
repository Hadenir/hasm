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

#define ARROW_UP_KEY 72
#define ARROW_DOWN_KEY 80
#define SPACE_KEY 32
#define RETURN_KEY 13
#define ESCAPE_KEY 27

struct display
{
    void* console_handle;   // Windows (WinAPI) console handle.
    int32_t* vm_regs;       // Copy of previously displayed values in registers.
    uint8_t* vm_memory;     // Copy of previously displayed values in memory.
    uint32_t mem_scroll;
    uint32_t mem_max_scroll;
    uint32_t code_scroll;
    char* status;
} display;

// Prepares console window to display virtual machine's state in a pretty way.
int disp_init(struct virtual_machine* vm, struct program* program);

void disp_finilize();

// Update text on status bar.
void disp_status(const char* status);

// Updates interface information.
// Returns different values depending on user intent:
// 0 - exit
// 1 - step forward
// 2 - continue execution till end
int disp_update(struct virtual_machine* vm, struct program* program);

void update_internal_vm(struct virtual_machine* vm);

// Helper functions for printing user interface.
void print_grid();

void print_regs(struct virtual_machine* vm);

void print_mem(struct virtual_machine* vm);

void print_code(struct virtual_machine* vm, struct program* program);

// Clears console.
void disp_clear();

// Moves console cursor.
void disp_cursor(int column, int row);

// Changes foreground and background color attributes.
void disp_color(int color);
