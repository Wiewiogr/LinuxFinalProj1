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

void restoreFile(struct Fifo* fifo)
{
    remove(fifo->path);
    link(fifo->backupPath,fifo->path);
    fifo->isOpened = false;
}

void correctFilePermission(struct Fifo* fifo)
{
    chmod(fifo->path, 00664);
}

void controlHandler(int sig, siginfo_t *si, void *uc)
{
    int runThisTime = 0;
    while(runThisTime++ < numberOfFifos)
    {
        currentFifo %= numberOfFifos;
        {
            struct stat sb;
            if (stat(fifos[currentFifo].path, &sb) == -1)
            {
                printf("%s does not exist, restoring it.\n", fifos[currentFifo].path);
                restoreFile(&fifos[currentFifo++]);
                break;
            }
            if(!S_ISFIFO(sb.st_mode))
            {
                printf("%s is not fifo ,replacing it with new correct one.\n", fifos[currentFifo].path);
                restoreFile(&fifos[currentFifo++]);
                break;
            }
            else if(!(sb.st_mode& S_IWUSR && sb.st_mode & S_IWGRP))
            {
                printf("%s does not have write permission, setting on 00664.\n", fifos[currentFifo].path);
                correctFilePermission(&fifos[currentFifo++]);
                break;
            }
        }

        if(!fifos[currentFifo].isOpened)
        {
            printf("%s opening - ", fifos[currentFifo].path);
            int fd = open(fifos[currentFifo].path,O_RDWR | O_NONBLOCK);
            if(fd != -1)
            {
                printf("succes.\n");
                fifos[currentFifo].isOpened = true;
                fifos[currentFifo].fileDescriptor = fd;
            }
            else
            {
                printf("failed.\n");
            }
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

    if(timeBetweenControls == 0 || strlen(fifoNameTemplate) < 1 || numberOfFifos < 1)
    {
        printf("usage : %s -f <float> -p <string> -c <int> [-d <string>] \n",argv[0]);
        exit(1);
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
        res = poll(&fds,1,-1);

        if(fds.revents & POLLIN)
        {
            struct timespec buffer;
            read(fds.fd,&buffer,sizeof(buffer));
            for(int i = 0; i < numberOfFifos; i++)
            {
                if(fifos[i].isOpened)
                {
                    int result = write(fifos[i].fileDescriptor, &buffer,sizeof(buffer));
                }
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
