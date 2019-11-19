#pragma once

#include <stdbool.h>
#include <stdint.h>

// Symbol table is implemented as singly linked list.
struct sym_table
{
    char* name;
    uint16_t addr;
    struct sym_table* next;
};

// Returns value of symbol in symbol table.
// If there is no such value, returns UINT16_MAX.
uint16_t sym_table_get(const struct sym_table* sym_table, const char* name);

// Inserts new value at the end of symbol table.
void sym_table_push_back(struct sym_table** sym_table, const char* name, uint16_t addr);

// Deallocates whole symbol table.
void sym_table_free(struct sym_table** sym_table);
