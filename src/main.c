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


int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        print_error_message();
        exit(1);
    }

    char* buf = NULL;
    size_t n = 0;
    char* pruned_input;
    char* cmd;
    node_t* path_list = create_node("/bin/");
    while (1)
    {
        printf("wish> ");
        getline(&buf, &n, stdin);
        pruned_input = prune(buf);
        free(buf);
        if (pruned_input == NULL)
        {
            print_error_message();
        }

        size_t line_length = strnlen(pruned_input, MAX_STRING_LENGTH);
        if (line_length > 0)
        {
            size_t num_args = count_words(pruned_input);
            char* args[num_args + 1];
            args[0] = strsep(&pruned_input, " ");

            if (strncmp(args[0], EXIT, EXIT_LENGTH) == 0)
            {
                wish_exit();
            }
            else if (strncmp(args[0], CD, CD_LENGTH) == 0)
            {
                cd(pruned_input);
            }
            else if (strncmp(args[0], PATH, PATH_LENGTH) == 0)
            {
                free_node_list(path_list);
                path_list = NULL;
                path(&path_list, pruned_input);
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
                    }
                }
                else
                {
                    print_error_message();
                }
            }
            
            // Reset for the next loop
            /*
            The pruned_input variable is managed; however, we call strsep on it until it is reduced to a NULL pointer. Thus, it no longer needs
            to be freed. We haven't cleaned up our managed resource though - that pointer now resides at the first position of our args[] array.
            So we have to release that instead.
            */
            free(args[0]);
            buf = NULL;
            n = 0;
        }
        else
        {
            print_error_message();
        }
    }

    free_node_list(path_list);

    return 0;
}
