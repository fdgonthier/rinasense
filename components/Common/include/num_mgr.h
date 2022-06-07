#ifndef _COMMON_NUM_MGR
#define _COMMON_NUM_MGR

#include <stdint.h>
#include <stddef.h>

#include "bit_array.h"
#include "portability/port.h"

typedef struct NUMMGR_T {
    /* Number of numbers to deal with. */
    size_t numCnt;

    /* Last number that was allocated. */
    uint32_t lastAllocated;

    /* Bit array storing the numbers. */
    bitarray_t *ba;

} num_mgr_t;

num_mgr_t *numMgrCreate(size_t numCnt);

bool_t numMgrInit(num_mgr_t *im, size_t numCnt);

void numMgrFini(num_mgr_t *im);

void numMgrDestroy(num_mgr_t *im);

uint32_t numMgrAllocate(num_mgr_t *im);

bool_t numMgrRelease(num_mgr_t *im, uint32_t n);

bool_t numMgrIsAllocated(num_mgr_t *im, uint32_t n);

#endif // _COMMON_NUM_MGR
