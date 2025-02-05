#include "portability/port.h"
#include "common/error.h"
#include "common/rina_name.h"
#include "common/rinasense_errors.h"
#include "common/rsrc.h"

#include "SerDes.h"
#include "SerDesMessage.h"
#include "CDAP.pb.h"
#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "Ribd_defs.h"

#include "configRINA.h"

#define TAG_SD_MSG "[SD-MSG]"

void vRibPrintCdapMessage(const string_t pcTag, const string_t pcTitle, messageCdap_t *pxDecodeCdap)
{
    LOG_LOCK();

    LOGD(pcTag, "---- %s ----", pcTitle);
    LOGD(pcTag, "opCode: %s", opcodeNamesTable[pxDecodeCdap->eOpCode]);
    LOGD(pcTag, "Invoke Id: %d ", pxDecodeCdap->nInvokeID);
    LOGD(pcTag, "Version: %jd", pxDecodeCdap->version);

    if (pxDecodeCdap->xAuthPolicy.pcName != NULL) {
        LOGD(pcTag, "AuthPolicy Name: %s", pxDecodeCdap->xAuthPolicy.pcName);
        LOGD(pcTag, "AuthPolicy Version: %s", pxDecodeCdap->xAuthPolicy.pcVersion);
    }

    LOGD(pcTag, "Source AEI: %s", pxDecodeCdap->xSourceInfo.pcEntityInstance);
    LOGD(pcTag, "Source AEN: %s", pxDecodeCdap->xSourceInfo.pcEntityName);
    LOGD(pcTag, "Source API: %s", pxDecodeCdap->xSourceInfo.pcProcessInstance);
    LOGD(pcTag, "Source APN: %s", pxDecodeCdap->xSourceInfo.pcProcessName);
    LOGD(pcTag, "Dest AEI: %s", pxDecodeCdap->xDestinationInfo.pcEntityInstance);
    LOGD(pcTag, "Dest AEN: %s", pxDecodeCdap->xDestinationInfo.pcEntityName);
    LOGD(pcTag, "Dest API: %s", pxDecodeCdap->xDestinationInfo.pcProcessInstance);
    LOGD(pcTag, "Dest APN: %s", pxDecodeCdap->xDestinationInfo.pcProcessName);
    LOGD(pcTag, "Result: %d", pxDecodeCdap->result);

    if (pxDecodeCdap->pcObjName)
        LOGD(pcTag, "ObjectName:%s", pxDecodeCdap->pcObjName);

    if (pxDecodeCdap->objInst)
        LOGD(pcTag, "ObjectInstance:%d", (int)pxDecodeCdap->objInst);

    if (pxDecodeCdap->pcObjClass)
        LOGD(pcTag, "ObjectClass:%s", pxDecodeCdap->pcObjClass);

    if (pxDecodeCdap->pxObjValue)
        LOGD(pcTag, "ObjectValue:%zu byte(s)", pxDecodeCdap->pxObjValue->xSerLength);

    LOG_UNLOCK();
}

bool_t xSerDesMessageInit(MessageSerDes_t *pxSD)
{
    size_t unSz;

    unSz = member_size(rina_messages_CDAPMessage, objClass)
        + member_size(rina_messages_CDAPMessage, objName)
        + member_size(rina_messages_CDAPMessage, resultReason)
        + member_size(rina_messages_authPolicy_t, name)
        + member_size(rina_messages_authPolicy_t, versions)
        + sizeof(rina_messages_objVal_t_byteval_t)
        + sizeof(messageCdap_t)
        + sizeof(serObjectValue_t);
    if (!(pxSD->xDecPool = pxRsrcNewPool("CDAP Message SerDes Decoding",
                                         unSz,
                                         CFG_SD_MESSAGE_DEC_POOL_INIT_ALLOC,
                                         CFG_SD_MESSAGE_DEC_POOL_INCR_ALLOC,
                                         CFG_SD_MESSAGE_DEC_POOL_MAX_RES)))
        return false;

    unSz = sizeof(rina_messages_CDAPMessage);
    if (!(pxSD->xEncPool = pxRsrcNewPool("CDAP Message SerDes Encoding",
                                         unSz,
                                         CFG_SD_MESSAGE_ENC_POOL_INIT_ALLOC,
                                         CFG_SD_MESSAGE_ENC_POOL_INCR_ALLOC,
                                         CFG_SD_MESSAGE_ENC_POOL_MAX_RES)))
        return false;

    return true;
}

