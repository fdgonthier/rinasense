#ifndef _PORT_LINUX_GLIB_RSLIST_H
#define _PORT_LINUX_GLIB_RSLIST_H

#include <glib.h>

typedef GList RsListItem_t;

typedef struct xRSLIST_T {
    RsListItem_t pFirstItem;

    size_t nListLength;
} RsList_t;

#endif // _PORT_LINUX_GLIB_RSLIST_H
