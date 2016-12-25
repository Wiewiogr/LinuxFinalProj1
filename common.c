#include "common.h"

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
