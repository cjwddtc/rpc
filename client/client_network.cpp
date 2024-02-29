
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/preprocessor.hpp>
#include "common/shared_type.h"
#include "common/rpc_serialized.h"
#include "common/rpc_enum.h"
#include "common/api.h"
using boost::asio::ip::tcp;
using boost::asio::detached;
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
        rpc_serialized::CommonObuffer buf;
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
            rpc_serialized::CharIbuffer ibuf{buf.data()};

            nothing((rpc_serialized::serialized(ibuf, arg,rpc_serialized::ResponseTag{}),0)...);
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
#include "../common/rpc_info.inc"
#define BEGIN_MODULE(module) using namespace module##_function_##_impl;
#define MODULE_FUNCTION(function,...) extern "C" dcmmReturn_t function(DcmmHandle *handle BOOST_PP_SEQ_FOR_EACH_I(DECLARE_ARG,_,BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__)))){ \
                        return function##_impl(handle BOOST_PP_SEQ_FOR_EACH_I(FORWARE_ARG,_,BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__))));                                                                     \
}
#include "../common/rpc_info.inc"


dcmmReturn_t createHandle(DcmmHandle **handle){
    auto ptr=new DcmmHandle;
    *handle=ptr;
    return DCMM_SUCCESS;
}
dcmmReturn_t destroyHandle(DcmmHandle **handle){
    delete *handle;
    return DCMM_SUCCESS;
}