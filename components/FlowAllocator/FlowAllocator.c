#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common/rina_ids.h"
#include "portability/port.h"

#include "configSensor.h"
#include "configRINA.h"

#include "efcpStructures.h"
#include "Enrollment.h"
#include "Enrollment_api.h"
#include "EnrollmentInformationMessage.pb.h"
#include "IPCP.h"
#include "IPCP_normal_defs.h"
#include "IPCP_normal_api.h"
#include "FlowAllocator.h"
#include "FlowAllocator_api.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "Rib.h"
#include "Ribd.h"
#include "Ribd_api.h"
#include "rina_common_port.h"
#include "RINA_API_flows.h"
#include "SerdesMsg.h"
#include "IPCP_api.h"

/* New API */

bool_t xFlowAllocatorInit(flowAllocator_t *pxFA)
{
    return false;
}

void vFlowAllocatorFini(flowAllocator_t *pxFA)
{
}

/* OLD UNREVIEWED API */

void prvAddFlowRequestEntry(flowAllocator_t *pxFA, flowAllocatorInstance_t *pxFAI)
{
    num_t x = 0;

    for (x = 0; x < FLOWS_REQUEST; x++)
    {
        if (pxFA->xFlowRequestTable[x].xValid == false)
        {
            pxFA->xFlowRequestTable[x].pxFAI = pxFAI;
            pxFA->xFlowRequestTable[x].xValid = true;

            break;
        }
    }
}

flowAllocatorInstance_t *pxFAFindInstance(flowAllocator_t *pxFA, portId_t xPortId)
{
    num_t x = 0;
    flowAllocatorInstance_t *pxFAI;

    for (x = 0; x < FLOWS_REQUEST; x++)
    {
        if (pxFA->xFlowRequestTable[x].xValid == true)
        {
            pxFAI = pxFA->xFlowRequestTable->pxFAI;
            if (pxFAI->xPortId == xPortId) // ad xPortId
            {
                return pxFAI;
            }
        }
    }
    return NULL;
}

flowAllocateHandle_t *pxFAFindFlowHandle(flowAllocator_t *pxFA, portId_t xPortId)
{
    num_t x = 0;
    flowAllocatorInstance_t *pxFAI;
    flowAllocateHandle_t *pxFlowAllocateRequest;

    for (x = 0; x < FLOWS_REQUEST; x++)
    {
        if (pxFA->xFlowRequestTable[x].xValid == true)
        {
            pxFAI = pxFA->xFlowRequestTable->pxFAI;
            if (pxFAI->xPortId == xPortId) // ad xPortId
            {

                return pxFAI->pxFlowAllocatorHandle;
            }
        }
    }
    return NULL;
}

flowAllocateHandle_t *prvGetPendingFlowRequest(flowAllocator_t *pxFA, portId_t xPortId)
{
    num_t x = 0;
    flowAllocateHandle_t *pxFlowAllocateRequest;
    flowAllocatorInstance_t *pxFAI;

    for (x = 0; x < FLOWS_REQUEST; x++)
    {
        if (pxFA->xFlowRequestTable[x].xValid == true)
        {
            pxFAI = pxFA->xFlowRequestTable->pxFAI;
            if (pxFAI->xPortId == xPortId) // ad xPortId
            {
                pxFlowAllocateRequest = pxFAI->pxFlowAllocatorHandle;
                return pxFlowAllocateRequest;
            }
        }
    }
    return NULL;
}

flowAllocator_t *pxFlowAllocatorCreate(struct ipcpInstance_t *pxNormalIpcp)
{
    flowAllocator_t *pxFlowAllocator;

    if (!(pxFlowAllocator = pvRsMemAlloc(sizeof(*pxFlowAllocator)))) {
        LOGE(TAG_FA, "Failed to allocated memory for flow allocator object");
        return NULL;
    }

    /* Save the normal IPCP instance we're attached to. */
    pxFlowAllocator->pxNormalIpcp = pxNormalIpcp;

    /* Create object in the Rib*/
    /* FIXME: TBD */

    /*Init List*/
    vRsListInit(&pxFlowAllocator->xFlowAllocatorInstances);

    return pxFlowAllocator;
}

