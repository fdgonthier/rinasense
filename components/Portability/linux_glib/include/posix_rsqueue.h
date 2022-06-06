#ifndef _PORT_GLIB_RSQUEUE_H
#define _PORT_GLIB_RSQUEUE_H

#include <glib.h>
#include "portability/posix/mqueue.h"

typedef struct xRSQUEUE_T {
    mqd_t xQueue;

    size_t uxQueueLength;
    size_t uxItemSize;

    char *sQueueName;
} RsQueue_t;

#endif // _PORT_GLIB_RSQUEUE_H
