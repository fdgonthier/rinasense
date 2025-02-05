#ifndef SERDES_MESSAGE_H_INCLUDED
#define SERDES_MESSAGE_H_INCLUDED

#include "portability/port.h"
#include "common/rsrc.h"

#include "CDAP.pb.h"
#include "SerDes.h"
#include "Ribd_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    rsrcPoolP_t xDecPool;
    rsrcPoolP_t xEncPool;

} MessageSerDes_t;

bool_t xSerDesMessageInit(MessageSerDes_t *pxSD);

rsErr_t xSerDesMessageEncode(MessageSerDes_t *pxSD,
                             messageCdap_t *pxMsg,
                             serObjectValue_t *pxSerValue);

messageCdap_t *pxSerDesMessageDecode(MessageSerDes_t *pxSD,
                                     uint8_t *pucBuffer,
                                     size_t xMessageLength);

void vRibPrintCdapMessage(const string_t pcTag, const string_t pcTitle, messageCdap_t *pxDecodeCdap);

void vSerDesMessageFree(messageCdap_t *pxMsg);

#ifdef __cplusplus
}
#endif

#endif /* SERDES_MESSAGE_H_INCLUDED */

