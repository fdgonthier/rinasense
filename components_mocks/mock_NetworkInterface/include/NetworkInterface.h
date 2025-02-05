#ifndef _mock_NETWORKINTERFACE_H
#define _mock_NETWORKINTERFACE_H

/* PUBLIC API */

#include "common/mac.h"
#include "BufferManagement.h"

#ifdef ESP_PLATFORM

#define xNetworkInterfaceInitialise mock_NetworkInterface_xNetworkInterfaceInitialise
#define xNetworkInterfaceOutput     mock_NetworkInterface_xNetworkInterfaceOutput
#define xNetworkInterfaceConnect    mock_NetworkInterface_xNetworkInterfaceConnect

bool_t mock_NetworkInterface_xNetworkInterfaceInitialise(struct ipcpInstance_t *pxSelf,
                                                         const MACAddress_t *phyDev);

bool_t mock_NetworkInterface_xNetworkInterfaceOutput(NetworkBufferDescriptor_t *const pxNetworkBuffer,
                                                     bool_t xReleaseAfterSend);

bool_t mock_NetworkInterface_xNetworkInterfaceConnect(void);

#else

bool_t xNetworkInterfaceInitialise(struct ipcpInstance_t *pxSelf,
                                   const MACAddress_t *phyDev);

bool_t xNetworkInterfaceOutput(NetworkBufferDescriptor_t *const pxNetworkBuffer,
                               bool_t xReleaseAfterSend);

bool_t xNetworkInterfaceConnect(void);

#endif

#endif // _mock_NETWORKINTERFACE_H
