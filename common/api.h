
#ifndef MAIN_API_H
#define MAIN_API_H
#ifdef __cplusplus
extern "C" {
#endif
enum dcmmReturn_t {
    DCMM_SUCCESS,
    DCMM_INVALID_MODULE,
    DCMM_INVALID_COMMAND,
    DCMM_INVALID_RESPONSE,
    DCMM_TEST,
};

struct DcmmHandle;

struct test_2 {
    int a;
    double b;
};
dcmmReturn_t test1(DcmmHandle *handle,char, int);
dcmmReturn_t test2(DcmmHandle *handle,test_2 *);
dcmmReturn_t test3(DcmmHandle *handle,const char *);
dcmmReturn_t createHandle(DcmmHandle **handle);
dcmmReturn_t destroyHandle(DcmmHandle **handle);
#ifdef __cplusplus
}
#endif
#endif //MAIN_API_H
