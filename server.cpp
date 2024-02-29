
#include <cstdio>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include "shared_type.h"

#include "server_api.h"

int main(){
    auto handle=start_server();
getchar();
    stop_server(handle);
}
