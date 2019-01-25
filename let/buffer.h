//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_BUFFER_H
#define LET_BUFFER_H

#include <event2/buffer.h>

namespace let {
    // c++ wrapper of evbuffer
    class Buffer {
    public:
        explicit Buffer(evbuffer *ev_buf) : ev_buf_(ev_buf) {

        }

        size_t length() const {
            return evbuffer_get_length(ev_buf_);
        }

        evbuffer *buffer() const {
            return ev_buf_;
        }

    protected:
        evbuffer *ev_buf_;
    };

    class InBuffer : Buffer {
    public:
        explicit InBuffer(evbuffer *ev_buf) : Buffer(ev_buf) {
        }

        bool drain(size_t datlen) {
            return evbuffer_drain(ev_buf_, datlen) == 0;
        }

        ev_ssize_t search() {
        }
    };

    class OutBuffer : Buffer {
    public:
        explicit OutBuffer(evbuffer *ev_buf) : Buffer(ev_buf) {
        }


        bool addFile(int fd) {
            return evbuffer_add_file(ev_buf_, fd, 0, 0) == 0;
        }

        bool expand(size_t datlen) {
            return evbuffer_expand(ev_buf_, datlen) == 0;
        }

        bool add(const void *data, size_t datlen) {
            return evbuffer_add(ev_buf_, data, datlen) == 0;
        }

        bool addBuffer(Buffer *buffer) {
            return evbuffer_add_buffer(ev_buf_, buffer->buffer()) == 0;
        }
    };


}
#endif //LET_BUFFER_H
