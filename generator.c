#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common.h"

timer_t createTimerId;
struct itimerspec timeUntilCreate;
float averageTime = -1;
float deviation = 0.0;
int maxNumberOfOdbiorniki;
int numberOfOdbiorniki;
int maxNumberOfFifos;
int outputFileDescriptor = -1;
char diagnosticPath[50];
char outputPath[50];
char fifoNameTemplate[50];
struct minMaxValues averageOdbiorcaLifetime;
struct minMaxValues averageOdbiorcaMParam;
struct minMaxValues averageOdbiorcaDParam;


void createTimerHandler(int sig, siginfo_t *info, void *context)
{
    float time = randomizeTime(averageTime,deviation);
    convertFloatToTimeSpec(time,&timeUntilCreate.it_value);
    setTimer(createTimerId,&timeUntilCreate);

    if(numberOfOdbiorniki < maxNumberOfOdbiorniki)
    {
        if(fork() == 0)
        {
            char averageValueArg[20];
            sprintf(averageValueArg,"-m%lf",getValueFromMinMax(&averageOdbiorcaMParam));
            char deviationArg[20];
            sprintf(deviationArg,"-d%lf",getValueFromMinMax(&averageOdbiorcaDParam));
            char lifeTimeOpt;
            switch(rand()%3)
            {
            case 0:
                lifeTimeOpt = 'c';
                break;
            case 1:
                lifeTimeOpt = 'p';
                break;
            case 2:
                lifeTimeOpt = 'w';
                break;
            }
            char lifeTimeArg[25];
            sprintf(lifeTimeArg,"-%c%lf",lifeTimeOpt, getValueFromMinMax(&averageOdbiorcaLifetime));
            char odbiornikName[20];
            switch(1)//(rand() % 4)
            {
            case 0:
                strcpy(odbiornikName,"./wandal.o");
                break;
            case 1:
            case 2:
                strcpy(odbiornikName,"./len.o");
                break;
            case 3:
                strcpy(odbiornikName,"./skrupulant.o");
                break;
            }
            char fifoArg[30];
            sprintf(fifoArg,"%s%d",fifoNameTemplate,(rand() % maxNumberOfFifos));
            printf("%s %s %s %s %s\n", odbiornikName, averageValueArg, deviationArg, lifeTimeArg, fifoArg);
            char * odbiornikArgs[] =
            {
                odbiornikName,
                averageValueArg,
                deviationArg,
                lifeTimeArg,
                fifoArg,
                (char *) 0
            };
            char *newenviron[] = { NULL };

            if(outputFileDescriptor != -1)
            {
                dup2(outputFileDescriptor,1);
            }

            if(execve(odbiornikName,odbiornikArgs,newenviron) == -1)
            {
                printf("zjeblo sie \n");
            }
            exit(1);
        }
        numberOfOdbiorniki++;
        printf("ilosc czildrenow %d/%d\n",numberOfOdbiorniki,maxNumberOfOdbiorniki);

    }
}

void childSignalHandler(int sig, siginfo_t *info, void *context)
{
    int code = info->si_code;
    if(code == CLD_KILLED || code == CLD_EXITED)
    {
        printf("Child killed or exited\n");
        numberOfOdbiorniki--;
    }
    else if(code == CLD_STOPPED)
    {
        printf("Child stopped\n");
        timer_t continueTimer;

        createTimerWithArgument(&continueTimer,SIGUSR1,info->si_pid);
        float time = randomizeTime(averageTime,deviation);
        convertFloatToTimeSpec(time,&timeUntilCreate.it_value);
        setTimer(continueTimer,&timeUntilCreate);

    }
}

void childContinuationHandler(int sig, siginfo_t *info, void *context)
{
    int childPid = info->si_value.sival_int;
    printf("signal arg : %d\n",childPid);//*(int*)context);
    int status;
    if(waitpid(childPid,&status,WNOHANG) == 0)
    {
        if(!WIFCONTINUED(status))
        {
            kill(childPid,SIGCONT);
            printf("Killlin1\n");
        }
        else
        {
            printf(" not Killlin1\n");
        }
    }
}

int main(int argc, char* argv[])
{
    srand(time(NULL));

    int opt;

    while ((opt = getopt(argc, argv, "B:D:M:L:O:p:c:l:m:d:")) != -1)
    {
        switch (opt)
        {
        case 'B':
            maxNumberOfOdbiorniki = atoi(optarg);
            break;
        case 'D':
            averageTime = strtof(optarg,NULL);
            break;
        case 'M':
            deviation = strtof(optarg,NULL);
            break;
        case 'L':
            strcpy(diagnosticPath,optarg);
            break;
        case 'O':
            strcpy(outputPath,optarg);
            break;
        case 'p':
            strcpy(fifoNameTemplate,optarg);
            break;
        case 'c':
            maxNumberOfFifos = atoi(optarg);
            break;
        case 'l':
            averageOdbiorcaLifetime = getMinMaxValuesFromString(optarg);
            break;
        case 'm':
            averageOdbiorcaMParam = getMinMaxValuesFromString(optarg);
            break;
        case 'd':
            averageOdbiorcaDParam = getMinMaxValuesFromString(optarg);
            break;
        }
    }

    if(strlen(outputPath)!= 0)
    {
        outputFileDescriptor = open(outputPath,O_WRONLY | O_CREAT, 00666);
        printf("file descriptor %d\n", outputFileDescriptor);
    }

    registerHandler(SIGUSR1,childContinuationHandler);
    registerHandler(SIGCHLD,childSignalHandler);
    registerHandler(SIGALRM,createTimerHandler);
    createTimer(&createTimerId,SIGALRM);

    float time = randomizeTime(averageTime,deviation);
    convertFloatToTimeSpec(time,&timeUntilCreate.it_value);
    setTimer(createTimerId,&timeUntilCreate);

    while(1)
        pause();

    return 0;
}
