#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "common.h"


timer_t controlTimerId;
struct itimerspec timeUntilControl;
char diagnosticPath[50];
char fifoNameTemplate[50];
int numberOfFifos;
float timeBetweenControls;

struct Fifo* fifos;

int currentFifo;


void controlHandler(int sig, siginfo_t *si, void *uc)
{
    printf("controlling!!!\n");
    while(1)
    {
        currentFifo %= numberOfFifos;
        if(!fifos[currentFifo].isOpened)
        {
            int fd = open(fifos[currentFifo].path,O_RDWR);
            if(fd != -1)
            {
                printf("opening fifo %s\n", fifos[currentFifo].path);
                fifos[currentFifo].isOpened = true;
                fifos[currentFifo].fileDescriptor = fd;
            }
            currentFifo++;
            break;
        }
        else if(!isFifo(fifos[currentFifo].path))
        {
            printf("creating new fifo\n");
            remove(fifos[currentFifo].path);
            link(fifos[currentFifo].backupPath,fifos[currentFifo].path);
            fifos[currentFifo].isOpened = false;
            currentFifo++;
            break;
        }
        currentFifo++;
    }
    setTimer(controlTimerId,&timeUntilControl);
}


int main(int argc, char* argv[])
{
    srand(time(NULL));

    int opt;

    while ((opt = getopt(argc, argv, "f:p:c:d:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            timeBetweenControls = 60.0/strtof(optarg,NULL);
            break;
        case 'p':
            strcpy(fifoNameTemplate,optarg);
            break;
        case 'c':
            numberOfFifos = atoi(optarg);
            break;
        case 'd':
            strcpy(diagnosticPath,optarg);
            break;
        }
    }

    fifos = (struct Fifo*)malloc(numberOfFifos * sizeof(struct Fifo));

    for(int i = 0; i < numberOfFifos;i++)
    {
        sprintf(fifos[i].path,"%s%d",fifoNameTemplate,i);
        sprintf(fifos[i].backupPath,"./.secret/%d",i);
        fifos[i].fileDescriptor = -1;
        fifos[i].isOpened = false;
    }

    createBackupFiles(fifos,numberOfFifos);

    struct pollfd fds = createPollfdStruct(0);
    int res;

    registerHandler(SIGALRM,controlHandler);
    createTimer(&controlTimerId,SIGALRM);

    convertFloatToTimeSpec(timeBetweenControls,&timeUntilControl.it_value);
    setTimer(controlTimerId,&timeUntilControl);

    while(1)
    {
        //if(!isFifo(fifoPath))
        //{
        //    printf("Not fifo ;/\n");
        //    break;
        //}
        res = poll(&fds,1,-1);

        if(fds.revents & POLLIN)
        {
            struct timespec buffer;
            read(fds.fd,&buffer,sizeof(buffer));
            for(int i = 0; i < numberOfFifos; i++)
            {
                if(fifos[i].isOpened)
                {
               //     printf("writing to file\n");
                    int result = write(fifos[i].fileDescriptor, &buffer,sizeof(buffer));
                    //printf(" writing res : %d", result);
                }
                //write(1, &buffer,sizeof(buffer));
            }
        }
        else
        {
            if(checkAndPrintPollErrors(fds.revents))
                break;
        }
    }
    free(fifos);

    return 0;
}
