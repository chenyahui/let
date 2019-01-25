//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#ifndef LET_IP_ADDR_H
#define LET_IP_ADDR_H

#include <string>
#include <event2/util.h>

namespace let {
    struct IpAddress {
        std::string ip;
        ev_int16_t port;

        sockaddr_in *toIpV4() const;
    };
}
#endif //LET_IP_ADDR_H
