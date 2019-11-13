#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "prog_ptr.h"

#define NUM_HANDLERS 32

uint32_t pc;        // Address of next instruction to be executed.
int32_t flags;     // State flags register.
int32_t* regs;     // 16 general-purpose registers.
uint8_t* memory;    // Address of allocated memory for virtual machine.
uint32_t mem_sz;    // Size of allocated memory.

bool (*handlers[NUM_HANDLERS])(uint32_t);    // Array of handler functions for each assembler instruction.

// Initializes virtual machine
int vm_init(struct prog_ptr prog_ptr);

// Starts executing code.
int vm_run();

// Does some clenup after virtual machine.
void vm_finalize();

// Helper function that updates flags register after instruction execution.
void vm_update_flags(int32_t value);

// Following functions handle performing each instruction.
bool handle_NOP(uint32_t args);
bool handle_A(uint32_t args);
bool handle_AR(uint32_t args);
bool handle_S(uint32_t args);
bool handle_SR(uint32_t args);
bool handle_M(uint32_t args);
bool handle_MR(uint32_t args);
bool handle_D(uint32_t args);
bool handle_DR(uint32_t args);
bool handle_C(uint32_t args);
bool handle_CR(uint32_t args);
bool handle_J(uint32_t args);
bool handle_JP(uint32_t args);
bool handle_JN(uint32_t args);
bool handle_JZ(uint32_t args);
bool handle_L(uint32_t args);
bool handle_LR(uint32_t args);
bool handle_ST(uint32_t args);
bool handle_LA(uint32_t args);
