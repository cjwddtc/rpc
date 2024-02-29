
#ifndef MAIN_RPC_SERIALIZED_H
#define MAIN_RPC_SERIALIZED_H
#include <memory.h>
#include <vector>
#include <stdint.h>
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
    class CharIbuffer:public Ibuffer{
        char *m_buf;
    public:
        CharIbuffer(void *buf):m_buf{reinterpret_cast<char*>(buf)}{}

        void *offset(size_t size)override{
            auto ret=m_buf;
            m_buf+=size;
            return ret;
        }
    };
    class Obuffer:public Buffer{

    };
    class CommonObuffer:public rpc_serialized::Obuffer,public std::vector<char> {
        ;
    public:
        void serialized(void *ptr, size_t size) override {
            auto q=reinterpret_cast<char*>(ptr);
            insert(end(),q,q+size);
        }
    };

    template<class T>
    void serialized(Ibuffer &buffer,T &value){
        buffer.serialized(&value,sizeof(T));
    }
    template<class T>
    void serialized(Obuffer &buffer,T &value){
        buffer.serialized(&value,sizeof(T));
    }
    class ResponseTag{};
    template<class T>
    void serialized(Ibuffer &buffer,T &,ResponseTag){
        buffer.offset(sizeof(T));
    }
    template <class T>
    void serialized(Ibuffer &buffer,T *&value){
        value=reinterpret_cast<T*>(buffer.offset(sizeof(T)));
    }
    template <class T>
    void serialized(Obuffer &buffer,T *value,ResponseTag ){
        buffer.serialized(value,sizeof(T));
    }
    template <class T>
    void serialized(Obuffer &buffer,T *value){
        buffer.serialized(value,sizeof(T));
    }
    template <class T>
    void serialized(Ibuffer &buffer,T *value,ResponseTag){
        buffer.serialized(reinterpret_cast<void*>(value),sizeof(T));
    }
    inline void serialized(Obuffer &buffer,const char*value){
        uint32_t len=static_cast<uint32_t>(strlen(value)+1);
        buffer.serialized(&len,sizeof(len));
        buffer.serialized(const_cast<char*>(value), len);
    }

    inline void serialized(Ibuffer &buffer,const char*&value){
        uint32_t len;
        serialized(buffer,len);
        value=reinterpret_cast<char*>(buffer.offset(len));
    }

    inline void serialized(Ibuffer &buffer,const char*value,ResponseTag){
        uint32_t len;
        serialized(buffer,len);
        buffer.offset(len);
    }
}
#endif //MAIN_RPC_SERIALIZED_H