rsErr_t xSerDesMessageEncode(MessageSerDes_t *pxSD, messageCdap_t *pxMsgCdap, serObjectValue_t *pxSerValue)
{
    bool_t xStatus;
    pb_ostream_t xStream;
    rina_messages_CDAPMessage xMsg = rina_messages_CDAPMessage_init_zero;

    /*Fill the message properly*/
    xMsg.version = pxMsgCdap->version;
    xMsg.has_version = true;
    xMsg.opCode = (rina_messages_opCode_t)pxMsgCdap->eOpCode;
    xMsg.invokeID = pxMsgCdap->nInvokeID;
    xMsg.has_invokeID = true;

    if (pxMsgCdap->result != -1) {
        xMsg.result = pxMsgCdap->result;
        xMsg.has_result = true;
    }

#define _COPY(x, i, y)                             \
    if (y) {                                       \
        strncpy(x.i , y, sizeof(x.i));             \
        xMsg.has_##i = true;                       \
    }

    /* Destination */
    _COPY(xMsg, destAEInst, pxMsgCdap->xDestinationInfo.pcEntityInstance);
    _COPY(xMsg, destAEName, pxMsgCdap->xDestinationInfo.pcEntityName);
    _COPY(xMsg, destApInst, pxMsgCdap->xDestinationInfo.pcProcessInstance);
    _COPY(xMsg, destApName, pxMsgCdap->xDestinationInfo.pcProcessName);

    /* Source */
    _COPY(xMsg, srcAEInst, pxMsgCdap->xSourceInfo.pcEntityInstance);
    _COPY(xMsg, srcAEName, pxMsgCdap->xSourceInfo.pcEntityName);
    _COPY(xMsg, srcApInst, pxMsgCdap->xSourceInfo.pcProcessInstance);
    _COPY(xMsg, srcApName, pxMsgCdap->xSourceInfo.pcProcessName);

    /* Authentication Policy */
    if (pxMsgCdap->xAuthPolicy.pcName != NULL) {
        strcpy(xMsg.authPolicy.name, pxMsgCdap->xAuthPolicy.pcName);
        xMsg.has_authPolicy = true;
        xMsg.authPolicy.versions_count = 1;
        strncpy(xMsg.authPolicy.versions[0], pxMsgCdap->xAuthPolicy.pcVersion,
                sizeof(xMsg.authPolicy.versions[0]));

        xMsg.authPolicy.has_name = true;
    }

    /* Result reason */
    _COPY(xMsg, resultReason, pxMsgCdap->pcResultReason);

    /* Object value */
    _COPY(xMsg, objClass, pxMsgCdap->pcObjClass);

    /* Object name */
    _COPY(xMsg, objName, pxMsgCdap->pcObjName);

    /* Object instance */
    if (pxMsgCdap->objInst != -1) {
        xMsg.objInst = pxMsgCdap->objInst;
        xMsg.has_objInst = true;
    }

    /* Object value */
    if (pxMsgCdap->pxObjValue != NULL) {
        xMsg.objValue.byteval.size = pxMsgCdap->pxObjValue->xSerLength;
        memcpy(xMsg.objValue.byteval.bytes, pxMsgCdap->pxObjValue->pvSerBuffer, pxMsgCdap->pxObjValue->xSerLength);

        xMsg.has_objValue = true;
        xMsg.objValue.has_byteval = true;
    }

    if (!(pxSerValue->pvSerBuffer = pxRsrcAlloc(pxSD->xEncPool, "CDAP message encoding")))
        return ERR_SET_OOM;

    xStream = pb_ostream_from_buffer((pb_byte_t *)pxSerValue->pvSerBuffer,
                                     sizeof(rina_messages_CDAPMessage));

    /*Encode the message*/
    xStatus = pb_encode(&xStream, rina_messages_CDAPMessage_fields, &xMsg);
    pxSerValue->xSerLength = xStream.bytes_written;

    if (!xStatus)
        return ERR_SET(ERR_SERDES_ENCODING_FAIL);

    LOGI(TAG_RIB, "Message CDAP with length: %zu encoded sucessfully ", pxSerValue->xSerLength);

    return SUCCESS;
}

