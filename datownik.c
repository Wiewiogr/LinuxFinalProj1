#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#define SIG SIGALRM

timer_t writeTimerId;
timer_t endTimerId;
struct timespec currentTime;
struct itimerspec timeUntilWrite;

void timerHandler(int sig, siginfo_t *si, void *uc)
{
    timeUntilWrite.it_value.tv_sec = 1;
    timeUntilWrite.it_value.tv_nsec = 0;

    if (timer_settime(writeTimerId, 0, &timeUntilWrite, NULL) == -1)
        printf("timer_settime error\n");

    clock_gettime(CLOCK_REALTIME,&currentTime);
    printf("currentTime %d.%d\n",currentTime.tv_sec, currentTime.tv_nsec);
}

int main(int argc, char* argv[])
{
    float time;
    float averageTime = -1;
    float deviation = 0.0;
    clockid_t endTimerType = -1;
    struct itimerspec timeUntilEnd;

    int opt;

    while ((opt = getopt(argc, argv, "m:d:w:c:p:")) != -1)
    {
//w - real c - monotoniczny p - lokalnmy procesu
        switch (opt)
        {
        case 'm':
            averageTime = strtof(optarg,NULL);
            break;
        case 'd':
            deviation = strtof(optarg,NULL);
            break;
        case 'w':
            endTimerType = CLOCK_REALTIME;
            time = strtof(optarg,NULL);
            timeUntilEnd.it_value.tv_sec = floor(time);
            timeUntilEnd.it_value.tv_nsec = (time - floor(time))*1000000000;
           //timeUntilEnd.it_value.tv_sec = 2;
           //timeUntilEnd.it_value.tv_nsec = 2;
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
        printf("sigaction error\n");

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIG;
    sev.sigev_value.sival_ptr = &writeTimerId;
    if (timer_create(CLOCK_REALTIME, &sev, &writeTimerId) == -1)
        printf("timer_create error\n");

    timeUntilWrite.it_value.tv_sec = 1;
    timeUntilWrite.it_value.tv_nsec = 0;

    if (timer_settime(writeTimerId, 0, &timeUntilWrite, NULL) == -1)
        printf("timer_settime error\n");

    if(endTimerType != -1)
    {
        sevProgramEnd.sigev_notify = SIGEV_SIGNAL;
        sevProgramEnd.sigev_signo = SIGTERM;
        sevProgramEnd.sigev_value.sival_ptr = &endTimerId;
        if (timer_create(CLOCK_REALTIME, &sevProgramEnd, &endTimerId) == -1)
            printf("timer_create error\n");

        if (timer_settime(endTimerId, 0, &timeUntilEnd, NULL) == -1)
            perror("timer_settime");
    }

    while(1)
        pause();

    return 0;
}
