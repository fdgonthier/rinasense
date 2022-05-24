#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "rina_name.h"

int main() {
    {
        const string_t s1 = "hello";
        string_t s2, s3;

        xRstringDup(s1, &s2);
        assert(strcmp(s1, s2) == 0);

        xRstringDup(s1, &s3);
        assert(strcmp(s1, s3) == 0);
        assert(s1 != s3);

        vRsMemFree(s1);
        vRsMemFree(s2);
    }

    {
        name_t n1;

        xRinaNameFromString("processName|processInstance|entityName|entityInstance", &n1);
        assert(strcmp(n1.pcProcessName, "processName") == 0);
        assert(strcmp(n1.pcProcessInstance, "processInstance") == 0);
        assert(strcmp(n1.pcEntityName, "entityName") == 0);
        assert(strcmp(n1.pcEntityInstance, "entityInstance") == 0);
        vRstrNameDestroy(&n1);
    }
}
