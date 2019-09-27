#ifndef LET_SOCKET_H
#define LET_SOCKET_H
#include <event2/util.h>

namespace let
{
int create_noblocking_socket();


// 用sockaddr_in6解析可以兼容sockaddr_in
struct sockaddr_in6 get_local_addr(int sockfd);
struct sockaddr_in6 get_peer_addr(int sockfd);
}
#endif