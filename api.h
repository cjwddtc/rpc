#ifndef MAIN_API_H
#define MAIN_API_H
#include <string>
#include <vector>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
double test1(char ch,std::string str);
std::string test2(std::vector<int> arr);
long test3(std::vector<std::string> arr);
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
#ifdef DECLARE_FUNC
DECLARE_FUNC(a,test1,test2)
DECLARE_FUNC(b,test3)
#endif
#endif //MAIN_API_H