static qosSpec_t *prvFlowAllocatorSelectQoSCube(void)
{
    qosSpec_t *pxQosSpec;
    struct flowSpec_t *pxFlowSpec;

    pxQosSpec = pvRsMemAlloc(sizeof(*pxQosSpec));
    pxFlowSpec = pvRsMemAlloc(sizeof(*pxFlowSpec));

    /* TODO: Found the most suitable QoScube for the pxFlowRequest->pxFspec
     * Now, it use a default cube to test */
    pxQosSpec->xQosId = QoS_CUBE_ID;
    pxQosSpec->pcQosName = QoS_CUBE_NAME;

    pxFlowSpec->xPartialDelivery = QoS_CUBE_PARTIAL_DELIVERY;
    pxFlowSpec->xOrderedDelivery = QoS_CUBE_ORDERED_DELIVERY;

    pxQosSpec->pxFlowSpec = pxFlowSpec;

    return pxQosSpec;
}

/**/
static flow_t *prvFlowAllocatorNewFlow(flowAllocateHandle_t *pxFlowRequest)
{
    flow_t *pxFlow;

    dtpConfig_t *pxDtpConfig;
    policy_t *pxDtpPolicySet;
    /* UNUSED struct dtcpConfig_t *pxDtcpConfig = NULL; */

    pxFlow = pvRsMemAlloc(sizeof(*pxFlow));

    pxDtpConfig = pvRsMemAlloc((sizeof(*pxDtpConfig)));
    pxDtpPolicySet = pvRsMemAlloc(sizeof(*pxDtpPolicySet));

    // ESP_LOGI(TAG_FA, "DEst:%s", strdup(pxFlowRequest->pxRemote));

    pxFlow->pxSourceInfo = pxFlowRequest->pxLocal;
    pxFlow->pxDestInfo = pxFlowRequest->pxRemote;
    pxFlow->ulHopCount = 4;
    pxFlow->ulMaxCreateFlowRetries = 1;
    pxFlow->eState = eFA_ALLOCATION_IN_PROGRESS;
    pxFlow->xSourceAddress = LOCAL_ADDRESS;
    pxFlow->ulCurrentConnectionId = 0;

    /* Select QoS Cube based on the FlowSpec Required */
    pxFlow->pxQosSpec = prvFlowAllocatorSelectQoSCube();

    /* Fulfill the DTP_config and the DTCP_config based on the QoSCube*/

    pxDtpConfig->xDtcpPresent = DTP_DTCP_PRESENT;
    pxDtpConfig->xInitialATimer = DTP_INITIAL_A_TIMER;

    pxDtpPolicySet->pcPolicyName = DTP_POLICY_SET_NAME;
    pxDtpPolicySet->pcPolicyVersion = DTP_POLICY_SET_VERSION;
    pxDtpConfig->pxDtpPolicySet = pxDtpPolicySet;

    pxFlow->pxDtpConfig = pxDtpConfig;

    // By the moment the DTCP is not implemented yet so we are using DTP_DTCP_PRESENT = pdFALSE

    return pxFlow;
}

/*Allocate_Request: Handle the request send by the application
 * if it is well-formed, create a new FlowAllocator-Instance*/

