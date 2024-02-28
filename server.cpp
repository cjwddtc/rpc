
#include <cstdio>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include "shared_type.h"

#include "boost/preprocessor.hpp"
#define QWE(a,b,c) int BOOST_PP_CAT(c,_);
#define DECLARE_FUNC(domain,...) class domain{ BOOST_PP_SEQ_FOR_EACH(QWE,_,BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__)))}BOOST_PP_CAT(m_,domain);

class RpcServerManager{
public:
    static RpcServerManager&getManager(){
        static RpcServerManager manager;
        return manager;
    }

#include "api.h"
};


int main(){
    RpcServerManager::getManager().m_a;
}
