#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "sym_table.h"

// Prints symbol table to stdout.
void print_sym_table(const struct sym_table* sym_table);
