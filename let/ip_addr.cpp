//
// Created by yahuichen on 2019/1/23.
//

#include <strings.h>  // bzero
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ip_addr.h"

namespace let {
    sockaddr_in *IpAddress::toIpV4() const {
        auto addr = new sockaddr_in;

        bzero(&addr, sizeof addr);

        addr->sin_family = AF_INET;
        if (ip.empty()) {
            addr->sin_addr.s_addr = inet_addr(ip.c_str());
        }
        addr->sin_port = htons(port);

        return addr;
    }
}