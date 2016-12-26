#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include "common.h"

timer_t writeTimerId;
struct timespec currentTime;
struct itimerspec timeUntilWrite;
float averageTime = -1;
float deviation = 0.0;

void timerHandler(int sig, siginfo_t *si, void *uc)
{
    float time = randomizeTime(averageTime,deviation);
    convertFloatToTimeSpec(time,&timeUntilWrite.it_value);

    setTimer(writeTimerId,&timeUntilWrite);

    clock_gettime(CLOCK_REALTIME,&currentTime);
    write(1,&currentTime,sizeof(currentTime));
    fflush(stdout);
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    clockid_t endTimerType = -1;
    struct itimerspec timeUntilEnd;

    int opt;

    while ((opt = getopt(argc, argv, "m:d:w:c:p:")) != -1)
    {
        switch (opt)
        {
        case 'm':
            averageTime = strtof(optarg,NULL);
            break;
        case 'd':
            deviation = strtof(optarg,NULL);
            break;
        case 'c':
            endTimerType = CLOCK_MONOTONIC;
            convertFloatToTimeSpec(strtof(optarg,NULL),&timeUntilEnd.it_value);
            break;
        case 'p':
            endTimerType = CLOCK_PROCESS_CPUTIME_ID;
            convertFloatToTimeSpec(strtof(optarg,NULL),&timeUntilEnd.it_value);
            break;
        case 'w':
            endTimerType = CLOCK_REALTIME;
            convertFloatToTimeSpec(strtof(optarg,NULL),&timeUntilEnd.it_value);
            break;
        }
    }

    if(averageTime < 0)
    {
        printf("Usage : %s -m float [-d float]\n",argv[0]);
        return 0;
    }

    struct sigaction sa; // ???

    createTimerAndRegisterHandler(&writeTimerId,timerHandler);

    float time = randomizeTime(averageTime,deviation);
    convertFloatToTimeSpec(time,&timeUntilWrite.it_value);
    setTimer(writeTimerId,&timeUntilWrite);

    createAndSetExitTimer(&timeUntilEnd, endTimerType);

    while(1)
        pause();

    return 0;
}
