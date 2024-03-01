#include "rpc_framework.h"
#include <stdio.h>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/strand.hpp>
boost::asio::awaitable<dcmmReturn_t> RpcApiManager::b::test3(const char *name) {
    static boost::asio::thread_pool pool(1);

    dcmmReturn_t ret{DCMM_SUCCESS};
    boost::asio::post(pool,[&ret](){
        // 模拟阻塞的设备操作
        sleep(5);
        ret=DCMM_TEST;
    });
    co_await boost::asio::post(pool,boost::asio::use_awaitable);
    printf("test3:%s\n",name);
    co_return ret;
}