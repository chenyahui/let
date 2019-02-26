//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_UTIL_H
#define LET_UTIL_H

#include <string>
namespace let{
    std::string format_time(time_t, const std::string& format);
    std::string format_now_time(const std::string& format);
    struct sockaddr_in get_local_addr(int sockfd);
    struct sockaddr_in get_peer_addr(int sockfd);
}
#endif //LET_UTIL_H
