#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "common.h"

timer_t stopTimerId;
struct itimerspec timeUntilStop;
float averageTime = -1;
float deviation = 0.0;

void timerHandler(int sig, siginfo_t *si, void *uc)
{
    float time = randomizeTime(averageTime,deviation);
    convertFloatToTimeSpec(time,&timeUntilStop.it_value);
    setTimer(stopTimerId,&timeUntilStop);

    raise(SIGSTOP);
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    clockid_t endTimerType = -1;
    struct itimerspec timeUntilEnd;
    char fifoPath[30];

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
            convertFloatToTimeSpec(0,&timeUntilEnd.it_interval);
            break;
        case 'p':
            endTimerType = CLOCK_PROCESS_CPUTIME_ID;
            convertFloatToTimeSpec(strtof(optarg,NULL),&timeUntilEnd.it_value);
            convertFloatToTimeSpec(0,&timeUntilEnd.it_interval);
            break;
        case 'w':
            endTimerType = CLOCK_REALTIME;
            convertFloatToTimeSpec(strtof(optarg,NULL),&timeUntilEnd.it_value);
            convertFloatToTimeSpec(0,&timeUntilEnd.it_interval);
            break;
        }
    }

    if(averageTime < 0 || (optind+1 != argc))
    {
        fprintf(stderr,"usage : %s -m <float> <path to fifo>  [-d <float>] [-w/-c/-p <float>]\n",argv[0]);
        exit(1);
    }

    strcpy(fifoPath,argv[optind]);

    createAndSetExitTimer(&timeUntilEnd, endTimerType);

    registerHandler(SIGALRM,timerHandler);
    createTimer(&stopTimerId,SIGALRM);

    float time = randomizeTime(averageTime,deviation);
    convertFloatToTimeSpec(time,&timeUntilStop.it_value);
    setTimer(stopTimerId,&timeUntilStop);

    int fd;
    if((fd = open(fifoPath,O_RDWR))== -1)
        perror("open");

    struct pollfd fds = createPollfdStruct(fd);

    int res;
    struct timespec timeBetweenPolls = {0,500000000};
    while(1)
    {
        if(!isFifo(fifoPath))
            break;

        nanosleep(&timeBetweenPolls,NULL);
        res = poll(&fds,1,0);
        if(res == 1)
        {
            if(fds.revents & POLLIN)
            {
                struct timespec buffer;
                read(fds.fd,&buffer,sizeof(buffer));
                showTimeDifferenceReport(&buffer,argv[0], fifoPath);
                fflush(stdout);
            }
            else
            {
                if(isPollError(fds.revents))
                    break;
            }
        }
    }

    close(fd);
    return 0;
}
