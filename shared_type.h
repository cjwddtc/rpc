#ifndef MAIN_SHARED_TYPE_H
#define MAIN_SHARED_TYPE_H
enum dcmmReturn_t{
    DCMM_SUCCESS,
    DCMM_INVALID_MODULE,
    DCMM_INVALID_COMMAND,
    DCMM_INVALID_RESPONSE,
    DCMM_TEST,
};
#include <string>
#include <vector>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
struct test_2{
    int a;
    double b;
};
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
namespace rpc_serialized {
    class Buffer{
    public:
        virtual void serialized(void *ptr,size_t size)=0;
    };
    class Ibuffer:public Buffer{
    public:
        virtual void *offset(size_t)=0;
        void serialized(void *ptr,size_t size)override{
            memcpy(ptr, offset(size),size);
        }
    };
    class Obuffer:public Buffer{

    };
    class ResourceBase{
        virtual ~ResourceBase();
    };
    template <class T>
    class Resource{
        Resource(T &a){}
    };
    template <class T>
    class Resource<T*>{
        std::unique_ptr<T> m_ptr;
        Resource(T *&ptr):m_ptr{new T{}}{
            ptr=m_ptr.get();
        }
    };
    template <>
    class Resource<const char*>{
        std::vector<char> data;
        Resource(T *&ptr):m_ptr{new T{}}{
            ptr=m_ptr.get();
        }
    };

    template<class T>
    void serialized(Buffer &buffer,T &value){
        buffer.serialized(&value,sizeof(value));
    }
    template <class T>
    void serialized(Ibuffer &buffer,T *value){

        *value=*reinterpret_cast<T*>(buffer.offset(sizeof(T)));
    }
    template <class T>
    void serialized(Obuffer &buffer,T *value){
        buffer.serialized(value,sizeof(T));
    }
    inline void serialized(Obuffer &buffer,const char*value){
        uint32_t len=strlen(value)+1;
        buffer.serialized(&len,sizeof(len));
        buffer.serialized(const_cast<char*>(value), len);
    }

    inline void serialized(Ibuffer &buffer,const char*&value){
        uint32_t len;
        serialized(buffer,len);
        value=reinterpret_cast<char*>(buffer.offset(len));
    }
}
namespace rpc_type {
    enum module_enum_t {
#define BEGIN_MODULE(module) module_##module##_id,

#include "api.h"
    };

#define BEGIN_MODULE(module) enum module_##module##_command_enum_t{
#define MODULE_FUNCTION(function,...) command_##function##_id,
#define END_MODULE() };

#include "api.h"

}
#endif //MAIN_SHARED_TYPE_H
