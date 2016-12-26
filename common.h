#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

extern float randomizeTime(float,float);

extern void convertFloatToTimeSpec(float time, struct timespec * ts);

extern void createAndSetExitTimer(struct itimerspec* , clockid_t type);

extern void createTimerAndRegisterHandler(timer_t *timerId, void(*handler)(int, siginfo_t*, void*));

extern void setTimer(timer_t,struct itimerspec*);

extern void showTimeDifferenceReport(struct timespec*);

