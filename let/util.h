//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_UTIL_H
#define LET_UTIL_H

#include <string>

#include "ip_addr.h"

namespace let
{
std::string format_time(time_t, const std::string &format);
std::string format_now_time(const std::string &format);

// 用sockaddr_in6解析可以兼容sockaddr_in
struct sockaddr_in6 get_local_addr(int sockfd);
struct sockaddr_in6 get_peer_addr(int sockfd);

int create_noblocking_socket();

struct timeval timestamp_to_timeval(long interval);
} // namespace let
#endif //LET_UTIL_H
