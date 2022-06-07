#include "num_mgr.h"

void test8bits()
{
    num_mgr_t *nm;

    // Allocate all 3 available numbers.
    RsAssert((nm = numMgrCreate(3)) != NULL);
    RsAssert(numMgrAllocate(nm) == 1);
    RsAssert(numMgrAllocate(nm) == 2);
    RsAssert(numMgrAllocate(nm) == 3);
    RsAssert(numMgrAllocate(nm) == INT_MAX);

    // Free the middle one
    RsAssert(numMgrRelease(nm, 2));

    // Make sure we get the one we just free if we allocate again.
    RsAssert(numMgrAllocate(nm) == 2);
    RsAssert(numMgrAllocate(nm) == INT_MAX);

    numMgrDestroy(nm);
}

void testManyBits()
{
    num_mgr_t nm;
    int i = 0;

    RsAssert(numMgrInit(&nm, SHRT_MAX - 1));

    for (i = 1; i <= SHRT_MAX - 1; i++)
        RsAssert(numMgrAllocate(&nm) == i);

    RsAssert((i = numMgrAllocate(&nm)) == INT_MAX);

    numMgrFini(&nm);
}

int main()
{
    test8bits();
    testManyBits();
}
