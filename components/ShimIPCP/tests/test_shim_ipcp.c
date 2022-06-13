#include "IpcManager.h"
#include "portability/port.h"

#include "ShimIPCP.h"

struct ipcpFactories testFactories;

const MACAddress_t mac1 = {
    1, 2, 3, 4, 5, 6
};

const MACAddress_t mac2 = {
    2, 3, 4, 5, 6, 7
};

int main()
{
    name_t n1;
    ipcManager_t ipcMgr;
    num_mgr_t pidm;
    struct ipcpInstance *ipcp;

    xIpcManagerInit(&ipcMgr);

    vRsListInit(&(testFactories.xFactoriesList));

    RsAssert(numMgrInit(&pidm, 10));
    RsAssert(xRinaNameFromString("e1|e2|e3", &n1));
    RsAssert(xShimIPCPInitFactory(&testFactories));
    RsAssert(pxFactoryIPCPFind(&testFactories, eShimWiFi) != NULL);
    RsAssert(xIpcManagerCreateInstance(&testFactories, eShimWiFi, &pidm));

    /* We're cheating as we know the first IPCP ID is 1 here. */
    RsAssert((ipcp = pxIpcManagerFindInstanceById(1)) != NULL);
    RsAssert(ipcp->xId == 1);
    RsAssert(ipcp->xType == eShimWiFi);

    RsAssert(pxFactoryIPCPFind(&testFactories, eNormal) == NULL);
    RsAssert(!xIpcManagerCreateInstance(&testFactories, eNormal, &pidm));
}
