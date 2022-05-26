#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "common.h"
#include "macros.h"
#include "stringutils.h"
#include "typedefs.h"


node_t* path_list;

void execute(FILE* input_stream)
{
    char* buf = NULL;
    size_t n = 0;
    node_t* cmd_list = NULL;
    getline(&buf, &n, input_stream);
    char* pruned_input = prune(buf);
    free(buf);
    /*
    The result of the prune function call is a managed resource. By using strsep on pruned_input below we'll be changing
    where our variable points so we need to remember the original address to free it later. We'll just reuse the buf
    variable to do so.
    */
    buf = pruned_input;
    char* ptr;
    while((ptr = strsep(&pruned_input, "&")) != NULL)
    {
        char* tmp_str = prune(ptr);
        if (tmp_str == NULL)
        {
            // We didn't get any input on one side of our & operator. Don't schedule a command.
            continue;
        }
        node_t* tmp = create_node(tmp_str);
        tmp->next = cmd_list;
        cmd_list = tmp;
        free(tmp_str);
    }

    while (cmd_list != NULL)
    {
        char* input = cmd_list->str;
        char* redirect = index(input, '>');
        if (redirect != NULL)
        {
            *redirect = '\0';
            redirect++;
            redirect = prune(redirect);
            if (input[0] == '\0' || redirect == NULL || count_char(redirect, ' ') != 0 || count_char(redirect, '>') != 0)
            {
                print_error_message();
                cmd_list = cmd_list->next;
                continue;
            }
        }

        size_t num_args = count_char(input, ' ') + 1;
        char* args[num_args + 1];
        args[0] = strsep(&input, " ");
        size_t cmd_length = strnlen(args[0], MAX_STRING_LENGTH);
        if (strncmp(args[0], EXIT, cmd_length) == 0)
        {
            if (num_args != 1)
            {
                print_error_message();
            }
            else
            {
                wish_exit();
            }
        }
        else if (strncmp(args[0], CD, cmd_length) == 0)
        {
            if (num_args != 2)
            {
                print_error_message();
            }
            else
            {
                cd(input);
            }
        }
        else if (strncmp(args[0], PATH, cmd_length) == 0)
        {
            free_node_list(path_list);
            path_list = NULL;
            if (num_args > 1)
            {
                path(&path_list, input);
            }
        }
        else
        {
            for (size_t i = 1; i < num_args; i++)
            {
                args[i] = strsep(&input, " ");
            }

            args[num_args] = NULL;
            bool authorized = false;
            node_t* tmp_list = path_list;
            char* cmd;
            while (tmp_list != NULL)
            {
                cmd = get_new_string(tmp_list->str, args[0]);
                if (access(cmd, X_OK) == 0)
                {
                    authorized = true;
                    break;
                }

                tmp_list = tmp_list->next;
                free(cmd);
            }

            if (authorized)
            {
                pid_t cpid = fork();
                if (cpid == 0)
                {
                    if (redirect != NULL)
                    {
                        int fd = open(redirect, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
                        close(STDOUT_FILENO);
                        close(STDERR_FILENO);
                        dup(fd);
                        dup(fd);
                        close(fd);
                    }

                    execv(cmd, args);
                }
                else
                {
                    free(cmd);
                    free(redirect);
                }
            }
            else
            {
                print_error_message();
            }
        }
        
        cmd_list = cmd_list->next;
    }

    while (wait(NULL) > 0);
    free_node_list(cmd_list);
    free(buf);
}

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        print_error_message();
        exit(1);
    }

    path_list = create_node("/bin/");
    if (argc == 2)
    {
        char* filename = argv[1];
        FILE* fp = fopen(filename, "r");
        if (fp == NULL)
        {
            print_error_message();
            exit(1);
        }
        
        while (feof(fp) == 0)
        {
            execute(fp);
        }
        
        fclose(fp);
    }
    else
    {
        while (1)
        {
            printf("wish> ");
            execute(stdin);
        }
    }

    free_node_list(path_list);

    return 0;
}
