#include "test.h"

void print_sym_table(const struct sym_table* sym_table)
{
    if(sym_table == NULL)
        return;

    const struct sym_table* current = sym_table;
    while(current != NULL)
    {
        printf("[\"%s\": %u]-->", current->name, current->addr);
        current = current->next;
    }

    printf("[NULL]\n");
}
