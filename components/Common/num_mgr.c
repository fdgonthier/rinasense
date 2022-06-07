#include <stdint.h>

#include "portability/port.h"

#include "num_mgr.h"
#include "bit_array.h"

num_mgr_t *numMgrCreate(size_t numCnt)
{
    num_mgr_t *nm;

    /* INT_MAX is our error code. */
    if (numCnt == INT_MAX)
        return NULL;

    nm = pvRsMemAlloc(sizeof(num_mgr_t));
    if (!nm)
        return NULL;

    if (!numMgrInit(nm, numCnt)) {
        vRsMemFree(nm);
        return NULL;
    }

    return nm;
}

bool_t numMgrInit(num_mgr_t *im, size_t numCnt)
{
    im->numCnt = numCnt + 1;
    im->lastAllocated = 0;

    im->ba = bitarray_alloc(im->numCnt);
    if (!im->ba)
        return false;

    return true;
}

void numMgrFini(num_mgr_t *im)
{
    bitarray_free(im->ba);
}

void numMgrDestroy(num_mgr_t *im)
{
    vRsMemFree(im);
}

uint32_t numMgrAllocate(num_mgr_t *im)
{
    uint32_t p;

    RsAssert(im != NULL);

    p = im->lastAllocated + 1;

    /* Loops from the next to last allocated port, until we reach the last
     * allocated port again, wrapping over MAX_PORT_ID. */
    for (;; p++) {

        /* We return INT_MAX if we overflow. */
        if (p == im->lastAllocated)
            return INT_MAX;

        if (p == im->numCnt)
            p = 1;

        if (!bitarray_get_bit(im->ba, p)) {
            bitarray_set_bit(im->ba, p);
            im->lastAllocated = p;
            break;
        }
    }

    return p;
}

bool_t numMgrRelease(num_mgr_t *im, uint32_t n)
{
    if (!bitarray_get_bit(im->ba, n))
        return false;
    else {
        bitarray_clear_bit(im->ba, n);
        return true;
    }
}

bool_t numMgrIsAllocated(num_mgr_t *im, uint32_t n)
{
    return bitarray_get_bit(im->ba, n);
}
