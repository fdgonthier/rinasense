#include <string.h>
#include <glib.h>

#include "glib_rslist.h"
#include "portability/rslist.h"

void vRsListInit( RsList_t * const pList )
{
    memset( pList, 0, sizeof( RsList_t ) );
    vRsListInitItem( &( pList->pFirstItem ) );
}

void vRsListInitItem( RsListItem_t * const pListItem )
{
    memset( pListItem, 0, sizeof( RsListItem_t ) );
}

void vRsListInsert( RsList_t * const pList, RsListItem_t * const pListItem )
{
    if ( g_list_append( &( pList->pFirstItem ), pListItem ) )
    {
        pList->nListLength++;
    }
}

void vRsListInsertEnd( RsList_t * const pList, RsListItem_t * const pListItem )
{
    if ( g_list_append( &( pList->pFirstItem ), pListItem ) )
    {
        pList->nListLength++;
    }
}

void * pRsListGetOwnerOfHeadEntry( RsList_t * const pList )
{
    if ( pList->pFirstItem.next != NULL )
    {
        return pList->pFirstItem.next->data;
    }
    else
    {
        return NULL;
    }
}

void vRsListRemoveItem( RsListItem_t * const pListItem )
{
}

uint32_t unRsListCurrentListLength( RsList_t * const pList )
{
    return pList->nListLength;
}

bool vRsListIsContainedWithin( RsList_t * const pList, RsListItem_t * const pListItem )
{
    return g_list_find( &( pList->pFirstItem ), pListItem ) != NULL;
}

void vRsListSetListItemOwner( RsListItem_t * const pList, void * pOwner )
{
    pList->data = pOwner;
}

