#include <glib.h>

void vSetMaxTimespec(struct timespec *ts)
{
    /* Kind of randomly set to 1h for now... */
    ts->tv_sec = 3600;
    ts->tv_nsec = 0;
}
