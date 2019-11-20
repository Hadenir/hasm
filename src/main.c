#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#include "assembler.h"
#include "display.h"
#include "virtual_machine.h"

// TODO: Get rid of some malloc()'s
// TODO: Cleaning, as always

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Wrong number of arguments. Use: hasm <file.hasm>\n");
        return -1;
    }

    int result;

    struct program program;
    const char* filename = argv[1];

    printf("Assembling %s...\n", filename);
    result = hasm_assemble(filename, &program);
    if(result != 0)
    {
        fprintf(stderr, "Error while assembling %s! Error code: %d\n", filename, result);
        return result;
    }

    struct virtual_machine vm;

    printf("Initializing virtual machine...\n");
    result = vm_init(program, &vm);
    if(result != 0)
    {
        fprintf(stderr, "Error while initializing virtual machine! Error code: %d\n", result);
        return result;
    }

    printf("Initializing console window...\n");
    result = disp_init(&vm, &program);
    if(result != 0)
    {
        fprintf(stderr, "Error while initializing console window! Error code: %d\n", result);
        return result;
    }

    char status[100];
    int action;
    while(result == 0)
    {
        sprintf(status, "File: %s", filename);
        disp_status(status);

        action = disp_update(&vm, &program);

        sprintf(status, "Executing...");
        disp_status(status);
        switch(action)
        {
            case 0:
                result = 1;
                break;
            case 1:
                result = vm_step(&vm);
                break;
            case 2:
                result = vm_run(&vm);
                break;
        }
    }

    sprintf(status, "Program exited with code 0x%02x. Press any key to quit...", result);
    disp_status(status);
    disp_update(&vm, &program);
    disp_clear();

    vm_finalize(&vm);
    source_code_free(&program.source);
    disp_finilize();

    printf("Goodbye.\n");
    return 0;
}
