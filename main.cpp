
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/asio/write.hpp>
#include <cstdio>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <map>
using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;
class RpcClient{
public:
    virtual std::string send(std::string str)=0;
};

namespace tuple_serialization_tool{

    template<typename Archive,typename T, size_t ...INDEX>
    Archive &serialize(Archive &ar, T &t, const unsigned int version) {
        std::initializer_list<int>{((ar &std::get<INDEX>(t)),0)...};
        return ar;
    }
}
namespace boost {
    namespace serialization {

        template<typename Archive, typename ... Elements>
        Archive &serialize(Archive &ar, std::tuple<Elements...> &t, const unsigned int version) {
            ::tuple_serialization_tool::serialize(ar,t,version);

            return ar;
        }

    } // serialization
} // boost
template <class RET,class ...ARG>
class FuncTag{
    RpcClient *m_client;
    FuncTag(FuncTag *client):m_client{client}{}
    RET operator()(ARG...arg)const {
        std::stringstream stream;
        {
            boost::archive::text_oarchive oa(stream);
            std::tuple<ARG...> tup(std::move(arg)...);
            oa >> tup;
        }
        auto ret_str = m_client->send(stream.str());
        stream.str(ret_str);
        boost::archive::text_iarchive ia(stream);
        RET ret;
        ia >> ret;
        return ret;
    }
};
class RpcServerManager {
    class RpcFunctionBase{
    public:
        virtual std::string call(const std::string &arg)=0;
    };
    template <class RET,class ...ARG>
    class RpcFunction:public RpcFunctionBase{
        RET (*m_ptr)(ARG...);
    public:
        std::string call(const std::string &arg)override{
            std::tuple<ARG...> tup;
            std::stringstream stream(arg);
            {
                boost::archive::text_iarchive ia(stream);
                ia >> tup;
            }
            RET ret=std::apply(m_ptr,std::move(tup));
            {
                stream.str("");
                boost::archive::text_oarchive oa(stream);
                oa << ret;
            }
            return stream.str();
        }
    public:
        RpcFunction(RET (*ptr)(ARG...)):m_ptr{ptr}{}
    };
    uint64_t module_id;
    std::vector<std::vector<std::unique_ptr<RpcFunctionBase>>> m_command_map;
public:
    void insert(){}
    template <class RET,class ...ARG,class ...PTRS>
    void insert(RET(*ptr)(ARG...),PTRS ...ptrs){
        m_command_map.back().emplace_back(std::make_unique<RpcFunction<RET,ARG...>>(ptr));
        insert(ptrs...);
    }
    std::string call(uint64_t module_id,uint64_t command,const std::string &str){
        return m_command_map.at(module_id).at(command)->call(str);
    }
    static RpcServerManager&getServer(){
        static RpcServerManager manager;
        return manager;
    }
};
#define DECLARE_FUNC(MODULE_NAME,...) static bool MODULE_NAME##init_flag=((void)RpcServerManager::getServer().insert(__VA_ARGS__),true);
#include "api.h"


#define DECLARE_RPC(funcname,ret,...)


#if defined(BOOST_ASIO_ENABLE_HANDLER_TRACKING)
# define use_awaitable \
  boost::asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif


double test1(char ch, std::string str) {
    return 0;
}

std::string test2(std::vector<int> arr) {
    return std::string();
}

long test3(std::vector<std::string> arr) {
    return 0;
}


awaitable<void> echo_once(tcp::socket& socket)
{
    std::string ret;
    {
        rpc_request_header header;

        co_await socket.async_receive(boost::asio::buffer(&header, sizeof(header)), use_awaitable);
        std::vector<char> buffer(header.content_size);
        co_await socket.async_receive(boost::asio::buffer(buffer), use_awaitable);

        // TODO: remove stupid copy
        std::string str(buffer.data(), buffer.size());

        ret = RpcServerManager::getServer().call(header.module_id, header.command, str);
    }
    {
        rpc_response_header header{.version=0,.content_size=ret.size(),.response_code=0};
        co_await async_write(socket, boost::asio::buffer(&header,sizeof(header)), use_awaitable);
        co_await async_write(socket, boost::asio::buffer(ret.c_str(),ret.size()), use_awaitable);
    }

}

awaitable<void> echo(tcp::socket socket)
{
    try
    {
        for (;;)
        {
            co_await echo_once(socket);
        }
    }
    catch (std::exception& e)
    {
        std::printf("echo Exception: %s\n", e.what());
    }
}

awaitable<void> listener()
{
    auto executor = co_await this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), 55555});
    for (;;)
    {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
        co_spawn(executor, echo(std::move(socket)), detached);
    }
}

int main()
{
    try
    {
        boost::asio::io_context io_context(1);

        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto){ io_context.stop(); });

        co_spawn(io_context, listener(), detached);

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::printf("Exception: %s\n", e.what());
    }
}