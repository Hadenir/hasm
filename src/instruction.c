#include "instruction.h"

uint32_t assemble(const struct instruction* inst, const char* args, const struct sym_table* sym_table)
{
    return inst->assemble_func(inst, args, sym_table);
}
