#include "assembler.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sym_table.h"
#include "test.h"

#define NUM_INSTRUCTIONS 19
#define MAX_TOKEN_LENGTH 64
#define MAX_LINE_LENGTH 256

const struct instruction instructions[NUM_INSTRUCTIONS] = {
    {"NOP", 0x00, 4, &assemble_nop},        // Perform no operation.
    {"A",  0x02, 4, &assemble_mem_and_reg}, // Add value in memory to value in a register.
    {"AR", 0x03, 2, &assemble_reg_and_reg}, // Add value in a register to value in another one.
    {"S",  0x04, 4, &assemble_mem_and_reg}, // Substract value in memory from value in a register.
    {"SR", 0x05, 2, &assemble_reg_and_reg}, // Substract value in a register from value in another one.
    {"M",  0x06, 4, &assemble_mem_and_reg}, // Multiply value in memory with value in a register.
    {"MR", 0x07, 2, &assemble_reg_and_reg}, // Multiply value in a register with value in another one.
    {"D",  0x08, 4, &assemble_mem_and_reg}, // Divide value in a register by value in memory.
    {"DR", 0x09, 2, &assemble_reg_and_reg}, // Divide value in a register by value in another one.
    {"C",  0x0a, 4, &assemble_mem_and_reg}, // Compare value in memory to value in a regsiter.
    {"CR", 0x0b, 2, &assemble_reg_and_reg}, // Compare value in register to value in another one.
    {"J",  0x0c, 4, &assemble_jump},        // Perform unconditional jump.
    {"JP", 0x0d, 4, &assemble_jump},        // Perform jump if the result of previous instruction was positive.
    {"JN", 0x0e, 4, &assemble_jump},        // Perform jump if the result of previous instruction was negative.
    {"JZ", 0x0f, 4, &assemble_jump},        // Perform jump if the result of previous instruction was zero.
    {"L",  0x10, 4, &assemble_mem_and_reg}, // Load value in memory into a register.
    {"LR", 0x11, 2, &assemble_reg_and_reg}, // Load value in a register into another one.
    {"ST", 0x12, 4, &assemble_mem_and_reg}, // Store in memory value in a register.
    {"LA", 0x14, 4, &assemble_mem_and_reg}, // Load address in memory into a register.
};

int hasm_assemble(const char* filename, struct prog_ptr* prog_ptr)
{
    if(filename == NULL || prog_ptr == NULL)
        return 1;

    FILE* file = fopen(filename, "r");
    if(file == NULL)
        return 2;

    char* line = malloc(MAX_LINE_LENGTH);
    char* token = malloc(MAX_TOKEN_LENGTH);
    uint16_t curr_addr = 0;
    struct sym_table* sym_table = NULL;

    // First pass over file collects symbols (labels) and stores addresses they point to.
    while(fgets(line, MAX_LINE_LENGTH, file))
    {
        if(line[0] == '\n' || line[0] == '\r' || line[0] == '#')
            continue;

        uint32_t offset = 0;
        uint32_t chars_read = 0;

        if(sscanf(line, "%63s %n", token, &chars_read) == 0)
        {
            sym_table_free(sym_table);
            free(token);
            free(line);
            return 3;
        }
        offset += chars_read;

        const struct instruction* inst = get_inst(token);
        if(inst != NULL) // If the token is an instruction, it's not a label.
        {
            curr_addr += inst->width;
        }
        else if(strcmp(token, "DS") == 0 || strcmp(token, "DC") == 0)   // If the token is "DS"/"DC", it's not a label as well.
        {
            uint32_t count = 0;
            if(sscanf(line + offset, "%u*INTEGER", &count) == 1)
                curr_addr += count * 4;
            else
                curr_addr += 4;
        }
        else    // The token is definitely a label.
        {
            sym_table_push_back(&sym_table, token, curr_addr);

            // We saved the label, so we can now read instruction to know if it's 2- or 4-bytes long.
            if(sscanf(line + offset, "%63s %n", token, &chars_read) == 0)
            {
                sym_table_free(sym_table);
                free(token);
                free(line);
                return 3;
            }
            offset += chars_read;

            // "DS"/"DC" must be handled differently than usual instrucions.
            if(strcmp(token, "DS") == 0 || strcmp(token, "DC") == 0)
            {
                uint32_t count = 0;
                if(sscanf(line + offset, "%u*INTEGER", &count) == 1)
                    curr_addr += count * 4;
                else
                    curr_addr += 4;
            }
            else
            {
                const struct instruction* inst = get_inst(token);
                if(inst == NULL)    // Unrecognized instruction mnemonic.
                {
                    sym_table_free(sym_table);
                    free(token);
                    free(line);
                    return 4;
                }

                curr_addr += inst->width;
            }
        }
    }

    print_sym_table(sym_table);

    rewind(file);
    bool code_block = false;
    uint16_t entry_addr;
    uint16_t mem_sz = curr_addr;
    uint8_t* mem = malloc(mem_sz);
    curr_addr = 0;

    // Second pass is where proper assembling happens.
    while(fgets(line, MAX_LINE_LENGTH, file))
    {
        if(line[0] == '\n' || line[0] == '\r' || line[0] == '#')
            continue;

        uint32_t offset = 0;
        uint32_t chars_read = 0;

        if(sscanf(line, "%63s %n", token, &chars_read) == 0)
        {
            sym_table_free(sym_table);
            free(token);
            free(line);
            return 3;
        }
        offset += chars_read;

        const struct instruction* inst = get_inst(token);

        // If the first token isn't an instruction, nor is it "DS"/"DC", then it's a label. We can skip it now.
        if(inst == NULL && strcmp(token, "DS") != 0 && strcmp(token, "DC") != 0)
        {
            if(sscanf(line + offset, "%63s %n", token, &chars_read) == 0)
            {
                sym_table_free(sym_table);
                free(token);
                free(line);
                return 3;
            }
            offset += chars_read;

            inst = get_inst(token);
        }

        if(inst != NULL)    // Instructions can be assembled using associated assemble function.
        {
            if(sscanf(line + offset, "%63[^\t\r\n]", token) == 0)
            {
                sym_table_free(sym_table);
                free(token);
                free(line);
                return 3;
            }

            uint32_t bytecode = assemble(inst, token, sym_table);
            if(bytecode == UINT32_MAX)
            {
                sym_table_free(sym_table);
                free(token);
                free(line);
                return 4;
            }

            mem_place_value(mem, curr_addr, bytecode);

            // First instruction to appear starts code block. This will be entry point in assemled program.
            if(!code_block)
            {
                entry_addr = curr_addr;
                code_block = true;
            }

            curr_addr += inst->width;
        }
        else if(strcmp(token, "DS") == 0 || strcmp(token, "DC") == 0)
        {
            uint32_t count = 0, value = 0;
            if(sscanf(line + offset, "%u*INTEGER(%u)", &count, &value) == 2)
            {
                for(uint32_t i = 0; i < count; ++i)
                    mem_place_value(mem, curr_addr + i * 4, value);

                curr_addr += count * 4;
            }
            else if(sscanf(line + offset, "%u*INTEGER", &count) == 1)
            {
                for(uint32_t i = 0; i < count; ++i)
                    mem_place_value(mem, curr_addr + i * 4, 0);

                curr_addr += count * 4;
            }
            else if(sscanf(line + offset, "INTEGER(%u)", &value) == 1)
            {
                mem_place_value(mem, curr_addr, value);
                curr_addr += 4;
            }
            else
            {
                mem_place_value(mem, curr_addr, 0);
                curr_addr += 4;
            }
        }
    }

    prog_ptr->mem_sz = mem_sz;
    prog_ptr->entry_addr = entry_addr;
    prog_ptr->mem_ptr = mem;

    sym_table_free(sym_table);
    free(line);
    free(token);
    return 0;
}

