#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "instruction.h"
#include "prog_ptr.h"

// Parses input file and truns code into bytecode to be executed on virtual machine.
int hasm_assemble(const char* filename, struct prog_ptr* prog_ptr);

// Puts 32-bit value inside program memory at specfied address.
void mem_place_value(uint8_t* mem, int16_t addr, uint32_t value);

// Searches instruction array for an instruction of desired mnemonic. Returns NULL if failed.
const struct instruction* get_inst(const char* mnemonic);

// Following functions handle turning instructions into opcodes (assembling). They return UINT32_MAX on failure.
uint32_t assemble_nop(const struct instruction* self, const char* args, const struct sym_table* sym_table);
uint32_t assemble_reg_and_reg(const struct instruction* self, const char* args, const struct sym_table* sym_table);
uint32_t assemble_mem_and_reg(const struct instruction* self, const char* args, const struct sym_table* sym_table);
uint32_t assemble_jump(const struct instruction* self, const char* args, const struct sym_table* sym_table);
