#ifndef MAIN_SERVER_API_H
#define MAIN_SERVER_API_H

#ifdef __cplusplus
extern "C" {
#endif
void *start_server();

void stop_server(void *ptr);
#ifdef __cplusplus
}
#endif
#endif //MAIN_SERVER_API_H
