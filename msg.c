#include <stdio.h>
#include <string.h>
#include "user.h"
#include "msg.h"
#include "commands.h"
#include "regexutils.h"

void parseMessage(User *bot, char *msg)
{
    const char *matchedString;
    enum Commands command;

    if (strstr(msg, "PING") != NULL)
    {
        command = PONG;
        executeCommand(command, bot);
    } 
    else if (strstr(msg, "PRIVMSG") != NULL)
    {
        char *regexPattern = "^:(.*)!(.*) PRIVMSG (.*) :\\!(.*)$";
        char *msgData[4];

        // Will return false if pattern isnt compiled or doesnt match
        if (!executeRegex(regexPattern, msg, msgData))
        {
            //printf("Failed to parse message\n");
            return;
        }

        printf("%s %s %s %s\n", msgData[0], msgData[2], msgData[3], msgData[4]);

        char *token = strtok(msgData[4], " ");
        char *cmdString = token;
        char *parameters[32];
        token = strtok(NULL, " ");
        int paramIndex = 0;
        while (token != NULL)
        {
            printf("%s\n", token);
            parameters[paramIndex] = token;
            token = strtok(NULL, " ");
            paramIndex++;
        }

        command = PRIVMSG;
        executeCommand(command, bot, msgData[1], cmdString, parameters);
        pcre_free_substring(matchedString);
    }
}

int isNick(char *nick, const char *msgData)
{
    if (strcmp(nick, msgData) == 0)
        return 1;
    return 0;
}


