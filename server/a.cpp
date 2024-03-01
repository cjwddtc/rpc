
#include "rpc_framework.h"
#include <stdio.h>
boost::asio::awaitable<dcmmReturn_t> RpcApiManager::a::test1(char ch, int a) {
    printf("test1,%c,%d\n",ch,a);
    co_return DCMM_SUCCESS;
}


boost::asio::awaitable<dcmmReturn_t> RpcApiManager::a::test2(test_2 *data) {

    printf("test2,%f,%d\n", data->b,data->a);
    data->a=4;
    co_return DCMM_SUCCESS;
}
