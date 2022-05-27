#include "portability/port.h"
#include "ipcp_events.h"

static RINAStackEvent_t *sentEvent = NULL;
static bool_t isCallingFromIPCPTask = false;

/* Mock IPCP public API */

bool_t xIsCallingFromIPCPTask(void) {
    return isCallingFromIPCPTask;
}

bool_t xSendEventStructToIPCPTask(const RINAStackEvent_t *pxEvent,
                                  struct timespec *uxTimeout)
{
    sentEvent = pxEvent;
    return true;
}

/* Utility mock functions */

RINAStackEvent_t *pxMockGetLastSentEvent()
{
    return sentEvent;
}

void vMockClearLastSentEvent()
{
    sentEvent = NULL;
}

void vMockSetIsCallingFromIPCPTask(bool_t v)
{
    isCallingFromIPCPTask = v;
}
