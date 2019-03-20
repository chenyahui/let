//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_UTILITY_H
#define LET_UTILITY_H

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

void set_tcp_nodelay(int fd, bool no_delay);

struct timeval timestamp_to_timeval(long interval);
int64_t get_monotonic_timestamp();
int64_t get_wall_clock_timestamp();

} // namespace let
#endif //LET_UTILITY_H
