#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include "common.h"

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

    int fd;
    if((fd = open(fifoPath,O_RDWR))== -1)
        perror("open");

    struct pollfd fds;
    fds.fd = fd;
    fds.events = POLLIN;
    fds.revents = 0;

    int res;

    while(1)
    {
        res = poll(&fds,1,-1);
        printf("res %d\n revent : %d\n",res,fds.revents);
        if(fds.revents & POLLIN)
        {
            char buffer[100];
            read(fds.fd,buffer,100);
            printf("from fd : %d read : %s\n",fds.fd,buffer);
        }
        else if(fds.revents & POLLERR)
        {
            printf("from fd %d POLLERR",fds.fd);
        }
        else if(fds.revents & POLLNVAL)
        {
            printf("from fd %d POLLNVAL",fds.fd);
        }
        else if(fds.revents & POLLHUP)
        {
            printf("from fd %d POLLHUP",fds.fd);
        }
        printf("zeruje revents!!\n");
        fds.revents = 0;
        printf("revent : %d\n",fds.revents);
    }

    return 0;
}
