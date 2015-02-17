#include <stdio.h>
#include <pcre.h>
#include <string.h>

// TODO: Study regex
int executeRegex(char *regexPattern, char *str, const char **data)
{
    pcre *compiledRegex;
    const char *regexErrorStr;
    int regexErrorOffset;
    int executedRegex;
    int subStrVec[30];

    compiledRegex = pcre_compile(regexPattern, 0, &regexErrorStr, &regexErrorOffset, NULL);
    if (compiledRegex == NULL)
    {
        printf("Problem compiling regex pattern\n");
        return 0;
    }

    executedRegex = pcre_exec(compiledRegex, 0, str, strlen(str), 0, 0, subStrVec, 30);
    if (executedRegex < 0)
    {
        printf("Pattern didnt match: %s\n", regexPattern);
        return 0;
    }
    else
    {
        int i;
        for (i = 0; i < executedRegex; i++)
        {
            pcre_get_substring(str, subStrVec, executedRegex, i, &data[i]);
        }
    }

    return 1;
}