//
// Created by yahuichen on 2019/1/23.
//

#include <strings.h>  // bzero
#include <endian.h>

#include "ip_addr.h"

namespace let {
    sockaddr_in *IpAddress::toIpV4() const{
//        sockaddr_in *addr;
//        bzero(&addr, sizeof addr);
//        addr->sin_family = AF_INET;
//        addr->sin_addr.s_addr = htobe32(ip);
//        addr->sin_port = htobe16(port);
    }
}