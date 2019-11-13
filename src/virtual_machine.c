#include "virtual_machine.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int vm_init(struct prog_ptr prog_ptr)
{
    mem_sz = prog_ptr.mem_sz;
    memory = prog_ptr.mem_ptr;
    pc = prog_ptr.entry_addr;

    if(memory == NULL || mem_sz == 0)   // Memory cannot point to null nor be size of zero.
        return 1;

    if(pc >= mem_sz)    // Entry point must be inside
        return 2;

    flags = 0;
    regs = calloc(16, 4);  // Allocate memory for an array of 16 32-bit registers.

    handlers[0x00] = handle_NOP;
    handlers[0x02] = handle_A;
    handlers[0x03] = handle_AR;
    handlers[0x04] = handle_S;
    handlers[0x05] = handle_SR;
    handlers[0x06] = handle_M;
    handlers[0x07] = handle_MR;
    handlers[0x08] = handle_D;
    handlers[0x09] = handle_DR;
    handlers[0x0a] = handle_C;
    handlers[0x0b] = handle_CR;
    handlers[0x0c] = handle_J;
    handlers[0x0d] = handle_JP;
    handlers[0x0e] = handle_JN;
    handlers[0x0f] = handle_JZ;
    handlers[0x10] = handle_L;
    handlers[0x11] = handle_LR;
    handlers[0x12] = handle_ST;
    handlers[0x14] = handle_LA;

    return 0;
}

int vm_run()
{
    uint8_t opcode = 0;
    uint32_t args = 0;
    bool reg_inst = false;

    for(;;)
    {
        opcode = *(uint8_t*) (memory + pc);
        reg_inst = (opcode & 1) != 0; // Even instructions work with memory, odd ones with 2 registers.

        if(opcode >= 0x0c && opcode <= 0x0f)
            reg_inst = false;   // Jump instructions are always 4-bytes long.

        if(reg_inst)
            args = *(memory + pc + 1);
        else
            args = *(uint32_t*) (memory + pc) >> 8;

        pc += reg_inst ? 2 : 4; /* Next instruction is 2 bytes further if current instruction is register-register,
                                   otherwise we need to skip 4 bytes (register-memory instruction). */
        if(pc > mem_sz)
            break;

        if(!handlers[opcode](args))
            return 1;
    }

    return 0;
}

void vm_finalize()
{
    free(regs);
    free(memory);
}

void vm_update_flags(int32_t value)
{
    if(value == 0)
        flags = 0;
    else if(value > 0)
        flags = 1;
    else
        flags = 2;
}

bool handle_NOP(uint32_t args)
{
    printf("-> ___\n");

    return true;
}

bool handle_A(uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    printf("-> (%d) r%u += [0x%08x] ", regs[reg], reg, addr + regs[addr_reg]);

    if(addr >= mem_sz)
        return false;

    int32_t value = *(int32_t*) (memory + addr + regs[addr_reg]);

    printf("(%d)\n", value);

    regs[reg] += value;
    vm_update_flags(regs[reg]);

    return true;
}

bool handle_AR(uint32_t args)
{
    uint8_t regA = args & 0xf;
    uint8_t regB = args >> 4;

    printf("-> (%d) r%u += r%u (%d)\n", regs[regA], regA, regB, regs[regB]);

    regs[regA] += regs[regB];
    vm_update_flags(regs[regA]);

    return true;
}

bool handle_S(uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    printf("-> (%d) r%u -= [0x%08x] ", regs[reg], reg, addr + regs[addr_reg]);

    if(addr >= mem_sz)
        return false;

    int32_t value = *(int32_t*) (memory + addr + regs[addr_reg]);

    printf("(%d)\n", value);

    regs[reg] -= value;
    vm_update_flags(regs[reg]);

    return true;
}

bool handle_SR(uint32_t args)
{
    uint8_t regA = args & 0xf;
    uint8_t regB = args >> 4;

    printf("-> (%d) r%u -= r%u (%d)\n", regs[regA], regA, regB, regs[regB]);

    regs[regA] -= regs[regB];
    vm_update_flags(regs[regA]);

    return true;
}

