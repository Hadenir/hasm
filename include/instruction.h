#pragma once

#include "common.h"
#include "sym_table.h"

struct instruction
{
    const char* mnemonic;   // Instruction's mnemonic, e.g. "A", "AR", "DC".
    const uint32_t opcode;  // Instruction's opcode, eg. 0x02, 0x03.
    const uint8_t width;    // Width of the instruction (2 or 4 bytes).
    uint32_t (*assemble_func)(const struct instruction*, const char*, const struct sym_table*);
    void (*disassemble_func)(const struct instruction*, uint32_t, char*);
};

// Assembles instruction using associated function.
uint32_t assemble(const struct instruction* inst, const char* args, const struct sym_table* sym_table);

// Disassembles instruction, turning bytecode into readable string.
void disassemble(const struct instruction* inst, uint32_t bytecode, char* str);
