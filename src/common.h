#ifndef COMMON_H
#define COMMON_H

#include "typedefs.h"

void print_error_message();

void* common_malloc(const size_t size);

void* common_realloc(void** ptr, const size_t size);

node_t* create_node(char* str);

void free_node_list(node_t* head);

#endif
