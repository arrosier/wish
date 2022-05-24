#include <string.h>

#include "common.h"
#include "macros.h"
#include "stringutils.h"


char* prune(char* str)
{
    size_t length = strnlen(str, MAX_STRING_LENGTH);
    if (length == 0)
    {
        return NULL;
    }

    size_t counter = 0;
    char* result = common_malloc(MAX_STRING_LENGTH + 1);
    for (size_t i = 0; i < length; i++)
    {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
        {
            result[counter] = str[i];
            counter++;

            if (str[i + 1] == ' ' || str[i + 1] == '\t')
            {
                result[counter] = ' ';
                counter++;
            }
        }
    }

    if (counter == 0)
    {
        return NULL;
    }

    if (result[counter - 1] == ' ' || result[counter - 1] == '\t')
    {
        result[counter - 1] = '\0';
        counter--;
    }

    common_realloc(result, counter + 1);
    result[counter] = '\0';
    
    return result;
}

char* get_new_string(const char* s1, const char* s2)
{
    const size_t s1_length = strnlen(s1, MAX_STRING_LENGTH);
    const size_t s2_length = strnlen(s2, MAX_STRING_LENGTH);
    char* result = common_malloc(s1_length + s2_length + 1);

    memcpy(result, s1, s1_length);
    memcpy(result + s1_length, s2, s2_length + 1);

    return result;
}

size_t count_char(char* str, char token)
{
    size_t count = 0;
    for (size_t i = 0; i < strnlen(str, MAX_STRING_LENGTH); i++)
    {
        if (str[i] == token)
        {
            count++;
        }
    }

    return count;
}
