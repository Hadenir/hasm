#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "source_code.h"

#define UNUSED(x) (void)(x)

#define NUM_HANDLERS 32

// Stores information about program to be executed by virtual machine.
struct program
{
    uint16_t mem_sz;            // Size of program code.
    uint16_t entry_addr;        // Address of first instruction to be executed.
    uint8_t* mem_ptr;           // Pointer to block of mem_sz bytes where program code is stored.
    struct source_code* source; // Program's source code.
};

// Stores whole state of virtual machine.
struct virtual_machine
{
    uint32_t pc;        // Address of next instruction to be executed.
    int32_t flags;      // State flags register.
    int32_t* regs;      // 16 general-purpose registers.
    uint8_t* memory;    // Address of allocated memory for virtual machine.
    uint32_t mem_sz;    // Size of allocated memory.

    bool (*handlers[NUM_HANDLERS])(struct virtual_machine*, uint32_t);    // Array of handler functions for each assembler instruction.
};
