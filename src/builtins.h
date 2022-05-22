#ifndef BUILTINS_H
#define BUILTINS_H

#include "typedefs.h"


void wish_exit();

void cd(char* dir);

void path(node_t** path_list, char* new);

#endif