void mem_place_value(uint8_t* mem, int16_t addr, uint32_t value)
{
    *((uint32_t*)(mem + addr)) = value;
}

const struct instruction* get_inst(const char* mnemonic)
{
    const struct instruction* inst;
    for(int i = 0; i < NUM_INSTRUCTIONS; ++i)
    {
        inst = &instructions[i];
        if(strcmp(mnemonic, inst->mnemonic) == 0)
            return inst;
    }

    return NULL;
}

uint32_t assemble_nop(const struct instruction* self, const char* args, const struct sym_table* sym_table)
{
    uint32_t bytecode = 0;

    bytecode |= self->opcode;

    return bytecode;
}

uint32_t assemble_reg_and_reg(const struct instruction* self, const char* args, const struct sym_table* sym_table)
{
    uint32_t bytecode = 0;

    uint16_t dest_reg, src_reg;
    if(sscanf(args, "%hu , %hu", &dest_reg, &src_reg) != 2)
        return UINT32_MAX;

    bytecode |= self->opcode;
    bytecode |= dest_reg << 8;
    bytecode |= src_reg << 12;

    return bytecode;
}

uint32_t assemble_mem_and_reg(const struct instruction* self, const char* args, const struct sym_table* sym_table)
{
    uint32_t bytecode = 0;

    uint16_t dest_reg, addr_reg, addr;
    char* label = calloc(MAX_TOKEN_LENGTH, 1);
    if(sscanf(args, "%hu , %hu ( %hu )", &dest_reg, &addr, &addr_reg) != 3)
    {
        if(sscanf(args, "%hu , %s", &dest_reg, label) == 2)
        {
            addr = sym_table_get(sym_table, label);
            if(addr == UINT16_MAX)
                return UINT32_MAX;
            addr_reg = 14;  // r14 is default address register.
        }
        else
        {
            return UINT32_MAX;
        }
    }

    bytecode |= self->opcode;
    bytecode |= dest_reg << 8;
    bytecode |= addr_reg << 12;
    bytecode |= addr << 16;

    free(label);
    return bytecode;
}

uint32_t assemble_jump(const struct instruction* self, const char* args, const struct sym_table* sym_table)
{
    uint32_t bytecode = 0;

    uint16_t addr_reg, addr;
    char* label = malloc(MAX_TOKEN_LENGTH);
    if(sscanf(args, "%hu ( %hu )", &addr, &addr_reg) != 2)
    {
        if(sscanf(args, "%s" , label) == 1)
        {
            addr = sym_table_get(sym_table, label);
            if(addr == UINT16_MAX)
                return UINT32_MAX;
            addr_reg = 14;  // r14 is default address register.
        }
        else
        {
            return UINT32_MAX;
        }
    }

    bytecode |= self->opcode;
    bytecode |= addr_reg << 12;
    bytecode |= addr << 16;

    free(label);
    return bytecode;
}
