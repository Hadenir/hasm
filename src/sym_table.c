#include "sym_table.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

uint16_t sym_table_get(const struct sym_table* sym_table, const char* name)
{
    if(sym_table == NULL)
        return UINT16_MAX;

    const struct sym_table* current = sym_table;
    while(current != NULL)
    {
        if(strcmp(current->name, name) == 0)
            return current->addr;

        current = current->next;
    }

    return UINT16_MAX;
}

void sym_table_push_back(struct sym_table** sym_table, const char* name, uint16_t addr)
{
    if(name == NULL)
        return;

    if(*sym_table == NULL)  // Create new symbol table.
    {
        *sym_table = malloc(sizeof(struct sym_table));
        (*sym_table)->addr = addr;
        (*sym_table)->next = NULL;
        size_t name_sz = strlen(name) + 1;
        char* new_name = malloc(name_sz);
        strncpy(new_name, name, name_sz);
        (*sym_table)->name = new_name;

        return;
    }

    // Append new symbol to existing table.
    struct sym_table* current = *sym_table;
    while(current->next != NULL)
        current = current->next;

    struct sym_table* new = malloc(sizeof(struct sym_table));
    new->addr = addr;
    new->next = NULL;
    size_t name_sz = strlen(name) + 1;
    char* new_name = malloc(name_sz);
    strncpy(new_name, name, name_sz);
    new->name = new_name;
    current->next = new;
}

void sym_table_free(struct sym_table* sym_table)
{
    if(sym_table == NULL)
        return;

    struct sym_table* current = sym_table;
    struct sym_table* next;
    while(current != NULL)
    {
        free(current->name);
        next = current->next;
        free(current);

        current = next;
    }

    sym_table = NULL;
}
