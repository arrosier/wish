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
#include "typedefs.h"       // Included just to be explicit


node_t* path_list;

void execute(FILE* input)
{
    char* buf = NULL;
    size_t n = 0;
    char* pruned_input;
    char* cmd;

    getline(&buf, &n, input);
    pruned_input = prune(buf);
    free(buf);
    bool valid_redirect = true;
    char* redirect = index(pruned_input, '>');
    if (redirect != NULL)
    {
        *redirect = '\0';
        redirect++;
        redirect = prune(redirect);
        char* tmp = pruned_input;
        pruned_input = prune(tmp);
        free(tmp);

        valid_redirect = redirect != NULL && index(redirect, '>') == NULL && count_words(redirect) == 1;
    }
    if (pruned_input != NULL && valid_redirect)
    {
        size_t num_args = count_words(pruned_input);
        char* args[num_args + 1];
        args[0] = strsep(&pruned_input, " ");

        if (strncmp(args[0], EXIT, EXIT_LENGTH) == 0)
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
        else if (strncmp(args[0], CD, CD_LENGTH) == 0)
        {
            if (num_args != 2)
            {
                print_error_message();
            }
            else
            {
                cd(pruned_input);
            }
        }
        else if (strncmp(args[0], PATH, PATH_LENGTH) == 0)
        {
            free_node_list(path_list);
            path_list = NULL;
            if (num_args > 1)
            {
                path(&path_list, pruned_input);
            }
        }
        else
        {
            bool authorized = false;
            node_t* tmp = path_list;
            while (tmp != NULL)
            {
                cmd = get_new_string(tmp->path_str, args[0]);
                if (access(cmd, X_OK) == 0)
                {
                    authorized = true;
                    break;
                }

                tmp = tmp->next;
                free(cmd);
            }

            if (authorized)
            {
                int stderr_backup;
                int stdout_backup;
                if (redirect != NULL)
                {
                    stderr_backup = dup(STDERR_FILENO);
                    stdout_backup = dup(STDOUT_FILENO);
                    freopen(redirect, "w+", stdout);
                    freopen(redirect, "w+", stderr);
                }

                int wstatus;
                pid_t cpid = fork();
                if (cpid == 0)
                {
                    for (size_t i = 1; i < num_args; i++)
                    {
                        args[i] = strsep(&pruned_input, " ");
                    }

                    args[num_args] = NULL;
                    execv(cmd, args);
                }
                else
                {
                    pid_t w = waitpid(cpid, &wstatus, WUNTRACED);
                    free(cmd);
                    if (w == -1)
                    {
                        print_error_message();
                    }

                    if (redirect != NULL)
                    {
                        fflush(stderr);
                        fflush(stdout);
                        dup2(stderr_backup, STDERR_FILENO);
                        dup2(stdout_backup, STDOUT_FILENO);
                        close(stderr_backup);
                        close(stdout_backup);
                    }
                }
            }
            else
            {
                print_error_message();
            }
        }

        /*
        The pruned_input variable is managed; however, we call strsep on it until it is reduced to a NULL pointer. Thus, it no longer needs
        to be freed. We haven't cleaned up our managed resource though - that pointer now resides at the first position of our args[] array.
        So we have to release that instead.
        */
        free(args[0]);
    }
    else
    {
        print_error_message();
    }
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
