//
// Created by yahuichen on 2019/1/23.
//

#include "tcp_conn.h"

namespace let {
    TcpConnection::TcpConnection(bufferevent *buf_ev)
            : buf_ev_(buf_ev) {
    }
}