#pragma once

// Stores information about program to be executed by virtual machine.
struct prog_ptr
{
    uint16_t mem_sz;        // Size of program code.
    uint16_t entry_addr;    // Address of first instruction to be executed.
    uint8_t* mem_ptr;       // Pointer to block of mem_sz bytes where program code is stored.
};
