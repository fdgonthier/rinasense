#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ARP826_defs.h"
#include "CdapMessage.h"
#include "Ribd_msg.h"
#include "SerDes.h"
#include "common/netbuf.h"
#include "common/rsrc.h"
#include "portability/port.h"
#include "common/rina_ids.h"
#include "common/rina_name.h"

#include "configRINA.h"
#include "configSensor.h"

#include "du.h"
#include "CDAP.pb.h"
#include "Enrollment_api.h"
#include "FlowAllocator_defs.h"
#include "FlowAllocator_api.h"
#include "IPCP_api.h"
#include "IPCP_events.h"
#include "IPCP_normal_api.h"
#include "rina_common_port.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "Ribd.h"
#include "Rib.h"
#include "rmt.h"
#include "RINA_API_flows.h"
#include "SerDesMessage.h"
#include "SerDesAData.h"

ribCallbackOps_t *prvRibdCreateCdapCallback(Ribd_t *pxRibd, opCode_t xOpCode, int invoke_id)
{
    ribCallbackOps_t *pxCallback;

    if (!(pxCallback = pxRsrcAlloc(pxRibd->xCbPool, "RIB callback")))
        return NULL;

    switch (xOpCode) {
    case M_START:
        pxCallback->start_response = xEnrollmentHandleStartR;
        break;

    case M_STOP:
        pxCallback->stop_response = xEnrollmentHandleStopR;
        break;

    case M_CREATE:
        pxCallback->create_response = xFlowAllocatorHandleCreateR;
        break;

    case M_DELETE:
        pxCallback->delete_response = xFlowAllocatorHandleDeleteR;
        break;

    default:
        break;
    }

    return pxCallback;
}

ribCallbackOps_t *prvRibdFindPendingResponseHandler(Ribd_t *pxRibd, int32_t invokeID)
{
    num_t x = 0;
    ribCallbackOps_t *pxCb;

    RsAssert(pxRibd);

    for (x = 0; x < RESPONSE_HANDLER_TABLE_SIZE; x++) {
        if (pxRibd->xPendingResponseHandlersTable[x].xValid == true)
            if (pxRibd->xPendingResponseHandlersTable[x].invokeID == invokeID)
                return pxRibd->xPendingResponseHandlersTable[x].pxCallbackHandler;
    }

    return NULL;
}

void prvRibdHandleAData(struct ipcpInstanceData_t *pxData, serObjectValue_t *pxObjValue)
{
    messageCdap_t *pxDecodeCdap;
    ribObject_t *pxRibObject;
    ribCallbackOps_t *pxCallback;

    pxDecodeCdap = pxSerDesMessageDecode(&pxData->xRibd.xMsgSD,
                                         pxObjValue->pvSerBuffer, pxObjValue->xSerLength);
    if (!pxDecodeCdap) {
        LOGE(TAG_RIB, "Failed to decode CDAP message");
        return;
    }

    vRibdPrintCdapMessage(pxDecodeCdap);

    if (pxDecodeCdap->eOpCode > MAX_CDAP_OPCODE) {
        LOGE(TAG_RIB, "Invalid opcode %s", opcodeNamesTable[pxDecodeCdap->eOpCode]);
        vRsrcFree(pxDecodeCdap);
    }

    LOGI(TAG_RIB, "Handling CDAP Message: %s", opcodeNamesTable[pxDecodeCdap->eOpCode]);

    pxRibObject = pxRibFindObject(&pxData->xRibd, pxDecodeCdap->pcObjName);

    switch (pxDecodeCdap->eOpCode) {
    case M_CREATE_R:
        pxCallback = prvRibdFindPendingResponseHandler(&pxData->xRibd, pxDecodeCdap->nInvokeID);
        if (!pxCallback) {
            LOGE(TAG_RIB, "Failed to find proper response handler");
            break;
        }

        if (!pxCallback->create_response(pxData, pxDecodeCdap->pxObjValue, pxDecodeCdap->result))
            LOGE(TAG_RIB, "It was not possible to handle the a_data message properly");

        break;

    case M_DELETE:
        if (pxRibObject->fnDelete)
            pxRibObject->fnDelete(pxData, pxRibObject, NULL,
                                  &pxDecodeCdap->xDestinationInfo,
                                  &pxDecodeCdap->xSourceInfo,
                                  pxDecodeCdap->nInvokeID,
                                  PORT_ID_WRONG);
        else
            LOGW(TAG_RIB, "Unsupported method M_DELETE on object %s", pxRibObject->ucObjName);

        break;
    default:

        break;
    }
}

