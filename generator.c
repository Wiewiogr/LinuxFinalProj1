#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include "common.h"

//timer_t writeTimerId;
//struct itimerspec timeUntilWrite;
float averageTime = -1;
float deviation = 0.0;
int maxNumberOfOdbiorniki;
int maxNumberOfFifos;
char diagnosticPath[50];
char outputPath[50];
char fifoNameTemplate[50];
struct minMaxValues averageOdbiorcaLifetime;
struct minMaxValues averageOdbiorcaMParam;
struct minMaxValues averageOdbiorcaDParam;

void timerHandler(int sig, siginfo_t *si, void *uc)
{
//    float time = randomizeTime(averageTime,deviation);
//    convertFloatToTimeSpec(time,&timeUntilWrite.it_value);
//
//    setTimer(writeTimerId,&timeUntilWrite);
}

int main(int argc, char* argv[])
{
    srand(time(NULL));

    int opt;

    while ((opt = getopt(argc, argv, "B:D:M:L:O:p:c:l:m:d:")) != -1)
    {
        switch (opt)
        {
        case 'B':
            maxNumberOfOdbiorniki = atoi(optarg);
            break;
        case 'D':
            averageTime = strtof(optarg,NULL);
            break;
        case 'M':
            deviation = strtof(optarg,NULL);
            break;
        case 'L':
            strcpy(diagnosticPath,optarg);
            break;
        case 'O':
            strcpy(outputPath,optarg);
            break;
        case 'p':
            strcpy(fifoNameTemplate,optarg);
            break;
        case 'c':
            maxNumberOfFifos = atoi(optarg);
            break;
        case 'l':
            averageOdbiorcaLifetime = getMinMaxValuesFromString(optarg);
            break;
        case 'm':
            averageOdbiorcaMParam = getMinMaxValuesFromString(optarg);
            break;
        case 'd':
            averageOdbiorcaDParam = getMinMaxValuesFromString(optarg);
            break;
        }
    }

    struct sigaction sa; // ???

    //createTimerAndRegisterHandler(&writeTimerId,timerHandler);

    //float time = randomizeTime(averageTime,deviation);
    //convertFloatToTimeSpec(time,&timeUntilWrite.it_value);
    //setTimer(writeTimerId,&timeUntilWrite);


    while(1)
        pause();

    return 0;
}
