#ifndef MAIN_RPC_ENUM_H
#define MAIN_RPC_ENUM_H

namespace rpc_type {
    enum module_enum_t {
#define BEGIN_MODULE(module) module_##module##_id,

#include "rpc_info.inc"
    };

#define BEGIN_MODULE(module) enum module_##module##_command_enum_t{
#define MODULE_FUNCTION(function,...) command_##function##_id,
#define END_MODULE() };

#include "rpc_info.inc"

}
#endif //MAIN_RPC_ENUM_H