appConnection_t *prvRibCreateConnection(rname_t *pxSource, rname_t *pxDestInfo)
{
    appConnection_t *pxAppConnectionTmp;
    rname_t *pxDestinationInfo, *pxSourceInfo;

    if (!((pxAppConnectionTmp = pvRsMemCAlloc(1, sizeof(appConnection_t))))) {
        LOGE(TAG_RIB, "Failed to allocate memory for application connection information");
        return NULL;
    }

    /* We need the copy the names in the structure since it'll get
     * inside the list of active connections */
    pxAppConnectionTmp->uCdapVersion = 0x01;

    xNameAssignDup(&pxAppConnectionTmp->xSourceInfo, pxSource);
    xNameAssignDup(&pxAppConnectionTmp->xDestinationInfo, pxDestInfo);

    pxAppConnectionTmp->xStatus = eCONNECTION_IN_PROGRESS;
    pxAppConnectionTmp->uRibVersion = 0x01;

    return pxAppConnectionTmp;
}

bool_t prvRibdAddAppConnectionEntry(Ribd_t *pxRibd, appConnection_t *pxAppConnectionToAdd, portId_t xPortId)
{
    num_t x = 0;

    RsAssert(pxRibd);
    RsAssert(pxAppConnectionToAdd);

    for (x = 0; x < APP_CONNECTION_TABLE_SIZE; x++) {

        if (pxRibd->xAppConnectionTable[x].xValid == false) {
            pxRibd->xAppConnectionTable[x].pxAppConnection = pxAppConnectionToAdd;
            pxRibd->xAppConnectionTable[x].xN1portId = xPortId;
            pxRibd->xAppConnectionTable[x].xValid = true;

            LOGI(TAG_RIB, "AppConnection Entry successful: %p,id:%d", pxAppConnectionToAdd, xPortId);

            return true;
        }
    }

    return false;
}

