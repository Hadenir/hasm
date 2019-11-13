#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "prog_ptr.h"

#define NUM_HANDLERS 32

struct virtual_machine
{
    uint32_t pc;        // Address of next instruction to be executed.
    int32_t flags;     // State flags register.
    int32_t* regs;     // 16 general-purpose registers.
    uint8_t* memory;    // Address of allocated memory for virtual machine.
    uint32_t mem_sz;    // Size of allocated memory.
};

bool (*handlers[NUM_HANDLERS])(struct virtual_machine*, uint32_t);    // Array of handler functions for each assembler instruction.

// Initializes virtual machine
int vm_init(struct prog_ptr prog_ptr, struct virtual_machine* vm);

// Starts executing code.
int vm_run(struct virtual_machine* vm);

// Executes one cycle on virtual machine.
int vm_step(struct virtual_machine* vm);

// Does some clenup after virtual machine.
void vm_finalize(struct virtual_machine* vm);

// Helper function that updates flags register after instruction execution.
void vm_update_flags(struct virtual_machine* vm, int32_t value);

// Following functions handle performing each instruction.
bool handle_NOP(struct virtual_machine* vm, uint32_t args);
bool handle_A(struct virtual_machine* vm, uint32_t args);
bool handle_AR(struct virtual_machine* vm, uint32_t args);
bool handle_S(struct virtual_machine* vm, uint32_t args);
bool handle_SR(struct virtual_machine* vm, uint32_t args);
bool handle_M(struct virtual_machine* vm, uint32_t args);
bool handle_MR(struct virtual_machine* vm, uint32_t args);
bool handle_D(struct virtual_machine* vm, uint32_t args);
bool handle_DR(struct virtual_machine* vm, uint32_t args);
bool handle_C(struct virtual_machine* vm, uint32_t args);
bool handle_CR(struct virtual_machine* vm, uint32_t args);
bool handle_J(struct virtual_machine* vm, uint32_t args);
bool handle_JP(struct virtual_machine* vm, uint32_t args);
bool handle_JN(struct virtual_machine* vm, uint32_t args);
bool handle_JZ(struct virtual_machine* vm, uint32_t args);
bool handle_L(struct virtual_machine* vm, uint32_t args);
bool handle_LR(struct virtual_machine* vm, uint32_t args);
bool handle_ST(struct virtual_machine* vm, uint32_t args);
bool handle_LA(struct virtual_machine* vm, uint32_t args);
