//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_SOCKET_H
#define LET_SOCKET_H

#include <string>
#include <netinet/in.h>

namespace let {
    std::string sockaddr_to_ip_port(const struct sockaddr *addr);

}
#endif //LET_SOCKET_H