bool_t prvRibHandleMessage(struct ipcpInstanceData_t *pxData,
                           messageCdap_t *pxDecodeCdap,
                           portId_t unPort)
{
    bool_t ret = true;
    appConnection_t *pxAppCon;
    ribObject_t *pxRibObject;
    ribCallbackOps_t *pxCallback;

    /*Check if the Operation Code is valid*/
    if (pxDecodeCdap->eOpCode > MAX_CDAP_OPCODE) {
        LOGE(TAG_RIB, "Invalid opcode %s", opcodeNamesTable[pxDecodeCdap->eOpCode]);
        vRsMemFree(pxDecodeCdap);
        return ret;
    }

    LOGI(TAG_RIB, "Handling CDAP Message: %s", opcodeNamesTable[pxDecodeCdap->eOpCode]);

    /* Looking for an App Connection using the N-1 Flow Port */
    pxAppCon = pxRibdFindAppConnection(&pxData->xRibd, unPort);

    // vPrintAppConnection(pxAppConnectionTmp);

    /* Looking for the object into the RIB */
    pxRibObject = pxRibFindObject(&pxData->xRibd, pxDecodeCdap->pcObjName);

    /*if (!pxRibObject)
    {
        return false;
    }*/

    switch (pxDecodeCdap->eOpCode)
    {
    case M_CONNECT:
        /* 1. Check AppConnection status (If it does not exist, create
         * a new one) under the status eCONNECTION_IN_PROGRESS), else
         * put the current appConnection under the status
         * eCONNECTION_IN_PROGRESS
         *
         * 2. Call to the Enrollment Handler Connect request */

        /* If App Connection is not registered, create a new one */
        if (pxAppCon == NULL) {
            pxAppCon = prvRibCreateConnection(&pxDecodeCdap->xDestinationInfo,
                                              &pxDecodeCdap->xSourceInfo);

            if (!pxAppCon) {
                LOGE(TAG_RIB, "Failed to create new connection object");
                return false;
            }

            if (!xEnrollmentHandleConnect(pxData, pxAppCon->xDestinationInfo.pcProcessName, unPort)) {
                LOGE(TAG_RIB, "Failed to handle enrollment CONNECT message");
                return false;
            }

            prvRibdAddAppConnectionEntry(&pxData->xRibd, pxAppCon, unPort);
        }

        break;

    case M_CONNECT_R:
        /*
         * 1. Update the pxAppconnection status to eCONNECTED
         * 2. Call to the Enrollment Handle ConnectR
         */
        /* Check if the current AppConnection status is in progress */
        if (pxAppCon->xStatus != eCONNECTION_IN_PROGRESS) {
            LOGE(TAG_RIB, "Invalid AppConnection State");
            return true;
        }

        /*Update Connection Status*/
        xNameAssignDup(&pxAppCon->xDestinationInfo, &pxDecodeCdap->xSourceInfo);
        pxAppCon->xStatus = eCONNECTED;

        LOGI(TAG_RIB, "Application Connection Status Updated to 'CONNECTED'");

        /*Call to Enrollment Handle ConnectR*/
        xEnrollmentHandleConnectR(pxData, pxAppCon->xDestinationInfo.pcProcessName, unPort);

        break;

    case M_RELEASE_R:
        /*
         * 1. Update the pxAppconnection status to eRELEASED
         */
        if (pxAppCon->xStatus != eRELEASED) {
            LOGE(TAG_RIB, "The connection is already released");
            return true;
        }

        /*Update Connection Status*/
        pxAppCon->xStatus = eRELEASED;
        LOGI(TAG_RIB, "Status Updated Released");

        // TODO: delete App connection from the table (APPConnection)

        break;

    case M_CREATE:
        /* Calling the Create function of the enrollment object to create a Neighbor object and
         * add into the RibObject table
         */
        #if 0
        pxRibCreateObject(&pxData->xRibd,
                          pxDecodeCdap->pcObjName, pxDecodeCdap->objInst,
                          pxDecodeCdap->pcObjName, pxDecodeCdap->pcObjClass,
                          ENROLLMENT);
        #endif
        break;

    case M_STOP:
        LOGI(TAG_RIB, "Preparing a M_STOP_R");

        pxRibObject->fnStop(pxData,
                            pxRibObject,
                            pxDecodeCdap->pxObjValue,
                            &pxAppCon->xDestinationInfo,
                            &pxAppCon->xSourceInfo,
                            pxDecodeCdap->nInvokeID,
                            unPort);
        break;

    case M_START:
        LOGI(TAG_RIB, "Preparing a M_START_R");

        pxRibObject->fnStart(pxData,
                             pxRibObject,
                             pxDecodeCdap->pxObjValue,
                             &pxAppCon->xDestinationInfo,
                             &pxAppCon->xSourceInfo,
                             pxDecodeCdap->nInvokeID,
                             unPort);
        break;

    case M_START_R:
        /* Looking for a pending request */
        pxCallback = prvRibdFindPendingResponseHandler(&pxData->xRibd, pxDecodeCdap->nInvokeID);
        pxCallback->start_response(pxData, pxAppCon->xDestinationInfo.pcProcessName, pxDecodeCdap->pxObjValue);

        break;
    case M_STOP_R:
        pxCallback = prvRibdFindPendingResponseHandler(&pxData->xRibd, pxDecodeCdap->nInvokeID);
        pxCallback->stop_response(pxData, pxAppCon->xDestinationInfo.pcProcessName);
        break;
    case M_CREATE_R:
        pxCallback = prvRibdFindPendingResponseHandler(&pxData->xRibd, pxDecodeCdap->nInvokeID);
        pxCallback->create_response(pxData, pxDecodeCdap->pxObjValue, pxDecodeCdap->result);
        break;

    case M_WRITE:
        if (strcmp(pxDecodeCdap->pcObjName, "a_data") == 0) {
            LOGD(TAG_RIB, "Handling M_WRITE a_data sending to decode");

            aDataMsg_t *pxADataMsg;
            pxADataMsg = pxSerDesADataDecode(&pxData->xRibd.xADataSD,
                                             pxDecodeCdap->pxObjValue->pvSerBuffer,
                                             pxDecodeCdap->pxObjValue->xSerLength);

            /* FIXME: LOCAL_ADDRESS is here hardcoded where it
               shouldn't. */
            if (pxADataMsg->xDestinationAddress == LOCAL_ADDRESS)
                (void)prvRibdHandleAData(pxData, pxADataMsg->pxMsgCdap);
        }

        break;

    case M_READ:
        LOGW(TAG_RIB, "Preparing a M_READ_R on objName: %s", pxDecodeCdap->pcObjName);
        if (pxRibObject->fnRead)
            pxRibObject->fnRead(pxData,
                                pxRibObject,
                                pxDecodeCdap->pxObjValue,
                                &pxDecodeCdap->xDestinationInfo,
                                &pxDecodeCdap->xSourceInfo,
                                pxDecodeCdap->nInvokeID,
                                unPort);

        /* Iterate over the connection table. */

        /* Neighbors */
        
        break;

    default:
        LOGE(TAG_RIB, "Unhandled CDAP message type: %s", opcodeNamesTable[pxDecodeCdap->eOpCode]);
        ret = false;
        break;
    }

    return ret;
}

