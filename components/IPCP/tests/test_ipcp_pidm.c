#include <stdio.h>
#include "pidm.h"
#include "rina_ids.h"

#include "portability/port.h"

/* Tests initialization */
void testCreateDestroy()
{
    pidm_t *p;

    RsAssert((p = pxPidmCreate()) != NULL);
    RsAssert(xPidmDestroy(p));
}

/* Tests that we can allocate a port at all. */
void testAllocate()
{
    pidm_t *p;
    portId_t port;

    RsAssert((p = pxPidmCreate()) != NULL);
    RsAssert(is_port_id_ok(port = xPidmAllocate(p)));
    RsAssert(xPidmAllocated(p, port));
    RsAssert(xPidmDestroy(p));
}

/* Tests basic alloc/release cycle. */
void testAllocateRelease()
{
    pidm_t *p;
    portId_t port;

    RsAssert((p = pxPidmCreate()) != NULL);
    RsAssert(is_port_id_ok(port = xPidmAllocate(p)));
    RsAssert(xPidmAllocated(p, port));
    RsAssert(xPidmRelease(p, port));
    RsAssert(!xPidmAllocated(p, port));
    RsAssert(xPidmDestroy(p));
}

/* Tests the behaviour of the PIDM code when the maximum number of
 * ports are allocated. */
void testAllocateMany()
{
    pidm_t *p;
    portId_t port;

    RsAssert((p = pxPidmCreate()) != NULL);

    /* Allocates the maximum! */
    for (int i = 1; i < 2047; i++) {
        port = xPidmAllocate(p);
        RsAssert(port == i);
        RsAssert(is_port_id_ok(port));
    }

    /* Make sure we can't allocate more ports at this point. */
    RsAssert(!is_port_id_ok(xPidmAllocate(p)));

    /* Free a port, make sure we can allocate afterward, and make sure
     * the port we get is the one we just freed. */
    xPidmRelease(p, 1024);
    RsAssert((is_port_id_ok(port = xPidmAllocate(p))));
    RsAssert(port == 1024);

    RsAssert(xPidmDestroy(p));
}

int main() {
    testCreateDestroy();
    testAllocate();
    testAllocateRelease();
    testAllocateMany();
    exit(0);
}
