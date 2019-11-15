#pragma once

#include "common.h"
#include "instruction.h"

// Parses input file and truns code into bytecode to be executed on virtual machine.
int hasm_assemble(const char* filename, struct prog_ptr* prog_ptr);

// Puts 32-bit value inside program memory at specfied address.
void mem_place_value(uint8_t* mem, int16_t addr, uint32_t value);

// Searches instruction array for an instruction of desired mnemonic. Returns NULL if failed.
const struct instruction* get_inst(const char* mnemonic);

// Searches instruction array for an instruction of desired opcode. Returns NULL if failed.
const struct instruction* get_inst_opcode(uint32_t opcode);

// Following functions handle turning instructions into opcodes (assembling). They return UINT32_MAX on failure.
uint32_t assemble_nop(const struct instruction* self, const char* args, const struct sym_table* sym_table);
uint32_t assemble_reg_and_reg(const struct instruction* self, const char* args, const struct sym_table* sym_table);
uint32_t assemble_mem_and_reg(const struct instruction* self, const char* args, const struct sym_table* sym_table);
uint32_t assemble_jump(const struct instruction* self, const char* args, const struct sym_table* sym_table);

// Following functions handle turning bytecode into readable instruction strings.
void disassemble_nop(const struct instruction* self, uint32_t bytecode, char* str);
void disassemble_reg_and_reg(const struct instruction* self, uint32_t bytecode, char* str);
void disassemble_mem_and_reg(const struct instruction* self, uint32_t bytecode, char* str);
void disassemble_jump(const struct instruction* self, uint32_t bytecode, char* str);