bool_t xRibdInit(Ribd_t *pxRibd)
{
    size_t unSz;

    if (!xSerDesMessageInit(&pxRibd->xMsgSD)) {
        LOGE(TAG_RIB, "Failed to initialise RIB message SerDes");
        return false;
    }

    if (!xSerDesADataInit(&pxRibd->xADataSD)) {
        LOGE(TAG_RIB, "Failed to initialise RIB ADATA SerDes");
        return false;
    }

    /* Pool for CDAP message content */
    if (!(pxRibd->xMsgPool = pxRsrcNewVarPool("RIB CDAP message pool", 0))) {
        LOGE(TAG_RIB, "Failed to allocate RIB CDAP message pool");
        return false;
    }

    /* Pool for callbacks attached to CDAP messages */
    unSz = sizeof(ribCallbackOps_t);
    if (!(pxRibd->xCbPool = pxRsrcNewPool("RIB callbacks pool", unSz, 1, 1, 0))) {
        LOGE(TAG_RIB, "Failed to allocate RIB callbacks pool");
        return false;
    }

    /* Pool for DU object containing CDAP data */
    if (!(pxRibd->xDuPool = xNetBufNewPool("RIBD DU pool"))) {
        LOGE(TAG_RIB, "Failed to allocate RIB DU pool");
        return false;
    }

    return true;
}

bool_t xRibdSendCdapMsg(Ribd_t *pxRibd, serObjectValue_t *pxSerVal, portId_t unPort)
{
    RINAStackEvent_t xEv;
    du_t *pxDu;
    size_t unHeaderSz, unSz;

    LOGI(TAG_RIB, "Sending the CDAP Message to the RMT");

    pxDu = pxNetBufNew(pxRibd->xDuPool, NB_RINA_DATA,
                       pxSerVal->pvSerBuffer, pxSerVal->xSerLength,
                       NETBUF_FREE_POOL);
    if (!pxDu) {
        LOGE(TAG_RIB, "Failed to allocate DU");
        return false;
    }

    xEv.eEventType = eSendMgmtEvent;
    xEv.xData.UN = unPort;
    xEv.xData2.DU = pxDu;

    if (!xSendEventStructToIPCPTask(&xEv, 1000)) {
        LOGE(TAG_RIB, "Failed to send management PDU to IPCP");
        return false;
    }

    return true;
}

