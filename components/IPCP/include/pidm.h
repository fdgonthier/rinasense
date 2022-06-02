/**
 *
 *        Port Id Manager:
 *       Managing the PortIds allocated in the whole stack. PortIds in IPCP Normal
 *       and Shim Wifi.
 *
 **/

#ifndef PIDM_H__INCLUDED
#define PIDM_H__INCLUDED

#include "portability/port.h"
#include "rina_ids.h"

typedef struct xPIDM {
    /* The last Allocated Port*/
	portId_t xLastAllocated;

    /* Big bit array of 2048 ports. */
    uint32_t ports[64];

} pidm_t;

typedef struct xALLOC_PID {
    /*List Item to register the Pid into the xAllocatedPorts List*/
    RsListItem_t xPortIdItem;

    /*Port Id allocated*/
    portId_t xPid;

} allocPid_t;


pidm_t * pxPidmCreate(void);

bool_t xPidmDestroy(pidm_t *pxInstance);

portId_t xPidmAllocate(pidm_t *pxInstance);

bool_t xPidmRelease(pidm_t *pxInstance, portId_t id);

bool_t xPidmAllocated(pidm_t *pxInstance, portId_t xPortId);

#endif
