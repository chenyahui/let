//
// Created by yahuichen on 2019/1/23.
//

#include <strings.h> // bzero
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

#include <event2/util.h>
#include "logger.h"
#include "utility.h"

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

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

struct timeval timestamp_to_timeval(long interval)
{
    struct timeval time_val
    {
    };
    time_val.tv_sec = interval / 1000;
    time_val.tv_usec = (interval % 1000) * 1000;
    return time_val;
}

int64_t get_monotonic_timestamp()
{
#if HAVE_CLOCK_GETTIME
    struct timespec tp;
    if (clock_gettime(CLOCK_MONOTONIC, &tp) == 0)
    {
        return (int64_t)(1000000000LL * tp.tv_sec + tp.tv_nsec);
    }
#elif defined(__MACH__)
    clock_serv_t cclock;
    mach_timespec_t mts;
    if (host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock) == KERN_SUCCESS)
    {
        if (clock_get_time(cclock, &mts) == KERN_SUCCESS)
        {
            mach_port_deallocate(mach_task_self(), cclock);
            return (int64_t)(1000000000LL * mts.tv_sec + mts.tv_nsec);
        }
        mach_port_deallocate(mach_task_self(), cclock);
    }
#endif /* HAVE_CLOCK_GETTIME */

    /* Oh my god, use wall clock instead. */
    return get_wall_clock_timestamp();
}

int64_t get_wall_clock_timestamp()
{
    time_t t;
    struct timeval tv;

#if HAVE_CLOCK_GETTIME
    struct timespec tp;
    if (clock_gettime(CLOCK_REALTIME, &tp) == 0)
    {
        return 1000000000LL * tp.tv_sec + tp.tv_nsec;
    }
#elif defined(__MACH__)
    clock_serv_t cclock;
    if (host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock) == KERN_SUCCESS)
    {
        mach_timespec_t mts;
        if (clock_get_time(cclock, &mts) == 0)
        {
            mach_port_deallocate(mach_task_self(), cclock);
            return (int64_t)(1000000000LL * mts.tv_sec + mts.tv_nsec);
        }
        mach_port_deallocate(mach_task_self(), cclock);
    }
#endif

    if (gettimeofday(&tv, NULL) == 0)
    {
        return (int64_t)(1000000000LL * tv.tv_sec + 1000LL * tv.tv_usec);
    }

    if ((t = time(NULL)) >= 0)
    {
        return (int64_t)(1000000000LL * t);
    }

    return -1;
}

void set_tcp_nodelay(int fd, bool no_delay)
{
    int enable = no_delay;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable));
}

} // namespace let