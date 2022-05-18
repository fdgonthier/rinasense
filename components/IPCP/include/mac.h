#ifndef _IPCP_MAC_H
#define _IPCP_MAC_H

#include "configSensor.h"

//Structure MAC ADDRESS
typedef struct xMAC_ADDRESS
{
	uint8_t ucBytes[ MAC_ADDRESS_LENGTH_BYTES ]; /**< Byte array of the MAC address */
}MACAddress_t;

#endif // _IPCP_MAC_H
