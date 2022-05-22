#ifndef TYPEDEFS_H
#define TYPEDEFS_H


typedef struct node
{
    char* path_str;
    struct node* next;
} node_t;

#endif