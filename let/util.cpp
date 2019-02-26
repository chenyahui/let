//
// Created by yahuichen on 2019/1/23.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h> // bzero
#include <arpa/inet.h>

#include "logger.h"
#include "util.h"

namespace let
{
std::string format_time(time_t rawtime, const std::string &format)
{
    char buffer[80];
    struct tm *timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), format.c_str(), timeinfo);
    return {buffer};
}

std::string format_now_time(const std::string &format)
{
    time_t rawtime;
    time(&rawtime);

    return format_time(rawtime, format);
}

struct sockaddr_in get_local_addr(int sockfd)
{
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    if (::getsockname(sockfd, (struct sockaddr *)(&localaddr), &addrlen) < 0)
    {
        LOG_ERROR << "get_local_addr error, fd is" << sockfd;
    }
    return localaddr;
}
struct sockaddr_in get_peer_addr(int sockfd)
{
    struct sockaddr_in peeraddr;
    bzero(&peeraddr, sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    if (::getpeername(sockfd, (struct sockaddr *)(&peeraddr), &addrlen) < 0)
    {
        LOG_ERROR << "get_peer_addr error, fd is" << sockfd;
    }
    return peeraddr;
}
} // namespace let