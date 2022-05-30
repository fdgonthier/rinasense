#ifndef _mock_IPCP_IPCP_H
#define _mock_IPCP_IPCP_H

/*
 * This is a partial public interface for the IPCP component.
 *
 * This is a TEMPORARY solution while the IPCP component is being
 * ported to Linux.
 */

#include "portability/port.h"
#include "ipcp_events.h"

#define SEND_STRUCT_WAIT_MAX 

/* PUBLIC API */

bool_t xIsCallingFromIPCPTask(void);

bool_t xSendEventStructToIPCPTask(const RINAStackEvent_t * pxEvent,
                                  struct timespec *uxTimeout);

/* Mock utilities */

RINAStackEvent_t *pxMockGetLastSentEvent();

void vMockClearLastSentEvent();

void vMockSetIsCallingFromIPCPTask(bool_t v);

bool_t xMockIPCPInit();
void vMockIPCPClean();

#endif // _mock_IPCP_IPCP_H
