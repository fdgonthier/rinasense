#ifndef _RS_LIST_H_
#define _RS_LIST_H

#include <stdint.h>
#include "port.h"

void vRsListInit( RsList_t * const );

void vRsListInitItem( RsListItem_t * const );

void vRsListInsert( RsList_t * const, RsListItem_t * const );

void vRsListInsertEnd( RsList_t * const, RsListItem_t * const );

void * pRsListGetOwnerOfHeadEntry( RsList_t * const );

void vRsListRemoveItem( RsListItem_t * const );

uint32_t unRsListCurrentListLength( RsList_t * const );

bool vRsListIsContainedWithin( RsList_t * const, RsListItem_t * const );

void vRsListSetListItemOwner( RsListItem_t * const, void * );

#endif // _RS_LIST_H

