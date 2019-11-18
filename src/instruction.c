#include "instruction.h"

uint32_t assemble(const struct instruction* inst, const char* args, const struct sym_table* sym_table)
{
    return inst->assemble_func(inst, args, sym_table);
}

void disassemble(const struct instruction* inst, uint32_t bytecode, char* str)
{
    inst->disassemble_func(inst, bytecode, str);
}
