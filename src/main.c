#include <stdio.h>
#include <stdlib.h>

#include "assembler.h"
#include "virtual_machine.h"

int main()
{
    int result;
    struct prog_ptr prog_ptr;
    const char* filename = "test.hasm";

    printf("Assembling %s...\n", filename);
    result = hasm_assemble(filename, &prog_ptr);
    if(result != 0)
    {
        fprintf(stderr, "Error while assembling %s! Error code: %d\n", filename, result);
        return result;
    }

    printf("mem_ptr: 0x%x, mem_sz: 0x%x, entry_addr: 0x%x\n", (uint32_t)prog_ptr.mem_ptr, prog_ptr.mem_sz, prog_ptr.entry_addr);

    printf("Initializing virtual machine...\n");
    result = vm_init(prog_ptr);
    if(result != 0)
    {
        fprintf(stderr, "Error while initializing virtual machine! Error code: %d\n", result);
        return result;
    }

    printf("Executing %s...\n", filename);
    result = vm_run();
    if(result != 0)
    {
        fprintf(stderr, "Error while running %s! Error code: %d\n", filename, result);
        return result;
    }

    printf("Program exited. Virtual machine shutting down...\n");
    vm_finalize();
    printf("Goodbye.");
    return 0;
}
