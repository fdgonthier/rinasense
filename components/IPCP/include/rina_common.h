/*
 * common.h
 *
 *  Created on: 21 oct. 2021
 *      Author: i2CAT
 */

#ifndef COMPONENTS_IPCP_INCLUDE_COMMON_H_
#define COMPONENTS_IPCP_INCLUDE_COMMON_H_

#include "configSensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "rina_ids.h"
#include "rina_common_port.h"

#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({         \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) ); })

struct flowSpec_t
{
        /* This structure defines the characteristics of a flow */

        /* Average bandwidth in bytes/s */
        uint32_t ulAverageBandwidth;

        /* Average bandwidth in SDUs/s */
        uint32_t ulAverageSduBandwidth;

        /*
         * In milliseconds, indicates the maximum delay allowed in this
         * flow. A value of 0 indicates 'do not care'
         */
        uint32_t ulDelay;
        /*
         * In milliseconds, indicates the maximum jitter allowed
         * in this flow. A value of 0 indicates 'do not care'
         */
        uint32_t ulJitter;

        /*
         * Indicates the maximum packet loss (loss/10000) allowed in this
         * flow. A value of loss >=10000 indicates 'do not care'
         */
        uint16_t usLoss;

        /*
         * Indicates the maximum gap allowed among SDUs, a gap of N
         * SDUs is considered the same as all SDUs delivered.
         * A value of -1 indicates 'Any'
         */
        int32_t ulMaxAllowableGap;

        /*
         * The maximum SDU size for the flow. May influence the choice
         * of the DIF where the flow will be created.
         */
        uint32_t ulMaxSduSize;

        /* Indicates if SDUs have to be delivered in order */
        BaseType_t xOrderedDelivery;

        /* Indicates if partial delivery of SDUs is allowed or not */
        BaseType_t xPartialDelivery;

        /* In milliseconds */
        uint32_t ulPeakBandwidthDuration;

        /* In milliseconds */
        uint32_t ulPeakSduBandwidthDuration;

        /* A value of 0 indicates 'do not care' */
        uint32_t ulUndetectedBitErrorRate;

        /* Preserve message boundaries */
        BaseType_t xMsgBoundaries;
};

typedef struct xCONNECTION_ID
{
        qosId_t xQosId;       /**< QoS Id  3 + 1 = 4 */
        cepId_t xDestination; /**< Cep Id Dest 4 + 1 = 5 */
        cepId_t xSource;      /**< Cep Id Source  5 + 1 = 6 */
} connectionId_t;

typedef struct xRMT_CONFIG
{
        /* The PS name for the RMT */
        policy_t *pxPolicySet;

        /* The configuration of the PDU Forwarding Function subcomponent */
        // struct pff_config * pff_conf;
} rmtConfig_t;

/* Represents a DIF configuration (policies, parameters, etc) */
typedef struct xDIF_CONFIG
{
        /* List of configuration entries */
        List_t xIpcpConfigEntries;

        /* the config of the efcp */
        efcpConfig_t *pxEfcpConfig;

        /* the config of the rmt */
        rmtConfig_t *pxRmtConfig;

        /* The address of the IPC Process*/
        address_t xAddress;
        /*
        struct fa_config * fa_config;
        struct et_config * et_config;
        struct nsm_config * nsm_config;
        struct routing_config * routing_config;
        struct resall_config * resall_config;
        struct secman_config * secman_config;*/
} difConfig_t;

/* Endian related definitions. */
//#if ( ipconfigBYTE_ORDER == pdFREERTOS_LITTLE_ENDIAN )

/* FreeRTOS_htons / FreeRTOS_htonl: some platforms might have built-in versions
 * using a single instruction so allow these versions to be overridden. */
//#ifndef FreeRTOS_htons
#define FreeRTOS_htons(usIn) ((uint16_t)(((usIn) << 8U) | ((usIn) >> 8U)))
//#endif

#ifndef FreeRTOS_htonl
#define FreeRTOS_htonl(ulIn)                                        \
        (                                                           \
            (uint32_t)(((((uint32_t)(ulIn))) << 24) |               \
                       ((((uint32_t)(ulIn)) & 0x0000ff00UL) << 8) | \
                       ((((uint32_t)(ulIn)) & 0x00ff0000UL) >> 8) | \
                       ((((uint32_t)(ulIn))) >> 24)))
#endif /* ifndef FreeRTOS_htonl */

//#else /* ipconfigBYTE_ORDER */

//#define FreeRTOS_htons( x )    ( ( uint16_t ) ( x ) )
//#define FreeRTOS_htonl( x )    ( ( uint32_t ) ( x ) )

//#endif /* ipconfigBYTE_ORDER == pdFREERTOS_LITTLE_ENDIAN */

#define FreeRTOS_ntohs(x) FreeRTOS_htons(x)
#define FreeRTOS_ntohl(x) FreeRTOS_htonl(x)
#define SOCKET_EVENT_BIT_COUNT 8

enum eFLOW_EVENT
{
        eFLOW_RECEIVE = 0x0001,
        eFLOW_SEND = 0x0002,
        eFLOW_ACCEPT = 0x0004,
        eFLOW_CONNECT = 0x0008,
        eFLOW_BOUND = 0x0010,
        eFLOW_CLOSED = 0x0020,
        eSELECT_ALL = 0x000F,

};

typedef struct xAUTH_POLICY
{
        string_t pcName;
        string_t pcVersion;
        uint8_t ucAbsSyntax;

} authPolicy_t;

// name_t *xRinaNameCreate(void);

// BaseType_t xRinaNameFromString(const string_t pcString, name_t *xName);
// void xRinaNameFree(name_t *xName);

// BaseType_t xRINAStringDup(const string_t *pcSrc, string_t **pcDst);

// name_t *xRINAstringToName(codownnst string_t *pxInput);

void memcheck(void);

int get_next_invoke_id(void);

#endif /* COMPONENTS_IPCP_INCLUDE_COMMON_H_ */
