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

timer_t writeTimerId;
struct itimerspec timeUntilWrite;
float averageTime = -1;
float deviation = 0.0;

void timerHandler(int sig, siginfo_t *si, void *uc)
{
    float time = randomizeTime(averageTime,deviation);
    convertFloatToTimeSpec(time,&timeUntilWrite.it_value);
    setTimer(writeTimerId,&timeUntilWrite);

    kill(getpid(),SIGSTOP);

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
    if(optind+1 == argc)
    {
        strcpy(fifoPath,argv[optind]);
        printf("fifo path : %s\n", fifoPath);
    }
    createAndSetExitTimer(&timeUntilEnd, endTimerType);
//
    createTimerAndRegisterHandler(&writeTimerId,timerHandler);

    float time = randomizeTime(averageTime,deviation);
    convertFloatToTimeSpec(time,&timeUntilWrite.it_value);
    setTimer(writeTimerId,&timeUntilWrite);
//
    int fd;
    if((fd = open(fifoPath,O_RDWR))== -1)
        perror("open");

    struct pollfd fds;
    fds.fd = fd;
    fds.events = POLLIN;
    fds.revents = 0;

    int res;
    struct timespec timeBetweenPolls = {0,500000000};

    while(1)
    {
        nanosleep(&timeBetweenPolls,NULL);

        res = poll(&fds,1,0);
        if(res == 1)
        {
            printf("res %d\n revent : %d\n",res,fds.revents);
            if(fds.revents & POLLIN)
            {
                struct timespec buffer;
                read(fds.fd,&buffer,sizeof(buffer));
                showTimeDifferenceReport(&buffer);
            }
            else
            {
                if(checkAndPrintPollErrors(fds.revents))
                    break;
                printf("zeruje revents!!\n");
                fds.revents = 0;
                printf("revent : %d\n",fds.revents);
            }
        }
    }
    close(fd);

    return 0;
}
