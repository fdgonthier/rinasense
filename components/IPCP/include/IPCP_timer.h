#ifndef _IPCP_TIMER_H
#define _IPCP_TIMER_H

/**
 * The software timer struct for various IPCP functions
 */
    typedef struct xIPCP_TIMER
    {
        uint32_t
            bActive : 1,            /**< This timer is running and must be processed. */
            bExpired : 1;           /**< Timer has expired and a task must be processed. */
        TimeOut_t xTimeOut;         /**< The timeout value. */
        TickType_t ulRemainingTime; /**< The amount of time remaining. */
        TickType_t ulReloadTime;    /**< The value of reload time. */
    } IPCPTimer_t;

#endif // _IPCP_TIMER_H
