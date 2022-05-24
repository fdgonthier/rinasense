#ifndef IPCP_BUFFER_H
#define IPCP_BUFFER_H

#include "portability/port.h"

typedef enum FRAMES_PROCESSING
{
	eReleaseBuffer = 0,   /* Processing the frame did not find anything to do - just release the buffer. */
	eProcessBuffer,       /* An Ethernet frame has a valid address - continue process its contents. */
	eReturnEthernetFrame, /* The Ethernet frame contains an ARP826 packet that can be returned to its source. */
	eFrameConsumed        /* Processing the Ethernet packet contents resulted in the payload being sent to the stack. */
} eFrameProcessingResult_t;

typedef struct xNETWORK_BUFFER
{
    RsListItem_t xBufferListItem;              /**< Used to reference the buffer form the free buffer list or a socket. */
    uint8_t ulGpa;                             /**< Source or destination Protocol address, depending on usage scenario. */
    uint8_t * pucEthernetBuffer;               /**< Pointer to the start of the Ethernet frame. */
    size_t xDataLength;                        /**< Starts by holding the total Ethernet frame length, then the UDP/TCP payload length. */
    uint32_t ulPort;                           /**< Source or destination port, depending on usage scenario. */
    uint32_t ulBoundPort;                      /**< The N-1 port to transmite. */

} NetworkBufferDescriptor_t;

#endif // IPCP_BUFFER_H
