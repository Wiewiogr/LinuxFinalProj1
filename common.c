#include "common.h"

float randomizeTime(float averageTime ,float deviation)
{
    return averageTime + (1.0*rand()/RAND_MAX)*deviation*2-deviation;
}

void convertFloatToTimeSpec(float time, struct timespec * ts)
{
    ts->tv_sec = floor(time);
    ts->tv_nsec = (time - floor(time))*1000000000;
}

void createAndSetExitTimer(struct itimerspec *timeUntilExit, clockid_t type)
{
    if(type != -1)
    {
        timer_t endTimerId;
        struct sigevent sevProgramEnd;
        sevProgramEnd.sigev_notify = SIGEV_SIGNAL;
        sevProgramEnd.sigev_signo = SIGKILL;
        sevProgramEnd.sigev_value.sival_ptr = &endTimerId;
        if (timer_create(type, &sevProgramEnd, &endTimerId) == -1)
            perror("timer_create");

        if (timer_settime(endTimerId, 0, timeUntilExit, NULL) == -1)
            perror("timer_settime");
    }

}

void registerHandler(int signalNumber, void(*handler)(int, siginfo_t*, void*))
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(signalNumber, &sa, NULL) == -1)
        perror("sigaction");
}

void createTimer(timer_t *timerId, int signalNumber)
{
    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = signalNumber;
    sev.sigev_value.sival_ptr = timerId;
    if (timer_create(CLOCK_REALTIME, &sev, timerId) == -1)
        perror("timer_create");
}

void createTimerWithArgument(timer_t *timerId, int signalNumber,int arg)
{
    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = signalNumber;
    sev.sigev_value.sival_ptr = timerId;
    sev.sigev_value.sival_int = arg;
    if (timer_create(CLOCK_REALTIME, &sev, timerId) == -1)
        perror("timer_create");
}

void createTimerAndRegisterHandler(timer_t *timerId, void(*handler)(int, siginfo_t*, void*))
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        perror("sigaction");

    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGALRM;
    sev.sigev_value.sival_ptr = timerId;
    if (timer_create(CLOCK_REALTIME, &sev, timerId) == -1)
        perror("timer_create");
}

void setTimer(timer_t timerId,struct itimerspec *timeSpec)
{
    if (timer_settime(timerId, 0, timeSpec, NULL) == -1)
        perror("timer_settime");
}

void showTimeDifferenceReport(struct timespec* time, char * odbiornikName, char* fifoPath)
{
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME,&currentTime);
    long sec = currentTime.tv_sec - time->tv_sec;
    long nsec = currentTime.tv_nsec - time->tv_nsec;
    if(nsec < 0)
    {
        nsec = 1000000000 - nsec;
        sec--;
    }
    printf("!!REPORT from %s with pid %d reading from %s!!!\n", odbiornikName, getpid(), fifoPath);
    printf("time received : %ld.%ld ",time->tv_sec, time->tv_nsec);
    printf("current time %ld.%ld \n",currentTime.tv_sec, currentTime.tv_nsec);
    printf("time difference %ld.%ld\n",sec,nsec);
}

bool isPollError(short revents)
{
    return (revents & POLLERR) || (revents & POLLNVAL) || (revents & POLLHUP);
}

bool checkAndPrintPollErrors(short revents)
{
    bool res = false;
    if(revents & POLLERR)
    {
        printf("POLLERR\n");
        res = true;
    }
    else if(revents & POLLNVAL)
    {
        printf("POLLNVAL\n");
        res = true;
    }
    else if(revents & POLLHUP)
    {
        printf("POLLHUP\n");
        res = true;
    }
    return res;
}

struct pollfd createPollfdStruct(int fd)
{
    struct pollfd fds;
    fds.fd = fd;
    fds.events = POLLIN;
    fds.revents = 0;
    return fds;
}


bool isFifo(char* path)
{
    struct stat sb;
    if (stat(path, &sb) == -1)
    {
    return false;
    }
    if(!S_ISFIFO(sb.st_mode))
        return false;
    return true;
}

struct minMaxValues getMinMaxValuesFromString(char* str)
{
    struct minMaxValues minMax;
    char* secondValue;
    minMax.min = strtof(str,&secondValue);

    if( strlen(secondValue) == 0)
    {
        minMax.max = minMax.min;
    }
    else
    {
        strcpy(secondValue,secondValue+1);
        minMax.max = strtof(secondValue,NULL);
    }

    return minMax;
}

float getValueFromMinMax(struct minMaxValues* minMax)
{
    float difference = minMax->max - minMax->min;
    return minMax->min + difference*(1.0*rand()/RAND_MAX);
}


void createBackupFile(struct Fifo* fifo)
{
    link(fifo->path,fifo->backupPath);
}

void createBackupFiles(struct Fifo* fifos,int amount)
{
    mkdir(".secret", 00777);
    for(int i = 0 ; i < amount; i++)
    {
        createBackupFile(&fifos[i]);
    }
}



