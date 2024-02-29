#ifndef MAIN_SERVER_H
#define MAIN_SERVER_H
#include "shared_type.h"
#include "boost/preprocessor.hpp"


template <class T>
class RpcFunction;
template <class ...ARG>
class RpcFunction<dcmmReturn_t (ARG...)>{
    dcmmReturn_t(*m_ptr)(ARG...);
    std::vector<void(*)(ARG...)> m_other;
public:
    RpcFunction(dcmmReturn_t(*ptr)(ARG...)):m_ptr{ptr}{}
    dcmmReturn_t operator()(ARG...arg){
        auto ret=m_ptr;
        if(ret==0) {
            for(auto &a:m_other)
                a(arg...);
        }
        return m_ptr(arg...);
    }
};
class charIbuffer:public rpc_serialized::Ibuffer{
    char *m_buf;
public:
    charIbuffer(void *buf):m_buf{reinterpret_cast<char*>(buf)}{}

    void *offset(size_t size)override{
        auto ret=m_buf;
        m_buf+=size;
        return ret;
    }
};
template <class T>
T readParam(rpc_serialized::Ibuffer &buffer){
    T a;
    rpc_serialized::serialized(buffer,a);
    return a;
}
template <class ...ARG>
dcmmReturn_t callByParam(dcmmReturn_t (*ptr)(ARG...),void *param){
    charIbuffer buf{param};
    // TODO 这里依赖了参数求值顺序，反正目前都是gcc不会有问题
    return ptr(readParam<ARG>(buf)...);
}

class RpcApiManager{
public:

#define BEGIN_MODULE(module) class module{public:
#define MODULE_FUNCTION(function,...) static dcmmReturn_t function(__VA_ARGS__);
#define END_MODULE() };
#include "api.h"
    static dcmmReturn_t call(uint64_t module_id,uint64_t command_id,void *param){
        switch(module_id){
#define BEGIN_MODULE(module) case rpc_type::module_##module##_id:switch(command_id){ using module_type=RpcApiManager::module;
#define MODULE_FUNCTION(function,...) case rpc_type::command_##function##_id:return callByParam(&module_type::function ,param);
#define END_MODULE() default: return DCMM_INVALID_COMMAND;};
#include "api.h"
            default:
                return DCMM_INVALID_MODULE;
        }
    }
};
void *start_server();

void stop_server(void *ptr);
#endif //MAIN_SERVER_H
