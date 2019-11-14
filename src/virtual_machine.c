#include "virtual_machine.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int vm_init(struct prog_ptr prog_ptr, struct virtual_machine* vm)
{
    vm->mem_sz = prog_ptr.mem_sz;
    vm->memory = prog_ptr.mem_ptr;
    vm->pc = prog_ptr.entry_addr;

    if(vm->memory == NULL || vm->mem_sz == 0)   // vm->memory cannot point to null nor be size of zero.
        return 1;

    if(vm->pc >= vm->mem_sz)    // Entry point must be inside
        return 2;

    vm->flags = 0;
    vm->regs = calloc(16, 4);  // Allocate vm->memory for an array of 16 32-bit registers.

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

int vm_run(struct virtual_machine* vm)
{
    int result = 0;
    while(vm->pc < vm->mem_sz)  // Execute until there are no more instructions to perform.
    {
        result = vm_step(vm);
        if(result != 0)
            return result;
    }

    return 0;
}

int vm_step(struct virtual_machine* vm)
{
    if(vm->pc >= vm->mem_sz)
        return 1;

    uint8_t opcode = 0;
    uint32_t args = 0;
    bool reg_inst = false;

    opcode = *(uint8_t*) (vm->memory + vm->pc);
    reg_inst = (opcode & 1) != 0; // Even instructions work with vm->memory, odd ones with 2 registers.

    if(opcode >= 0x0c && opcode <= 0x0f)
        reg_inst = false;   // Jump instructions are always 4-bytes long.

    if(reg_inst)
        args = *(vm->memory + vm->pc + 1);
    else
        args = *(uint32_t*) (vm->memory + vm->pc) >> 8;

    vm->pc += reg_inst ? 2 : 4; /* Next instruction is 2 bytes further if current instruction is register-register,
                                otherwise we need to skip 4 bytes (register-vm->memory instruction). */

    if(!handlers[opcode](vm, args))
        return 2;

    return 0;
}

void vm_finalize(struct virtual_machine* vm)
{
    free(vm->regs);
    free(vm->memory);
}

void vm_update_flags(struct virtual_machine* vm, int32_t value)
{
    if(value == 0)
        vm->flags = 0;
    else if(value > 0)
        vm->flags = 1;
    else
        vm->flags = 2;
}

bool handle_NOP(struct virtual_machine* vm, uint32_t args)
{
    // printf("-> ___\n");

    return true;
}

bool handle_A(struct virtual_machine* vm, uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    // printf("-> (%d) r%u += [0x%08x] ", vm->regs[reg], reg, addr + vm->regs[addr_reg]);

    if(addr >= vm->mem_sz)
        return false;

    int32_t value = *(int32_t*) (vm->memory + addr + vm->regs[addr_reg]);

    // printf("(%d)\n", value);

    vm->regs[reg] += value;
    vm_update_flags(vm, vm->regs[reg]);

    return true;
}

bool handle_AR(struct virtual_machine* vm, uint32_t args)
{
    uint8_t regA = args & 0xf;
    uint8_t regB = args >> 4;

    // printf("-> (%d) r%u += r%u (%d)\n", vm->regs[regA], regA, regB, vm->regs[regB]);

    vm->regs[regA] += vm->regs[regB];
    vm_update_flags(vm, vm->regs[regA]);

    return true;
}

bool handle_S(struct virtual_machine* vm, uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    // printf("-> (%d) r%u -= [0x%08x] ", vm->regs[reg], reg, addr + vm->regs[addr_reg]);

    if(addr >= vm->mem_sz)
        return false;

    int32_t value = *(int32_t*) (vm->memory + addr + vm->regs[addr_reg]);

    // printf("(%d)\n", value);

    vm->regs[reg] -= value;
    vm_update_flags(vm, vm->regs[reg]);

    return true;
}

bool handle_SR(struct virtual_machine* vm, uint32_t args)
{
    uint8_t regA = args & 0xf;
    uint8_t regB = args >> 4;

    // printf("-> (%d) r%u -= r%u (%d)\n", vm->regs[regA], regA, regB, vm->regs[regB]);

    vm->regs[regA] -= vm->regs[regB];
    vm_update_flags(vm, vm->regs[regA]);

    return true;
}

bool handle_M(struct virtual_machine* vm, uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    // printf("-> (%d) r%u *= [0x%08x] ", vm->regs[reg], reg, addr + vm->regs[addr_reg]);

    if(addr >= vm->mem_sz)
        return false;

    int32_t value = *(int32_t*) (vm->memory + addr + vm->regs[addr_reg]);

    // printf("(%d)\n", value);

    vm->regs[reg] *= value;
    vm_update_flags(vm, vm->regs[reg]);

    return true;
}

bool handle_MR(struct virtual_machine* vm, uint32_t args)
{
    uint8_t regA = args & 0xf;
    uint8_t regB = args >> 4;

    // printf("-> (%d) r%u *= r%u (%d)\n", vm->regs[regA], regA, regB, vm->regs[regB]);

    vm->regs[regA] *= vm->regs[regB];
    vm_update_flags(vm, vm->regs[regA]);

    return true;
}

bool handle_D(struct virtual_machine* vm, uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    // printf("-> (%d) r%u /= [0x%08x] ", vm->regs[reg], reg, addr + vm->regs[addr_reg]);

    if(addr >= vm->mem_sz)
        return false;

    int32_t value = *(int32_t*) (vm->memory + addr + vm->regs[addr_reg]);

    if(value == 0)  // Division by zero is an invalid operation.
    {
        vm->flags = 3;
        return true;
    }

    // printf("(%d)\n", value);

    vm->regs[reg] /= value;
    vm_update_flags(vm, vm->regs[reg]);

    return true;
}

bool handle_DR(struct virtual_machine* vm, uint32_t args)
{
    uint8_t regA = args & 0xf;
    uint8_t regB = args >> 4;

    // printf("-> (%d) r%u /= r%u (%d)\n", vm->regs[regA], regA, regB, vm->regs[regB]);

    int32_t value = vm->regs[regB];

    if(value == 0)
    {
        vm->flags = 3;
        return true;
    }

    vm->regs[regA] /= value;
    vm_update_flags(vm, vm->regs[regA]);

    return true;
}

bool handle_C(struct virtual_machine* vm, uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    // printf("-> (%d) r%u <=> [0x%08x] ", vm->regs[reg], reg, addr + vm->regs[addr_reg]);

    if(addr >= vm->mem_sz)
        return false;

    int32_t value = *(int32_t*) (vm->memory + addr + vm->regs[addr_reg]);

    // printf("(%d)\n", value);

    int32_t result = vm->regs[reg] - value;
    vm_update_flags(vm, result);

    return true;
}

bool handle_CR(struct virtual_machine* vm, uint32_t args)
{
    uint8_t regA = args & 0xf;
    uint8_t regB = args >> 4;

    // printf("-> (%d) r%u <=> r%u (%d)\n", vm->regs[regA], regA, regB, vm->regs[regB]);

    int32_t result = vm->regs[regA] - vm->regs[regB];
    vm_update_flags(vm, result);

    return true;
}

bool handle_J(struct virtual_machine* vm, uint32_t args)
{
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = (args >> 8) + vm->regs[addr_reg];

    // printf("-> J(vm->pc = 0x%08x)\n", addr);

    if(addr >= vm->mem_sz)
        return false;

    vm->pc = addr;

    return true;
}

bool handle_JP(struct virtual_machine* vm, uint32_t args)
{
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = (args >> 8) + vm->regs[addr_reg];

    // printf("-> JP(vm->pc = 0x%08x)\n", addr);

    if(addr >= vm->mem_sz)
        return false;

    if(vm->flags == 1)
        vm->pc = addr;

    return true;
}

bool handle_JN(struct virtual_machine* vm, uint32_t args)
{
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = (args >> 8) + vm->regs[addr_reg];

    // printf("-> JN(vm->pc = 0x%08x)\n", addr);

    if(addr >= vm->mem_sz)
        return false;

    if(vm->flags == 2)
        vm->pc = addr;

    return true;
}

bool handle_JZ(struct virtual_machine* vm, uint32_t args)
{
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = (args >> 8) + vm->regs[addr_reg];

    // printf("-> JZ(vm->pc = 0x%08x)\n", addr);

    if(addr >= vm->mem_sz)
        return false;

    if(vm->flags == 0)
        vm->pc = addr;

    return true;
}

bool handle_L(struct virtual_machine* vm, uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    // printf("-> r%u := [0x%08x] ", reg, addr + vm->regs[addr_reg]);

    if(addr >= vm->mem_sz)
        return false;

    int32_t value = *(int32_t*) (vm->memory + addr + vm->regs[addr_reg]);

    // printf("(%d)\n", value);

    vm->regs[reg] = value;
    return true;
}

bool handle_LR(struct virtual_machine* vm, uint32_t args)
{
    uint8_t regA = args & 0xf;
    uint8_t regB = args >> 4;

    // printf("-> r%u := r%u (%d)\n", regA, regB, vm->regs[regB]);

    vm->regs[regA] = vm->regs[regB];
    return true;
}

bool handle_ST(struct virtual_machine* vm, uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    // printf("-> [0x%08x] := r%u (%d)\n", addr + vm->regs[addr_reg], reg, vm->regs[reg]);

    if(addr >= vm->mem_sz)
        return false;

    *(int32_t*) (vm->memory + addr + vm->regs[addr_reg]) = vm->regs[reg];
    return true;
}

bool handle_LA(struct virtual_machine* vm, uint32_t args)
{
    uint8_t reg = args & 0xf;
    uint8_t addr_reg = (args >> 4) & 0xf;
    uint16_t addr = args >> 8;

    // printf("-> r%u := 0x%08x\n", reg, addr + vm->regs[addr_reg]);

    vm->regs[reg] = addr + vm->regs[addr_reg];
    return true;
}
