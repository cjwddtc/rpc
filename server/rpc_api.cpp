
#include "rpc_framework.h"
#include "rpc_api.h"
#include <boost/asio/awaitable.hpp>


boost::asio::awaitable<uint64_t> rpc_api_call(uint64_t module_id,uint64_t command_id,void *param){
    co_return co_await RpcApiManager::call(module_id,command_id,param);
}