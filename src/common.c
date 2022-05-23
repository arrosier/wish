#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "macros.h"
#include "typedefs.h"


void print_error_message()
{
    write(STDERR_FILENO, ERROR_MESSAGE, ERROR_MESSAGE_LENGTH);
}

void* common_malloc(const size_t size)
{
    void* result = malloc(size);

    if (result == NULL)
    {
        print_error_message();
        exit(1);
    }
    else
    {
        return result;
    }
}

void* common_realloc(void* ptr, const size_t size)
{
    void* result = realloc(ptr, size);

    if (result == NULL)
    {
        print_error_message();
        exit(1);
    }
    else
    {
        return result;
    }
}

node_t* create_node(char* str)
{
    node_t* new_node = common_malloc(sizeof(node_t));
    size_t length = strnlen(str, MAX_STRING_LENGTH) + 1;
    new_node->str = common_malloc(length);
    new_node->next = NULL;
    memcpy(new_node->str, str, length);

    return new_node;
}

void free_node_list(node_t* head)
{
    node_t* current;
    while ((current = head) != NULL)
    {
        head = head->next;
        free(current->str);
        free(current);
    }
}