messageCdap_t *pxSerDesMessageDecode(MessageSerDes_t *pxSD, uint8_t *pucBuffer, size_t xMessageLength)
{
    bool_t xStatus;
    pb_istream_t xStream;
    messageCdap_t *pxMsg;
    void *px;
    rina_messages_CDAPMessage message = rina_messages_CDAPMessage_init_zero;

    /*Create a stream that will read from the buffer*/
    xStream = pb_istream_from_buffer((pb_byte_t *)pucBuffer, xMessageLength);

    xStatus = pb_decode(&xStream, rina_messages_CDAPMessage_fields, &message);

    if (!xStatus)
        return ERR_SET_NULL(ERR_SERDES_DECODING_FAIL);

    if (!(pxMsg = pxRsrcAlloc(pxSD->xDecPool, "CDAP Message Decoding")))
        return ERR_SET_OOM_NULL;

    memset(pxMsg, 0, sizeof(messageCdap_t));

    if (ERR_CHK(xNameAssignFromPartsDup(&pxMsg->xDestinationInfo,
                                        message.destApName, message.destApInst,
                                        message.destAEName, message.destAEInst))) {
        LOGE(TAG_SD_MSG, "Failed to assign destination RINA name");
        return NULL;
    }

    if (ERR_CHK(xNameAssignFromPartsDup(&pxMsg->xSourceInfo,
                                        message.srcApName, message.srcApInst,
                                        message.srcAEName, message.srcAEInst))) {
        LOGE(TAG_SD_MSG, "Failed to assigne source RINA name");
        return NULL;
    }

    pxMsg->eOpCode = (opCode_t)message.opCode;
    pxMsg->version = message.version;
    pxMsg->nInvokeID = message.invokeID;
    pxMsg->result = message.result;
    pxMsg->objInst = message.objInst;

    /* Initialize the string pointers */
    pxMsg->pcObjClass = (px = (void *)pxMsg + sizeof(messageCdap_t));
    pxMsg->pcObjName = (px += member_size(rina_messages_CDAPMessage, objClass));
    pxMsg->pcResultReason = (px += member_size(rina_messages_CDAPMessage, resultReason));
    pxMsg->xAuthPolicy.pcName = (px += member_size(rina_messages_CDAPMessage, objName));
    pxMsg->xAuthPolicy.pcVersion = (px += member_size(rina_messages_authPolicy_t, name));

    /* Move the pointer past the last field */
    px += member_size(rina_messages_authPolicy_t, versions);

    /* Copy the strings */
    if (message.has_objClass)
        strcpy(pxMsg->pcObjClass, message.objClass);
    else
        strcpy(pxMsg->pcObjClass, "");

    if (message.has_objName)
        strcpy(pxMsg->pcObjName, message.objName);
    else
        strcpy(pxMsg->pcObjName, "");

    if (message.has_authPolicy) {
        if (message.has_authPolicy && message.authPolicy.has_name)
            strcpy(pxMsg->xAuthPolicy.pcName, message.authPolicy.name);

        strcpy(pxMsg->xAuthPolicy.pcVersion, message.authPolicy.versions[0]);
    }
    else {
        strcpy(pxMsg->xAuthPolicy.pcName, "");
        strcpy(pxMsg->xAuthPolicy.pcVersion, "");
    }

    if (message.has_resultReason)
        strcpy(pxMsg->pcResultReason, message.resultReason);
    else
        strcpy(pxMsg->pcResultReason, "");

    if (message.has_objValue) {
        pxMsg->pxObjValue = px;
        pxMsg->pxObjValue->pvSerBuffer = (px += sizeof(serObjectValue_t));
        pxMsg->pxObjValue->xSerLength = message.objValue.byteval.size;

        memcpy(pxMsg->pxObjValue->pvSerBuffer, message.objValue.byteval.bytes,
               pxMsg->pxObjValue->xSerLength);
    }

    return pxMsg;
}

void vSerDesMessageFree(messageCdap_t *pxMsg)
{
    vNameFree(&pxMsg->xDestinationInfo);
    vNameFree(&pxMsg->xSourceInfo);

    vRsrcFree(pxMsg);
}
