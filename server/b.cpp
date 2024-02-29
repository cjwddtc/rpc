#include "rpc_framework.h"
#include <stdio.h>
dcmmReturn_t RpcApiManager::b::test3(const char *name) {
    printf("test3:%s\n",name);
    return DCMM_TEST;
}