#include "common.h"

float randomizeTime(float averageTime ,float deviation)
{
    return averageTime + (1.0*rand()/RAND_MAX)*deviation*2-deviation;
}

void convertFloatToTimeSpec(float time, struct timespec * ts)
{
    ts->tv_sec = floor(time);
    ts->tv_nsec = (time - floor(time))*1000000000;
}

void createAndSetExitTimer(struct itimerspec *timeUntilExit, clockid_t type)
{
    if(type != -1)
    {
        struct sigaction sa;
        timer_t endTimerId;
        struct sigevent sevProgramEnd;
        sevProgramEnd.sigev_notify = SIGEV_SIGNAL;
        sevProgramEnd.sigev_signo = SIGKILL;
        sevProgramEnd.sigev_value.sival_ptr = &endTimerId;
        if (timer_create(type, &sevProgramEnd, &endTimerId) == -1)
            perror("timer_create");

        if (timer_settime(endTimerId, 0, timeUntilExit, NULL) == -1)
            perror("timer_settime");
    }

}

void createTimerAndRegisterHandler(timer_t *timerId, void(*handler)(int, siginfo_t*, void*))
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        perror("sigaction");

    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGALRM;
    sev.sigev_value.sival_ptr = timerId;
    if (timer_create(CLOCK_REALTIME, &sev, timerId) == -1)
        perror("timer_create");
}

extern void setTimer(timer_t timerId,struct itimerspec *timeSpec)
{
    if (timer_settime(timerId, 0, timeSpec, NULL) == -1)
        perror("timer_settime");

}

extern void showTimeDifferenceReport(struct timespec* time)
{
    printf("!!REPORT!!! sec : %d nsec: %d \n",time->tv_sec, time->tv_nsec);

}
