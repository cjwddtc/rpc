#ifndef MAIN_SHARED_TYPE_H
#define MAIN_SHARED_TYPE_H
#include <stdint.h>
struct rpc_request_header{
    uint64_t version;
    uint64_t module_id;
    uint64_t command;
    uint64_t content_size;
};
struct rpc_response_header{
    uint64_t version;
    uint64_t response_code;
    uint64_t content_size;
};
#endif //MAIN_SHARED_TYPE_H
