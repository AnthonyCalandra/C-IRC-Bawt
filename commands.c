#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "user.h"
#include "commands.h"
#include "msgstream.h"
#include "curl/curl.h"
#include "regexutils.h"

#define BUFFER_SIZE     30000

void executeCommand(enum Commands command, User *bot, ...)
{
    va_list a_list;
    va_start(a_list, bot);
    char *sender, *cmdStr;
    char **parameters;
    switch (command)
    {
        case PONG:
            pong(bot);
            break;
        case PRIVMSG:
            sender = va_arg(a_list, char *);
            cmdStr = va_arg(a_list, char *);
            parameters = va_arg(a_list, char **);
            if (strcmp("speak", cmdStr) == 0 && strcmp("Anthony`", sender) == 0)
                privmsg(bot, sender, parameters[0]);
            else if (strcmp("rolldice", cmdStr) == 0)
                rolldice(bot, sender, atoi(parameters[0]));
            else if (strcmp("die", cmdStr) == 0 && strcmp("Anthony`", sender) == 0)
                killBot(bot);
            else if (strcmp("coolmeter", cmdStr) == 0)
                coolmeter(bot, sender, parameters[0]);
            else if (strcmp("fight", cmdStr) == 0)
                fight(bot, sender, parameters[0], parameters[1]);
            else if (strcmp("getlatestpost", cmdStr) == 0)
                getLatestRlPost(bot, sender);
            break;
    }

    va_end(a_list);
}

void pong(User *bot)
{
    char command[512];
    sprintf(command, "PONG %s\n\r", bot->channel);
    sendMessage(bot->client, command);
    printf("PONG\n");
}

void privmsg(User *bot, char *sender, char *message)
{
    char command[512];
    replaceUnderscores(message);
    sprintf(command, "PRIVMSG #%s %s\n\r", bot->channel, message);
    sendMessage(bot->client, command);
    printf("Sent message - Sender: %s\n", sender);
}

void rolldice(User *bot, char *sender, int num)
{
    char command[512];
    char reply[64], *result;

    if (num < 1 || num > 6) {
        sprintf(command, "PRIVMSG #%s USAGE: !rolldice <1-6>\n\r", bot->channel);
        sendMessage(bot->client, command);
        printf("Played dice - Sender: %s\n", sender);
        return;
    }

    srand(time(NULL));
    int botnum = rand() % 6 + 1;
    if (botnum == num)
        result = "You got it!";
    else
        result = "Better luck next time.";

    sprintf(reply, "You chose: %d. I rolled a: %d. %s", num, botnum, result);
    sprintf(command, "PRIVMSG #%s %s\n\r", bot->channel, reply);
    sendMessage(bot->client, command);
    printf("Played dice - Sender: %s\n", sender);
}

void fight(User *bot, char *sender, char *opponent1, char *opponent2)
{
    char command[512], reply[128];

    srand(time(NULL));
    int chance = rand() % 2 + 1;

    if (chance == 1) {
        sprintf(reply, "%s lost and didn't stand a chance.", opponent2);
    } else {
        sprintf(reply, "Jumping roundhouse kick to the face and body slam crushed %s with great power.", opponent1);
    }

    sprintf(command, "PRIVMSG #%s %s\n\r", bot->channel, reply);
    sendMessage(bot->client, command);
    printf("Match fought - %d\n", strlen(reply));
}

void coolmeter(User *bot, char *sender, char *recipient)
{
    char command[512], reply[64];

    srand(time(NULL));
    int cool = rand() % 101;
    sprintf(reply, "%s, you are %d%% cool. ", recipient, cool);
    if (cool > 80)
        strcat(reply, "Almost as cool as cboy.");
    else if (cool > 60)
        strcat(reply, "Boss.");
    else if (cool > 50)
        strcat(reply, "Barely cool.");
    else if (cool > 30)
        strcat(reply, "Average moparscaper.");
    else if (cool < 30)
        strcat(reply, "/quit Use_it.");

    sprintf(command, "PRIVMSG #%s %s\n\r", bot->channel, reply);
    sendMessage(bot->client, command);
    printf("Activated coolmeter\n");
}

void getLatestRlPost(User *bot, char *sender)
{
    char command[512], *thread[2], *lastPoster[2];
    char *regexPattern = "<strong>Title:</strong>  <a href=\"(.*)\">(.*)</a></div>";
    char *regexPattern2 = "<strong>Last post by:</strong> <a href=\"(.*)\">(.*)</a></div>";
    struct curlBuffer buffer;
    char *data = (char *) malloc((size_t) BUFFER_SIZE);
    buffer.data = data;
    buffer.pos = 0;

    CURL *curlHandle = curl_easy_init();
    curl_easy_setopt(curlHandle, CURLOPT_URL, "http://www.runelocus.com/forums");
    curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, &curlCallback);
    curl_easy_perform(curlHandle);
    buffer.data[buffer.pos] = '\0';

    if (executeRegex(regexPattern, buffer.data, thread) &&
            executeRegex(regexPattern2, buffer.data, lastPoster))
    {
        sprintf(command, "PRIVMSG #%s Title: %s Last Post by: %s\n\r", bot->channel, thread[2], lastPoster[2]);
        sendMessage(bot->client, command);
        printf("Retrieved latest post - Sender: %s\n", sender);
    }

    free(data);
    curl_easy_cleanup(curlHandle);
}

size_t curlCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    struct curlBuffer *result = (struct curlBuffer *) userdata;
    if (result->pos + size * nmemb >= BUFFER_SIZE - 1) {
        printf("curl error: too small buffer\n");
        return 0;
    }

    memcpy(result->data + result->pos, ptr, size * nmemb);
    result->pos += size * nmemb;
    return size * nmemb;
}

void killBot(User *bot)
{
    char command[512];
    sprintf(command, "PRIVMSG #%s RIP %s\n\r", bot->channel, bot->nick);
    sendMessage(bot->client, command);
    bot->isAlive = 0;
    curl_global_cleanup();
    printf("Killed bot (lol)\n");
}

void replaceUnderscores(char *msg)
{
    int i;
    for (i = 0; i < strlen(msg); i++)
        if (msg[i] == '_')
            msg[i] = ' ';
}
