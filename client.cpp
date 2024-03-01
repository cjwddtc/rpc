#include "common/api.h"
#include "stdio.h"
int asd(){
        DcmmHandle *handle;
    createHandle(&handle);

        test1(handle,'c',5);
        test_2 t{5,5.5};
        test2(handle,&t);
        printf("%d,%f\n",t.a,t.b);

        auto ret=test3(handle,"qwe");
        printf("asd,%d\n",ret);

    destroyHandle(&handle);
        return 0;
}
#include <thread>
#include <vector>
int main(){
    std::vector<std::thread> thr;
    thr.emplace_back(asd);
    sleep(1);
    thr.emplace_back(asd);
    sleep(1);
    thr.emplace_back(asd);
    for(auto &a:thr){
        a.join();
    }
}