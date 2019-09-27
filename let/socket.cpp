#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <strings.h>
#include <netinet/in.h>

#include "logger.h"
#include "socket.h"

namespace let
{
int create_noblocking_socket()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        return sock;
    }

    if (evutil_make_socket_nonblocking(sock) < 0)
    {
        evutil_closesocket(sock);
        return -1;
    }
    return sock;
}

struct sockaddr_in6 get_local_addr(int sockfd)
{
    struct sockaddr_in6 localaddr;
    bzero(&localaddr, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    if (::getsockname(sockfd, (struct sockaddr *)(&localaddr), &addrlen) < 0)
    {
        LOG_ERROR << "get_local_addr error, fd is" << sockfd;
    }
    return localaddr;
}

struct sockaddr_in6 get_peer_addr(int sockfd)
{
    struct sockaddr_in6 peeraddr;
    bzero(&peeraddr, sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    if (::getpeername(sockfd, (struct sockaddr *)(&peeraddr), &addrlen) < 0)
    {
        LOG_ERROR << "get_peer_addr error, fd is" << sockfd;
    }
    return peeraddr;
}
} // namespace let
