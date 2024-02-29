#ifndef MAIN_RPC_FRAMEWORK_H
#define MAIN_RPC_FRAMEWORK_H
#include "common/api.h"
#include <stdint.h>
class RpcApiManager{
public:

#define BEGIN_MODULE(module) class module{public:
#define MODULE_FUNCTION(function,...) static dcmmReturn_t function(__VA_ARGS__);
#define END_MODULE() };
#include "../common/rpc_info.inc"

    static dcmmReturn_t call(uint64_t module_id,uint64_t command_id,void *param);
};
#endif //MAIN_RPC_FRAMEWORK_H
