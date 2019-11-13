#include <stdio.h>
#include <stdlib.h>

#include "assembler.h"
#include "virtual_machine.h"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Wrong number of arguments. Use: hasm <file.hasm>\n");
        return -1;
    }

    int result;

    struct prog_ptr prog_ptr;
    const char* filename = argv[1];

    printf("Assembling %s...\n", filename);
    result = hasm_assemble(filename, &prog_ptr);
    if(result != 0)
    {
        fprintf(stderr, "Error while assembling %s! Error code: %d\n", filename, result);
        return result;
    }

    struct virtual_machine vm;

    printf("Initializing virtual machine...\n");
    result = vm_init(prog_ptr, &vm);
    if(result != 0)
    {
        fprintf(stderr, "Error while initializing virtual machine! Error code: %d\n", result);
        return result;
    }

    printf("Executing %s...\n", filename);
    result = vm_run(&vm);
    if(result != 0)
    {
        fprintf(stderr, "Error while running %s! Error code: %d\n", filename, result);
        return result;
    }

    printf("Program exited. Virtual machine shutting down...\n");
    vm_finalize(&vm);

    printf("Goodbye.");
    return 0;
}
