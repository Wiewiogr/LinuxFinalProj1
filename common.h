#pragma once
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <string.h>

struct Fifo
{
    char path[50];
    char backupPath[50];
    int fileDescriptor;
    bool isOpened;
    bool isFull;
};

struct minMaxValues
{
    float min;
    float max;
};

extern float randomizeTime(float,float);

extern void convertFloatToTimeSpec(float time, struct timespec * ts);

extern void createAndSetExitTimer(struct itimerspec* , clockid_t type);

extern void createTimerAndRegisterHandler(timer_t *timerId, void(*handler)(int, siginfo_t*, void*));

extern void setTimer(timer_t,struct itimerspec*);

extern void showTimeDifferenceReport(struct timespec*);

extern bool checkAndPrintPollErrors(short revents);

extern struct pollfd createPollfdStruct(int fd);

extern bool isFifo(char* path);

extern struct minMaxValues getMinMaxValuesFromString(char* str);

extern float getValueFromMinMax(struct minMaxValues*);

extern void createTimer(timer_t *timerId, int signalNumber);

extern void registerHandler(int signalNumber, void(*handler)(int, siginfo_t*, void*));

extern void createTimerWithArgument(timer_t *timerId, int signalNumber,int arg);

extern void createBackupFile(struct Fifo* fifo);

extern void createBackupFiles(struct Fifo* fifos,int amount);

extern bool isPollError(short revents);
