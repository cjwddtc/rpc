
#include "rpc_framework.h"
#include "rpc_api.h"
uint64_t rpc_api_call(uint64_t module_id,uint64_t command_id,void *param){
    return RpcApiManager::call(module_id,command_id,param);
}