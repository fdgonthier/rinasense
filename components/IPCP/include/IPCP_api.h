#ifndef _IPCP_API_H
#define _IPCP_API_H

/*
 * Send the event eEvent to the IPCP task event queue, using a block time of
 * zero.  Return pdPASS if the message was sent successfully, otherwise return
 * pdFALSE.
 */
bool_t xSendEventToIPCPTask(eRINAEvent_t eEvent);

/* Returns pdTRUE is this function is called from the IPCP-task */
bool_t xIsCallingFromIPCPTask( void );

bool_t xSendEventStructToIPCPTask( const RINAStackEvent_t * pxEvent,
                                         TickType_t uxTimeout );

eFrameProcessingResult_t eConsiderFrameForProcessing( const uint8_t * const pucEthernetBuffer );

bool_t RINA_IPCPInit( void );

#endif // _IPCP_API_H
