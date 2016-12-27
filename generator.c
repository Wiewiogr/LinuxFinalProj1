#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include "common.h"

//timer_t createTimerId;
//struct itimerspec timeUntilCreate;
float averageTime = -1;
float deviation = 0.0;
int maxNumberOfOdbiorniki;
int numberOfOdbiorniki;
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
//    convertFloatToTimeSpec(time,&timeUntilCreate.it_value);
//    setTimer(createTimerId,&timeUntilCreate);

    if(numberOfOdbiorniki < maxNumberOfOdbiorniki)
    {
        char averageValueArg[20];
        sprintf(averageValueArg,"-m%lf",getValueFromMinMax(&averageOdbiorcaMParam));
        char deviationArg[20];
        sprintf(deviationArg,"-d%lf",getValueFromMinMax(&averageOdbiorcaDParam));
        char * odbiornikArgs[] =
        {
            /*name*/
            /*args*/
            (char *) 0
        };
        float odbiornikType = rand() % 4;
    }
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
            printf("ex val: %lf\n", getValueFromMinMax(&averageOdbiorcaLifetime));
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

    //createTimerAndRegisterHandler(&createTimerId,timerHandler);

    //float time = randomizeTime(averageTime,deviation);
    //convertFloatToTimeSpec(time,&timeUntilCreate.it_value);
    //setTimer(createTimerId,&timeUntilCreate);


    while(1)
        pause();

    return 0;
}
