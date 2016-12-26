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

timer_t destructionTimerId;
struct itimerspec timeUntilDestruction;
float averageTime = -1;
float deviation = 0.0;
char fifoPath[30];

void removeFilesWritePriviliges(char* path)
{
    printf("zabierz prawa do zapisu\n");
    chmod(path,00444);
}

void removeFile(char* path)
{
    remove(path);
    printf("usun plik\n");
}

void replaceFifoWithRegularFile(char* path)
{
    printf("podmien plik\n");
    remove(path);
    creat(path,00666);
}

void timerHandler(int sig, siginfo_t *si, void *uc)
{
    float time = randomizeTime(averageTime,deviation);
    convertFloatToTimeSpec(time,&timeUntilDestruction.it_value);
    setTimer(destructionTimerId,&timeUntilDestruction);
    int choice = rand() % 3;

    if(isFifo(fifoPath))
    {
        switch(choice)
        {
        case 0:
            removeFilesWritePriviliges(fifoPath);
            break;
        case 1:
            removeFile(fifoPath);
            break;
        case 2:
            replaceFifoWithRegularFile(fifoPath);
            break;
        }
    }
    else
        printf("not fifo ;/\n");
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

    createTimerAndRegisterHandler(&destructionTimerId,timerHandler);

    float time = randomizeTime(averageTime,deviation);
    convertFloatToTimeSpec(time,&timeUntilDestruction.it_value);
    setTimer(destructionTimerId,&timeUntilDestruction);

    while(1)
        pause();

    return 0;
}
