
#include "rpc_framework.h"
#include <stdio.h>
dcmmReturn_t RpcApiManager::a::test1(char ch, int a) {
    printf("test1,%c,%d\n",ch,a);
    return DCMM_SUCCESS;
}


dcmmReturn_t RpcApiManager::a::test2(test_2 *data) {

    printf("test2,%f,%d\n", data->b,data->a);
    data->a=4;
    return DCMM_SUCCESS;
}
