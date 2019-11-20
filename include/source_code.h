#pragma once

#include <stdbool.h>
#include <stdint.h>

// Source code structure is implemented as a linked list.
struct source_code
{
    bool empty;
    uint16_t addr;
    char* text;
    struct source_code* next;
};

void source_code_push_back(struct source_code** source_code, uint16_t addr, const char* text);

void source_code_free(struct source_code** source_code);