bool handle_M(uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    printf("-> (%d) r%u *= [0x%08x] ", regs[reg], reg, addr + regs[addr_reg]);

    if(addr >= mem_sz)
        return false;

    int32_t value = *(int32_t*) (memory + addr + regs[addr_reg]);

    printf("(%d)\n", value);

    regs[reg] *= value;
    vm_update_flags(regs[reg]);

    return true;
}

bool handle_MR(uint32_t args)
{
    uint8_t regA = args & 0xf;
    uint8_t regB = args >> 4;

    printf("-> (%d) r%u *= r%u (%d)\n", regs[regA], regA, regB, regs[regB]);

    regs[regA] *= regs[regB];
    vm_update_flags(regs[regA]);

    return true;
}

bool handle_D(uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    printf("-> (%d) r%u /= [0x%08x] ", regs[reg], reg, addr + regs[addr_reg]);

    if(addr >= mem_sz)
        return false;

    int32_t value = *(int32_t*) (memory + addr + regs[addr_reg]);

    if(value == 0)  // Division by zero is an invalid operation.
    {
        flags = 3;
        return true;
    }

    printf("(%d)\n", value);

    regs[reg] /= value;
    vm_update_flags(regs[reg]);

    return true;
}

bool handle_DR(uint32_t args)
{
    uint8_t regA = args & 0xf;
    uint8_t regB = args >> 4;

    printf("-> (%d) r%u /= r%u (%d)\n", regs[regA], regA, regB, regs[regB]);

    int32_t value = regs[regB];

    if(value == 0)
    {
        flags = 3;
        return true;
    }

    regs[regA] /= value;
    vm_update_flags(regs[regA]);

    return true;
}

bool handle_C(uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    printf("-> (%d) r%u <=> [0x%08x] ", regs[reg], reg, addr + regs[addr_reg]);

    if(addr >= mem_sz)
        return false;

    int32_t value = *(int32_t*) (memory + addr + regs[addr_reg]);

    printf("(%d)\n", value);

    int32_t result = regs[reg] - value;
    vm_update_flags(result);

    return true;
}

bool handle_CR(uint32_t args)
{
    uint8_t regA = args & 0xf;
    uint8_t regB = args >> 4;

    printf("-> (%d) r%u <=> r%u (%d)\n", regs[regA], regA, regB, regs[regB]);

    int32_t result = regs[regA] - regs[regB];
    vm_update_flags(result);

    return true;
}

bool handle_J(uint32_t args)
{
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    printf("-> J(pc = 0x%08x)\n", addr);

    if(addr >= mem_sz)
        return false;

    pc = addr;

    return true;
}

bool handle_JP(uint32_t args)
{
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    printf("-> JP(pc = 0x%08x)\n", addr);

    if(addr >= mem_sz)
        return false;

    if(flags == 1)
        pc = addr;

    return true;
}

bool handle_JN(uint32_t args)
{
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    printf("-> JN(pc = 0x%08x)\n", addr);

    if(addr >= mem_sz)
        return false;

    if(flags == 2)
        pc = addr;

    return true;
}

bool handle_JZ(uint32_t args)
{
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    printf("-> JZ(pc = 0x%08x)\n", addr);

    if(addr >= mem_sz)
        return false;

    if(flags == 0)
        pc = addr;

    return true;
}

bool handle_L(uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    printf("-> r%u := [0x%08x] ", reg, addr + regs[addr_reg]);

    if(addr >= mem_sz)
        return false;

    int32_t value = *(int32_t*) (memory + addr + regs[addr_reg]);

    printf("(%d)\n", value);

    regs[reg] = value;
    return true;
}

bool handle_LR(uint32_t args)
{
    uint8_t regA = args & 0xf;
    uint8_t regB = args >> 4;

    printf("-> r%u := r%u (%d)\n", regA, regB, regs[regB]);

    regs[regA] = regs[regB];
    return true;
}

bool handle_ST(uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    printf("-> [0x%08x] := r%u (%d)\n", addr + regs[addr_reg], reg, regs[reg]);

    if(addr >= mem_sz)
        return false;

    *(int32_t*) (memory + addr + regs[addr_reg]) = regs[reg];
    return true;
}

bool handle_LA(uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    printf("-> r%u := 0x%08x\n", reg, addr + regs[addr_reg]);

    regs[reg] = addr + regs[addr_reg];
    return true;
}
