#ifndef MAIN_RPC_API_H
#define MAIN_RPC_API_H
#include <boost/asio/awaitable.hpp>

boost::asio::awaitable<uint64_t> rpc_api_call(uint64_t module_id,uint64_t command_id,void *param);
#endif //MAIN_RPC_API_H
