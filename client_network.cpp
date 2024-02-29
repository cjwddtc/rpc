
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/asio/write.hpp>
#include <boost/preprocessor.hpp>
#include "shared_type.h"

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;
/*
awaitable<void> echo(tcp::socket socket) {
    try {
        co_await async_write(socket, boost::asio::buffer(&header, sizeof(header)), use_awaitable);
    }
    catch (std::exception &e) {
        std::printf("echo Exception: %s\n", e.what());
    }
}*/
class Obuffer:public rpc_serialized::Obuffer,public std::vector<char> {
    ;
public:
    void serialized(void *ptr, size_t size) override {
        auto q=reinterpret_cast<char*>(ptr);
        insert(end(),q,q+size);
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
struct DcmmHandle;
class ClientNetwork{
    boost::asio::io_context m_ctx;
    tcp::socket m_socket;
public:
    ClientNetwork():m_ctx(), m_socket(m_ctx){

        tcp::endpoint end_point{boost::asio::ip::address::from_string("127.0.0.1"), 32001};
        m_socket.connect(end_point);
    }
    template <class ...ARG>void nothing(ARG...){}
    template <class ...ARG>
    dcmmReturn_t callWithArg(uint64_t module_id,uint64_t command_id,dcmmReturn_t(*)(DcmmHandle *,ARG...) ,ARG...arg) {
        Obuffer buf;
        {
            nothing((rpc_serialized::serialized(buf, arg), 0)...);
            rpc_request_header header{0, module_id, command_id, buf.size()};
            m_socket.write_some(boost::asio::buffer(&header, sizeof(header)));
            m_socket.write_some(boost::asio::buffer(buf));
        }
        {
            const char *p=buf.data();
            rpc_response_header header;
            m_socket.receive(boost::asio::buffer(&header,sizeof(header)));
            if(header.content_size!=buf.size()) {
                return DCMM_INVALID_RESPONSE;
            }
            m_socket.receive(boost::asio::buffer(buf));
            charIbuffer ibuf{buf.data()};
            nothing((rpc_serialized::serialized(ibuf,arg),0)...);
            return static_cast<dcmmReturn_t>(header.response_code);
        }
    }
    ~ClientNetwork(){
        m_socket.close();
    }
};

struct DcmmHandle {
    ClientNetwork network;
};

#define DECLARE_ARG(r,data,i,value) ,value BOOST_PP_CAT(arg_,i)
#define FORWARE_ARG(r,data,i,value) ,BOOST_PP_CAT(arg_,i)
#define BEGIN_MODULE(module) namespace module##_function_##_impl{static constexpr uint64_t module_id=rpc_type::module_##module##_id;
#define MODULE_FUNCTION(function,...) inline dcmmReturn_t function##_impl(DcmmHandle *handle BOOST_PP_SEQ_FOR_EACH_I(DECLARE_ARG,_,BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__)))){ \
                       return  handle->network.callWithArg(module_id,rpc_type::command_##function##_id,&function##_impl BOOST_PP_SEQ_FOR_EACH_I(FORWARE_ARG,_,BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__))));  \
}
#define END_MODULE() }
#include "api.h"
#define BEGIN_MODULE(module) using namespace module##_function_##_impl;
#define MODULE_FUNCTION(function,...) dcmmReturn_t function(DcmmHandle *handle BOOST_PP_SEQ_FOR_EACH_I(DECLARE_ARG,_,BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__)))){ \
                        return function##_impl(handle BOOST_PP_SEQ_FOR_EACH_I(FORWARE_ARG,_,BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__))));                                                                     \
}
#include "api.h"

static dcmmReturn_t call(uint64_t module_id,uint64_t command_id,void *param){
    switch(module_id){
#define BEGIN_MODULE(module) case rpc_type::module_##module##_id:switch(command_id){ using module_type=RpcApiManager::module;
#define MODULE_FUNCTION(function,...) case rpc_type::command_##function##_id:return callByParam(module_type ,param);
#define END_MODULE() default: return DCMM_INVALID_COMMAND;};
        default:
            return DCMM_INVALID_MODULE;
    }
}

int zxc(){
    DcmmHandle handle;

    //test1(&handle,'c',5);
    test_2 t{5,5.5};
    test2(&handle,&t);
    printf("%d,%f\n",t.a,t.b);

//    test3(&handle,"qwe");
    return 0;
}