#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"
#include "common.h"
#include "macros.h"
#include "stringutils.h"


void wish_exit()
{
    exit(0);
}

void cd(char* dir)
{
    if (chdir(dir) != 0)
    {
        print_error_message();
    }
}

void path(node_t** path_list, char* new)
{
    size_t num_args = count_words(new);
    node_t* tmp;
    char* arg;
    for (size_t i = 0; i < num_args; i++)
    {
        arg = strsep(&new, " ");
        tmp = create_node(arg);
        tmp->next = *path_list;
        *path_list = tmp;
    }
}
