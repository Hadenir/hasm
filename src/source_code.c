#include "source_code.h"

#include <stdlib.h>
#include <string.h>

void source_code_push_back(struct source_code** source_code, uint16_t addr, const char* text)
{
    if(text == NULL)
        return;

    if(*source_code == NULL)  // Create new entry.
    {
        *source_code = malloc(sizeof(struct source_code));
        (*source_code)->addr = addr;
        (*source_code)->next = NULL;
        size_t text_sz = strlen(text) + 1;
        char* new_text = malloc(text_sz);
        memcpy(new_text, text, text_sz);
        if(new_text[text_sz - 2] == '\n')
        {
            new_text[text_sz - 2] = '\0';
        }
        else if(new_text[text_sz - 2] == '\r')
        {
            new_text[text_sz - 2] = '\0';
            new_text[text_sz - 3] = '\0';
        }
        (*source_code)->text = new_text;
        (*source_code)->empty = strlen(new_text) == 0 || new_text[0] == '#';

        return;
    }

    // Append new entry.
    struct source_code* current = *source_code;
    while(current->next != NULL)
        current = current->next;

    struct source_code* new = malloc(sizeof(struct source_code));
    new->addr = addr;
    new->next = NULL;
    size_t text_sz = strlen(text) + 1;
    char* new_text = malloc(text_sz);
    memcpy(new_text, text, text_sz);
    if(new_text[text_sz - 2] == '\n')
    {
        new_text[text_sz - 2] = '\0';
    }
    else if(new_text[text_sz - 2] == '\r')
    {
        new_text[text_sz - 2] = '\0';
        new_text[text_sz - 3] = '\0';
    }
    new->text = new_text;
    new->empty = strlen(new_text) == 0 || new_text[0] == '#';
    current->next = new;
}

void source_code_free(struct source_code** source_code)
{
    if(*source_code == NULL)
        return;

    struct source_code* current = *source_code;
    struct source_code* next;
    while(current != NULL)
    {
        free(current->text);
        next = current->next;
        free(current);

        current = next;
    }

    *source_code = NULL;
}
