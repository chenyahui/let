//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_BUFFER_H
#define LET_BUFFER_H

#include <event2/buffer.h>

namespace let {
    class Buffer {
    public:
        Buffer(evbuffer *ev_buf) : ev_buf_(ev_buf) {

        }


    private:
        evbuffer *ev_buf_;
    };
}
#endif //LET_BUFFER_H