bool_t xRibdSendResponse(Ribd_t *pxRibd,
                         string_t pcObjClass,
                         string_t pcObjName,
                         long objInst,
                         int result,
                         string_t pcResultReason,
                         opCode_t eOpCode,
                         int invokeId,
                         portId_t xN1Port,
                         serObjectValue_t *pxObjVal)
{
    messageCdap_t *pxMsgCdap = NULL;
    serObjectValue_t *pxSerVal;
    bool_t xStatus = false;

    switch (eOpCode) {
    case M_CONNECT:
        pxMsgCdap = pxRibdCdapMsgCreateRequest(pxRibd, pcObjClass, pcObjName, objInst, eOpCode, pxObjVal);
        break;

    case M_CONNECT_R:
    case M_START_R:
    case M_STOP_R:
    case M_READ_R:
        pxMsgCdap = pxRibdCdapMsgCreateResponse(pxRibd,
                                                pcObjClass, pcObjName, objInst, eOpCode, pxObjVal,
                                                result, pcResultReason, invokeId);
        break;

    default:
        break;
    }

    if (!pxMsgCdap) {
        LOGE(TAG_RIB, "Failed to generate CDAP message");
        return false;
    }

    if (!(pxSerVal = pxSerDesMessageEncode(&pxRibd->xMsgSD, pxMsgCdap))) {
        LOGE(TAG_RIB, "Failed to encode CDAP message");
        goto fail;
    }

    /* Send */
    xStatus = xRibdSendCdapMsg(pxRibd, pxSerVal, xN1Port);

    vRibdCdapMsgFree(pxMsgCdap);

    if (!xStatus)
        goto fail;

    return xStatus;

    fail:
    vRibdCdapMsgFree(pxMsgCdap);
    vRsrcFree(pxSerVal);

    return xStatus;
}

bool_t xRibdAddResponseHandler(Ribd_t *pxRibd, int32_t invokeID, ribCallbackOps_t *pxCb)
{
    num_t x = 0;

    RsAssert(pxRibd);
    RsAssert(pxCb);

    for (x = 0; x < RESPONSE_HANDLER_TABLE_SIZE; x++) {

        if (pxRibd->xPendingResponseHandlersTable[x].xValid == false) {
            pxRibd->xPendingResponseHandlersTable[x].invokeID = invokeID;
            pxRibd->xPendingResponseHandlersTable[x].xValid = true;
            pxRibd->xPendingResponseHandlersTable[x].pxCallbackHandler = pxCb;
            return true;
        }
    }

    LOGW(TAG_RIB, "Out of space in response handler table");

    return false;
}

appConnection_t *pxRibdFindAppConnection(Ribd_t *pxRibd, portId_t unPort)
{
    num_t x = 0;
    appConnection_t *pxAppConnection;

    RsAssert(pxRibd);

    for (x = 0; x < APP_CONNECTION_TABLE_SIZE; x++) {
        if (pxRibd->xAppConnectionTable[x].xValid == true)
            if (pxRibd->xAppConnectionTable[x].xN1portId == unPort)
                return pxRibd->xAppConnectionTable[x].pxAppConnection;
    }

    return NULL;
}

bool_t xRibdSend(Ribd_t *pxRibd,
                 string_t pcObjClass, string_t pcObjName, long objInst,
                 opCode_t eOpCode, portId_t xN1flowPortId, serObjectValue_t *pxObjVal)
{
    messageCdap_t *pxMsgCdap = NULL;
    serObjectValue_t *pxSerVal;
    bool_t xStatus = false;

    pxMsgCdap = pxRibdCdapMsgCreateRequest(pxRibd, pcObjClass, pcObjName, objInst, eOpCode, pxObjVal);

    /* This is an unconditional request, not expecting an answer. */
    pxMsgCdap->nInvokeID = 0;

    if (!(pxSerVal = pxSerDesMessageEncode(&pxRibd->xMsgSD, pxMsgCdap))) {
        LOGE(TAG_RIB, "Failed to encode CDAP message");
        goto fail;
    }

    xStatus = xRibdSendCdapMsg(pxRibd, pxSerVal, xN1flowPortId);

    fail:
    vRibdCdapMsgFree(pxMsgCdap);

    return xStatus;
}

