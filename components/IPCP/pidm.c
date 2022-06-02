/**
*
*        Port Id Manager:
*       Managing the PortIds allocated in the whole stack. PortIds in IPCP Normal
*       and Shim Wifi.
*
**/

#include "configRINA.h"
#include "configSensor.h"
#include "pidm.h"
#include "rina_ids.h"
#include "bit_array.h"

#include "portability/port.h"

#define BITS_PER_BYTE (8)
#define MAX_PORT_ID (((2 << BITS_PER_BYTE) * sizeof(portId_t)) - 1)


/** @brief pxPidmCreate.
 *
 * Create the instance of Port Id manager. This instance is register in the
 * IpcManager. Initialize the list of allocated ports.
 */
pidm_t *pxPidmCreate(void)
{
        pidm_t *pxPidmInstance;

        pxPidmInstance = pvRsMemAlloc(sizeof(*pxPidmInstance));
        if (!pxPidmInstance)
                return NULL;

        //Initializing the list of Allocated Ports
        //memset(&pxPidmInstance->ports, 0, sizeof(pxPidmInstance->ports));
        pxPidmInstance->pBitArray = bitarray_alloc(MAX_PORT_ID);
        pxPidmInstance->xLastAllocated = 0;

        return pxPidmInstance;
}

/** @brief pxPidmDestroy
 *
 * Destroy the instance of Port Id manager. This instance free memory.
 */
bool_t xPidmDestroy(pidm_t *pxInstance)
{
        allocPid_t *pxPos, *pxNext;

        if (!pxInstance)
        {
                LOGE(TAG_IPCPMANAGER, "Bogus instance passed, bailing out");
                return false;
        }

        bitarray_free(pxInstance->pBitArray);
        vRsMemFree(pxInstance);

        return true;
}

/** @brief xPidmAllocated.
 *
 * Check if a PortId was allocated or assigned */
bool_t xPidmAllocated(pidm_t *pxInstance, portId_t xPortId)
{
        return bitarray_get_bit(pxInstance->pBitArray, xPortId);
}

/** @brief xPidmAllocate
 *
 * Allocate or assign a new port Id. Insert the port Id into the list of port Id
 * allocated Return the portId
 */
portId_t xPidmAllocate(pidm_t *pxInstance)
{
        portId_t p;

        /* Check if the PxInstances parameter is not null*/
        if (!pxInstance)
        {
                LOGE(TAG_IPCPMANAGER, "Bogus instance passed, bailing out");
                return port_id_bad();
        }

        /* Unlike the previous implementation, this leaves to the caller to
         * decide if it wants to retry port allocation. The previous
         * implementation would just lock up until a port was freed. */

        p = pxInstance->xLastAllocated + 1;

        /* Loops from the next to last allocated port, until we reach the last
         * allocated port again, wrapping over MAX_PORT_ID. */
        for (;; p++) {
                if (p == pxInstance->xLastAllocated)
                        return port_id_bad();

                if (p == MAX_PORT_ID)
                        p = 1;

                if (!bitarray_get_bit(pxInstance->pBitArray, p)) {
                        bitarray_set_bit(pxInstance->pBitArray, p);
                        pxInstance->xLastAllocated = p;
                        break;
                }
        }

        LOGI(TAG_IPCPMANAGER, "Port-id allocation completed successfully (id = %d)", p);

        return p;
}

/** @brief xPidmRelease
 *
 * Release a port Id.
 */
bool_t xPidmRelease(pidm_t *pxInstance, portId_t xPortId)
{
        if (!is_port_id_ok(xPortId))
        {
                LOGE(TAG_IPCPMANAGER, "Bad port ID passed, bailing out");
                return false;
        }
        if (!pxInstance)
        {
                LOGE(TAG_IPCPMANAGER, "Bogus instance passed, bailing out");
                return false;
        }

        if (!bitarray_get_bit(pxInstance->pBitArray, xPortId)) {
                LOGE(TAG_IPCPMANAGER, "Didn't find port ID %d, returning error", xPortId);
                return false;
        }
        else {
                bitarray_clear_bit(pxInstance->pBitArray, xPortId);
                LOGI(TAG_IPCPMANAGER, "Port ID release completed successfully (port ID: %d)", xPortId);
                return true;
        }

}
