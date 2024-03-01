#include "rpc_framework.h"
#include "common/rpc_serialized.h"
#include "common/rpc_enum.h"
namespace RpcApiManagerImpl {
    template<class T>
    T readParam(rpc_serialized::Ibuffer &buffer) {
        T a;

        rpc_serialized::serialized(buffer, a);
        return a;
    }

    template<class ...ARG>
    boost::asio::awaitable<dcmmReturn_t> callByParam(boost::asio::awaitable<dcmmReturn_t> (*ptr)(ARG...), void *param) {
        rpc_serialized::CharIbuffer buf{param};
        // TODO 这里依赖了参数求值顺序，反正目前都是gcc不会有问题
        co_return co_await ptr(readParam<ARG>(buf)...);
    }
}
boost::asio::awaitable<dcmmReturn_t> RpcApiManager::call(uint64_t module_id,uint64_t command_id,void *param){
    switch(module_id){
#define BEGIN_MODULE(module) case rpc_type::module_##module##_id:switch(command_id){ using module_type=RpcApiManager::module;
#define MODULE_FUNCTION(function,...) case rpc_type::command_##function##_id:co_return co_await RpcApiManagerImpl::callByParam(&module_type::function ,param);
#define END_MODULE() default: co_return DCMM_INVALID_COMMAND;};
#include "../common/rpc_info.inc"

        default:
            co_return DCMM_INVALID_MODULE;
    }
}