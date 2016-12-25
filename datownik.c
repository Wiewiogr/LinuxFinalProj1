#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include "libCommon.h"

#define SIG SIGALRM

timer_t writeTimerId;
timer_t endTimerId;
struct timespec currentTime;
struct itimerspec timeUntilWrite;
float averageTime = -1;
float deviation = 0.0;

void timerHandler(int sig, siginfo_t *si, void *uc)
{
    float time = averageTime + (1.0*rand()/RAND_MAX)*deviation*2-deviation;
    convertFloatToTimeSpec(time,&timeUntilWrite.it_value);

    if (timer_settime(writeTimerId, 0, &timeUntilWrite, NULL) == -1)
        printf("timer_settime error\n");

    clock_gettime(CLOCK_REALTIME,&currentTime);
    printf("time : %lf, currentTime %d.%d\n",time, currentTime.tv_sec, currentTime.tv_nsec);
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

    struct sigaction sa;
    struct sigevent sev;
    struct sigevent sevProgramEnd;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timerHandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIG, &sa, NULL) == -1)
        perror("sigaction");

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIG;
    sev.sigev_value.sival_ptr = &writeTimerId;
    if (timer_create(CLOCK_REALTIME, &sev, &writeTimerId) == -1)
        perror("timer_create");

    timeUntilWrite.it_value.tv_sec = 1;
    timeUntilWrite.it_value.tv_nsec = 0;

    if (timer_settime(writeTimerId, 0, &timeUntilWrite, NULL) == -1)
        perror("timer_settime");


    if(endTimerType != -1)
    {
        sevProgramEnd.sigev_notify = SIGEV_SIGNAL;
        sevProgramEnd.sigev_signo = SIGKILL;
        sevProgramEnd.sigev_value.sival_ptr = &endTimerId;
        if (timer_create(CLOCK_REALTIME, &sevProgramEnd, &endTimerId) == -1)
            perror("timer_create");

        if (timer_settime(endTimerId, 0, &timeUntilEnd, NULL) == -1)
            perror("timer_settime");
    }

    while(1)
        pause();

    return 0;
}
