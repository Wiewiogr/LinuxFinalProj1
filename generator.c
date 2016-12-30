#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
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
            switch(rand() % 4)
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
            sprintf(fifoArg,"%s%d",fifoNameTemplate,(rand() % maxNumberOfFifos+1));
            printf("created - %s %s %s %s %s\n", odbiornikName, averageValueArg, deviationArg, lifeTimeArg, fifoArg);
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

            execve(odbiornikName,odbiornikArgs,newenviron);
            exit(1);
        }
        numberOfOdbiorniki++;
        printf("number of receivers alive : %d/%d\n",numberOfOdbiorniki,maxNumberOfOdbiorniki);
    }
    fflush(stdout);
}

void childSignalHandler(int sig, siginfo_t *info, void *context)
{
    int code = info->si_code;
    if(code == CLD_KILLED || code == CLD_EXITED)
    {
        printf("receiver with pid %d died\n",info->si_pid);
        numberOfOdbiorniki--;
    }
    else if(code == CLD_STOPPED)
    {
        printf("receiver with pid %d stopped\n",info->si_pid);
        timer_t continueTimer;

        createTimerWithArgument(&continueTimer,SIGUSR1,info->si_pid);
        float time = randomizeTime(averageTime,deviation);
        convertFloatToTimeSpec(time,&timeUntilCreate.it_value);
        setTimer(continueTimer,&timeUntilCreate);

    }
    fflush(stdout);
}

void childContinuationHandler(int sig, siginfo_t *info, void *context)
{
    int childPid = info->si_value.sival_int;
    int status;
    if(waitpid(childPid,&status,WNOHANG) == 0)
    {
        if(!WIFCONTINUED(status))
        {
            kill(childPid,SIGCONT);
            printf("receiver with pid %d continued\n",childPid);
        }
    }
    fflush(stdout);
}

int main(int argc, char* argv[])
{
    srand(time(NULL));

    {
        int opt;
        bool Bset=false, Dset=false, Oset=false, lset=false,
             mset=false, dset=false, pset=false, cset=false;
        while ((opt = getopt(argc, argv, "B:D:M:L:O:p:c:l:m:d:")) != -1)
        {
            switch (opt)
            {
            case 'B':
                maxNumberOfOdbiorniki = atoi(optarg);
                Bset = true;
                break;
            case 'D':
                averageTime = strtof(optarg,NULL);
                Dset = true;
                break;
            case 'M':
                deviation = strtof(optarg,NULL);
                break;
            case 'L':
                strcpy(diagnosticPath,optarg);
                break;
            case 'O':
                strcpy(outputPath,optarg);
                Oset = true;
                break;
            case 'p':
                strcpy(fifoNameTemplate,optarg);
                pset = true;
                break;
            case 'c':
                maxNumberOfFifos = atoi(optarg);
                cset = true;
                break;
            case 'l':
                averageOdbiorcaLifetime = getMinMaxValuesFromString(optarg);
                lset = true;
                break;
            case 'm':
                averageOdbiorcaMParam = getMinMaxValuesFromString(optarg);
                mset = true;
                break;
            case 'd':
                averageOdbiorcaDParam = getMinMaxValuesFromString(optarg);
                dset = true;
                break;
            }
        }

        if(!(Bset && Dset && Oset && lset && mset && dset && pset && cset))
        {
            fprintf(stderr,"usage : %s \n-B <int> -D <float> [-M <float>] [-L <string>] -O <string> \n-p <string> -c <int> \n-l <float>[:<float>] -m <float>[:<float>] -d <float>[:<float>]\n",argv[0]);
            exit(1);
        }
    }

    if(strlen(outputPath)!= 0)
    {
        outputFileDescriptor = open(outputPath,O_WRONLY | O_CREAT, 00666);
    }

    if(strlen(diagnosticPath)!= 0)
    {
        int descriptor = open(diagnosticPath,O_WRONLY | O_CREAT, 00666);
        dup2(descriptor,1);
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
