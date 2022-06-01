#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins.h"
#include "common.h"
#include "macros.h"
#include "stringutils.h"
#include "typedefs.h"


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
    size_t num_args = count_char(new, ' ') + 1;
    node_t* tmp;
    char* arg;
    for (size_t i = 0; i < num_args; i++)
    {
        arg = strsep(&new, " ");
        size_t length = strnlen(arg, MAX_STRING_LENGTH);
        if (arg[length - 2] != '/')
        {
            arg = get_new_string(arg, "/");
        }
        
        tmp = create_node(arg);
        tmp->next = *path_list;
        *path_list = tmp;
        free(arg);
    }
}
