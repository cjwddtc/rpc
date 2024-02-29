#include "server.h"

#include <thread>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <thread>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/asio/write.hpp>
#include <cstdio>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "server.h"
using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;
class RpcNetworkManager {
    std::thread m_thr;
    boost::asio::io_context m_io_context;

    awaitable<void> echo_once(tcp::socket &socket) {
        uint64_t ret;
        std::vector<char> buffer;
        {
            rpc_request_header header;

            co_await socket.async_receive(boost::asio::buffer(&header, sizeof(header)), use_awaitable);
            buffer.resize(header.content_size);
            co_await socket.async_receive(boost::asio::buffer(buffer), use_awaitable);

            ret=RpcApiManager::call(header.module_id,header.command,buffer.data());

        }
        {
            rpc_response_header header{.version=0,.response_code=ret, .content_size=buffer.size()};
            co_await async_write(socket, boost::asio::buffer(&header, sizeof(header)), use_awaitable);
            co_await async_write(socket, boost::asio::buffer(buffer), use_awaitable);
            test_2 *p=(test_2*)buffer.data();
            printf("%d,%f\n",p->a,p->b);
        }

    }

    awaitable<void> echo(tcp::socket socket) {
        try {
            for (;;) {
                co_await echo_once(socket);
            }
        }
        catch (std::exception &e) {
            std::printf("echo Exception: %s\n", e.what());
        }
    }

    awaitable<void> listener() {
        auto executor = co_await this_coro::executor;
        tcp::acceptor acceptor(executor, {tcp::v4(), 32001});
        for (;;) {
            tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
            co_spawn(executor, echo(std::move(socket)), detached);
        }
    }
public:
    RpcNetworkManager() : m_io_context{1} {
        co_spawn(m_io_context, listener(), detached);
        std::thread{[this](){
            m_io_context.run();
        }}.swap(m_thr);
    }

    ~RpcNetworkManager() {
        m_io_context.stop();
        m_thr.join();
    }
};

void *start_server(){
    return new RpcNetworkManager{};
}

void stop_server(void *ptr){
    delete reinterpret_cast<RpcNetworkManager*>(ptr);
}

dcmmReturn_t RpcApiManager::a::test1(char ch, int a) {
    printf("test1,%c,%d\n",ch,a);
    return DCMM_SUCCESS;
}


dcmmReturn_t RpcApiManager::a::test2(test_2 *data) {

    printf("test2,%f,%d\n", data->b,data->a);
    data->a=4;
    return DCMM_SUCCESS;
}

dcmmReturn_t RpcApiManager::b::test3(const char *name) {
    printf("test3:%s\n",name);
    return DCMM_TEST;
}