void vFlowAllocatorFlowRequest(flowAllocator_t *pxFA,
                               portId_t xAppPortId,
                               flowAllocateHandle_t *pxFlowRequest)
{
    flow_t *pxFlow;
    neighborInfo_t *pxNeighbor;
    cepId_t xCepSourceId;
    flowAllocatorInstance_t *pxFlowAllocatorInstance;
    connectionId_t *pxConnectionId;
    string_t pcNeighbor;
    struct efcpContainer_t *pxEfcpc;
    struct ipcpInstanceData_t *pxIpcpData;

    pxIpcpData = IPCP_DATA_FROM_FLOW_ALLOCATOR(pxFA);
    pxEfcpc = &pxIpcpData->xEfcpContainer;

    /* Create a flow object and fill using the event FlowRequest */
    pxFlow = prvFlowAllocatorNewFlow(pxFlowRequest);

    if (!(pxConnectionId = pvRsMemAlloc(sizeof(connectionId_t)))) {
        LOGE(TAG_FA, "Failed to allocate memory for connection ID");
        goto fail;
    }

    /* Create a FAI and fill the struct properly*/
    if (!(pxFlowAllocatorInstance = pvRsMemAlloc(sizeof(*pxFlowAllocatorInstance)))) {
        LOGE(TAG_FA, "Failed to allocate memory for flow allocator instance");
        goto fail;
    }

    pxFlowAllocatorInstance->eFaiState = eFAI_NONE;
    pxFlowAllocatorInstance->xPortId = xAppPortId;
    pxFlowAllocatorInstance->pxFlowAllocatorHandle = pxFlowRequest;

    prvAddFlowRequestEntry(pxFA, pxFlowAllocatorInstance);
    LOGD(TAG_FA, "FAI added properly");

    // Query NameManager to getting neighbor how knows the destination requested
    // pcNeighbor = xNmsGetNextHop(pxFlow->pxDesInfo->pcProcessName;

    /* FIXME: Harcoded REMOTE_ADDRESS_AP_NAME in
       vFlowAllocatorFlowRequest */
    pcNeighbor = REMOTE_ADDRESS_AP_NAME; // "ar1.mobile"; // Hardcode for testing
    LOGD(TAG_FA, "Getting Neighbor");

    /* Request to DFT the Next Hop, at the moment request to EnrollmmentTask */
    pxNeighbor = pxEnrollmentFindNeighbor(pcNeighbor);
    if (!pxNeighbor) {
        LOGE(TAG_FA, "No neighbor found");
        return;
    }

    pxFlow->xRemoteAddress = pxNeighbor->xNeighborAddress;
    pxFlow->xSourcePortId = xAppPortId;

    if (pxFlow->xRemoteAddress == 0)
        LOGE(TAG_FA, "Failed to get next hop");

    /* Call EFCP to create an EFCP instance following the EFCP Config */
    LOGD(TAG_FA, "Creating a Connection");

    if ((xCepSourceId = xNormalConnectionCreateRequest(pxIpcpData->pxIpcp,
                                                       pxEfcpc,
                                                       xAppPortId,
                                                       LOCAL_ADDRESS,
                                                       pxFlow->xRemoteAddress,
                                                       pxFlow->pxQosSpec->xQosId,
                                                       pxFlow->pxDtpConfig,
                                                       pxFlow->pxDtcpConfig)) == CEP_ID_WRONG) {
        LOGE(TAG_FA, "Failed to create connection request");
    }

    /*------ Add CepSourceID into the Flow------*/
    if (!xNormalUpdateCepIdFlow(pxIpcpData->pxIpcp, xAppPortId, xCepSourceId))
    {
        LOGE(TAG_FA, "CepId not updated into the flow");
    }

    LOGD(TAG_FA, "CepId updated into the flow");
    /* Fill the Flow connectionId */
    pxConnectionId->xSource = xCepSourceId;
    pxConnectionId->xQosId = pxFlow->pxQosSpec->xQosId;
    pxConnectionId->xDestination = 0;

    pxFlow->pxConnectionId = pxConnectionId;

    /* Send the flow message to the neighbor */
    // Serialize the pxFLow Struct into FlowMsg and Encode the FlowMsg as obj_value
    serObjectValue_t *pxObjVal = NULL;

    pxObjVal = pxSerdesMsgFlowEncode(pxFlow);

    char flowObj[CHAR_MAX];
    sprintf(flowObj, "/fa/flows/key=%d-%d", pxFlow->xSourceAddress, pxFlow->xSourcePortId);

    if (!pxRibCreateObject(flowObj, -1, "Flow", "Flow", FLOW))
    {
        LOGE(TAG_FA, "It was a problem to create Rib Object");
    }

    if (!xRibdSendRequest("Flow", flowObj, -1, M_CREATE, pxNeighbor->xN1Port, pxObjVal))
    {
        LOGE(TAG_FA, "It was a problem to send the request");
        // return pdFALSE;
    }

    if (pxObjVal != NULL)
        vRsMemFree(pxObjVal);

    fail:
}

