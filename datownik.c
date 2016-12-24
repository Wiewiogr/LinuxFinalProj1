#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#define SIG SIGALRM

timer_t timerid;
struct timespec currentTime;
struct itimerspec its;

void timerHandler(int sig, siginfo_t *si, void *uc)
{
    its.it_value.tv_sec = 2;
    its.it_value.tv_nsec = 0;

    if (timer_settime(timerid, 0, &its, NULL) == -1)
        printf("timer_settime error\n");

    clock_gettime(CLOCK_REALTIME,&currentTime);
    printf("currentTime %d.%d\n",currentTime.tv_sec, currentTime.tv_nsec);
}

int main(int argc, char* argv[])
{
    float averageTime = -1;
    float deviation = 0.0;

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
        }
    }

    if(averageTime < 0)
    {
        printf("Usage : %s -m float [-d float]\n",argv[0]);
        return 0;
    }

    struct sigaction sa;
    struct sigevent sev;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timerHandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIG, &sa, NULL) == -1)
        printf("sigaction error\n");

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIG;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
        printf("timer_create error\n");

    its.it_value.tv_sec = 2;
    its.it_value.tv_nsec = 0;

    if (timer_settime(timerid, 0, &its, NULL) == -1)
        printf("timer_settime error\n");


    while(1)
        pause();

    return 0;
}
