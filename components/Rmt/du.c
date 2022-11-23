/*
 * du.c
 *
 *  Created on: 30 sept. 2021
 *      Author: i2CAT
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common/netbuf.h"
#include "common/rsrc.h"
#include "rmt.h"
#include "du.h"
#include "pci.h"

#define TAG_DTP "[DTP]"

bool_t xDuDecap(du_t *pxDu)
{
    RsAssert(eNetBufType(pxDu) == NB_RINA_PCI);
    return xNetBufPop(pxDu, unNetBufSize(pxDu));
}

bool_t xDuEncap(void *pvPci, size_t unSz, du_t *pxDu)
{
    netbuf_t *pxNb;

    RsAssert(eNetBufType(pxDu) == NB_RINA_DATA);

    if (!(pxNb = pxNetBufNew(pxDu->xPool, NB_RINA_PCI, pvPci, unSz, NETBUF_FREE_POOL)))
        return false;

    vNetBufAppend(pxNb, pxDu);

    return true;
}

size_t xDuLen(du_t *pxDu)
{
    return unNetBufTotalSize(pxDu);
}

bool_t xDuIsOk(const du_t *pxDu)
{
    return true;
}
