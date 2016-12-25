#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
extern void convertFloatToTimeSpec(float time, struct timespec * ts);
extern void createAndSetExitTimer(struct itimerspec* , clockid_t type);
