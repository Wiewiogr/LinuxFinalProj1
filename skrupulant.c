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
            break;
        case 'd':
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

    int fd;
    if((fd = open(fifoPath,O_RDWR))== -1)
        perror("open");

    struct pollfd fds = createPollfdStruct(fd);
    int res;

    while(1)
    {
        if(!isFifo(fifoPath))
        {
            printf("Not fifo ;/\n");
            break;
        }
        res = poll(&fds,1,-1); // a moze -1=>0 ? wtedy powyższy warunek zadziała, ale procek będzie musiał cały czas działać ;/
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
        }
    }
    close(fd);

    return 0;
}