bool_t xFlowAllocatorHandleCreateR(flowAllocator_t *pxFA, serObjectValue_t *pxSerObjValue, int result)
{
    portId_t xAppPortId;
    flowAllocatorInstance_t *pxFAI;
    flow_t *pxFlow;
    bool_t xStatus;

    RsAssert(pxFA);
    RsAssert(pxSerObjValue);

    if (result != 0) {
        LOGE(TAG_FA, "Remote flow creation failed");
        return false;
    }

    if (!(pxFlow = pxSerdesMsgDecodeFlow(pxSerObjValue->pvSerBuffer, pxSerObjValue->xSerLength))) {
        LOGE(TAG_FA, "Failed to deserialize flow creation message");
        return false;
    }

    if (!(pxFAI = pxFAFindInstance(pxFA, pxFlow->xDestinationPortId))) {
        LOGE(TAG_FA, "Flow Allocator Instance was not founded ");
        return false;
    }

    CALL_IPCP_CHECK(xStatus, pxFA->pxNormalIpcp, connectionModify,
                    pxFlow->pxConnectionId->xDestination,
                    pxFlow->xRemoteAddress,
                    pxFlow->xSourceAddress) {
        LOGE(TAG_FA, "Failed to modify the connection");
        return false;
    }

    CALL_IPCP_CHECK(xStatus, pxFA->pxNormalIpcp, connectionUpdate,
                    pxFlow->xDestinationPortId,
                    pxFlow->pxConnectionId->xSource,
                    pxFlow->pxConnectionId->xDestination) {
        LOGE(TAG_FA, "It was not possible to update the connection");
        return false;
    }

    pxFAI->eFaiState = eFAI_ALLOCATED;
    LOGI(TAG_FA, "Flow state updated to Allocated");

    pthread_mutex_lock(&pxFAI->pxFlowAllocatorHandle->xEventMutex);
    {
        pxFAI->pxFlowAllocatorHandle->nEventBits |= eFLOW_BOUND;
        pthread_cond_signal(&pxFAI->pxFlowAllocatorHandle->xEventCond);
    }
    pthread_mutex_unlock(&pxFAI->pxFlowAllocatorHandle->xEventMutex);

    return true;
}

bool_t xFlowAllocatorHandleDeleteR(flowAllocator_t *pxFA, struct ribObject_t *pxRibObject, int invoke_id)
{
    LOGE(TAG_FA, "HANDLE DELETE");

    // Delete connection
    // delete EFCP instance
    // change portId to NO ALLOCATED,
    // Send message to User with close,

    return false;
}

bool_t xFlowAllocatorHandleDelete(struct ribObject_t *pxRibObject, int invoke_id)
{
    LOGD(TAG_FA, "Calling: %s", __func__);

    // Delete connection
    // delete EFCP instance
    // change portId to NO ALLOCATED,
    // Send message to User with close,

    return false;
}

void vFlowAllocatorDeallocate(portId_t xAppPortId)
{

    if (!xAppPortId)
    {
        LOGE(TAG_FA, "Bogus data passed, bailing out");
    }

    // Find Flow and move to deallocate status.
}

bool_t xFlowAllocatorDuPost(flowAllocator_t *pxFA, portId_t xAppPortId, struct du_t *pxDu)
{
    flowAllocatorInstance_t *pxFlowAllocatorInstance;
    NetworkBufferDescriptor_t *pxNetworkBuffer;

    RsAssert(pxFA);
    RsAssert(xDuIsOk(pxDu));
    RsAssert(is_port_id_ok(xAppPortId));

    pxFlowAllocatorInstance = pxFAFindInstance(pxFA, xAppPortId);

    if (!pxFlowAllocatorInstance) {
        LOGE(TAG_FA, "Failed to find flow allocator instance");
        return false;
    }

    LOGD(TAG_FA, "Posting DU to port-id %d ", xAppPortId);

    pxNetworkBuffer = pxDu->pxNetworkBuffer;

    if (pxFlowAllocatorInstance->eFaiState != eFAI_ALLOCATED) {
        LOGE(TAG_FA, "Flow with port-id %d is not allocated", xAppPortId);
        return false;
    }

    /* Add the network packet to the list of packets to be
     * processed by the socket. */

    /* FIXME: Shouldn't this kind of signaling be done elsewhere? I
     * think we'd rather not put too much threads barriers within the
     * stack, and, if we do, it should be mostly all at the same
     * place. */

    pthread_mutex_lock(&pxFA->xMux);
    {
        vRsListInitItem(&(pxNetworkBuffer->xBufferListItem), (void *)pxNetworkBuffer);
        vRsListInsert(&(pxFlowAllocatorInstance->pxFlowAllocatorHandle->xListWaitingPackets), &(pxNetworkBuffer->xBufferListItem));
    }
    pthread_mutex_unlock(&pxFA->xMux);

    pthread_mutex_lock(&pxFlowAllocatorInstance->pxFlowAllocatorHandle->xEventMutex);
    {
        pxFlowAllocatorInstance->pxFlowAllocatorHandle->nEventBits |= eFLOW_RECEIVE;
        pthread_cond_signal(&pxFlowAllocatorInstance->pxFlowAllocatorHandle->xEventCond);
    }
    pthread_mutex_unlock(&pxFlowAllocatorInstance->pxFlowAllocatorHandle->xEventMutex);

    return true;
}