bool_t xRibdSendRequest(Ribd_t *pxRibd,
                        string_t pcObjClass, string_t pcObjName, long objInst,
                        opCode_t eOpCode, portId_t xN1flowPortId, serObjectValue_t *pxObjVal)
{
    messageCdap_t *pxMsgCdap = NULL;
    ribCallbackOps_t *pxCb = NULL;
    serObjectValue_t *pxSerVal;
    bool_t xStatus = false;

    pxMsgCdap = pxRibdCdapMsgCreateRequest(pxRibd, pcObjClass, pcObjName, objInst, eOpCode, pxObjVal);

    if (!(pxCb = prvRibdCreateCdapCallback(pxRibd, eOpCode, pxMsgCdap->nInvokeID))) {
        LOGE(TAG_RIB, "Failed to create CDAP response callback");
        goto fail;
    }

    if (!xRibdAddResponseHandler(pxRibd, pxMsgCdap->nInvokeID, pxCb)) {
        LOGE(TAG_RIB, "Failed to add CDAP response handled");
        goto fail;
    }

    if (!(pxSerVal = pxSerDesMessageEncode(&pxRibd->xMsgSD, pxMsgCdap))) {
        LOGE(TAG_RIB, "Failed to encode CDAP message");
        goto fail;
    }

    xStatus = xRibdSendCdapMsg(pxRibd, pxSerVal, xN1flowPortId);

    fail:
    vRibdCdapMsgFree(pxMsgCdap);

    return xStatus;
}

bool_t xRibdProcessLayerManagementPDU(struct ipcpInstanceData_t *pxData,
                                      portId_t unPort,
                                      du_t *pxDu)
{
    messageCdap_t *pxDecodeCdap;

    LOGI(TAG_RIB, "Processing a Management PDU");

    RsAssert(eNetBufType(pxDu) == NB_RINA_DATA);

    /* Decode CDAP Message */
    pxDecodeCdap = pxSerDesMessageDecode(&pxData->xRibd.xMsgSD,
                                         pvNetBufPtr(pxDu),
                                         unNetBufSize(pxDu));
    if (!pxDecodeCdap) {
        LOGE(TAG_RIB, "Failed to decode management PDU");
        return false;
    }

    vRibdPrintCdapMessage(pxDecodeCdap);

    /* Call to rib Handle Message */
    if (!prvRibHandleMessage(pxData, pxDecodeCdap, unPort)) {
        LOGE(TAG_RIB, "Failed to handle management PDU");
        vRsrcFree(pxDecodeCdap);
        return false;
    }

    vRsrcFree(pxDecodeCdap);
    return true;
}

/* OLD CODE HERE */


bool encode_string(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
    const char *str = (const char *)(*arg);

    if (!pb_encode_tag_for_field(stream, field))
        return false;

    return pb_encode_string(stream, (uint8_t *)str, strlen(str));
}

void vPrintAppConnection(appConnection_t *pxAppConnection)
{
    LOGI(TAG_RIB, "--------APP CONNECTION--------");
    LOGI(TAG_RIB, "Destination EI:%s", pxAppConnection->xDestinationInfo.pcEntityInstance);
    LOGI(TAG_RIB, "Destination EN:%s", pxAppConnection->xDestinationInfo.pcEntityName);
    LOGI(TAG_RIB, "Destination PI:%s", pxAppConnection->xDestinationInfo.pcProcessInstance);
    LOGI(TAG_RIB, "Destination PN:%s", pxAppConnection->xDestinationInfo.pcProcessName);

    LOGI(TAG_RIB, "Source EI:%s", pxAppConnection->xSourceInfo.pcEntityInstance);
    LOGI(TAG_RIB, "Source EN:%s", pxAppConnection->xSourceInfo.pcEntityName);
    LOGI(TAG_RIB, "Source PI:%s", pxAppConnection->xSourceInfo.pcProcessInstance);
    LOGI(TAG_RIB, "Source PN:%s", pxAppConnection->xSourceInfo.pcProcessName);
    LOGI(TAG_RIB, "Connection Status:%d", pxAppConnection->xStatus);
}



