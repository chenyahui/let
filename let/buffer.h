//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_BUFFER_H
#define LET_BUFFER_H

#include <event2/buffer.h>
#include <boost/utility/string_view.hpp>

namespace let
{
// c++ wrapper of evbuffer
class Buffer
{
  public:
    explicit Buffer(evbuffer *ev_buf) : ev_buf_(ev_buf)
    {
    }

    size_t length() const
    {
        return evbuffer_get_length(ev_buf_);
    }

    evbuffer *buffer() const
    {
        return ev_buf_;
    }

    int moveToBuffer(struct evbuffer *src, struct evbuffer *dst,
                     size_t datlen)
    {
        return evbuffer_remove(src, dst, datlen);
    }

  protected:
    evbuffer *ev_buf_;
};

class InBuffer : Buffer
{
  public:
    explicit InBuffer(evbuffer *ev_buf) : Buffer(ev_buf)
    {
    }

    ev_ssize_t copyOut(void *data_out, size_t datlen, const struct evbuffer_ptr *pos = nullptr)
    {
        return evbuffer_copyout_from(ev_buf_, pos, data_out, datlen);
    }

    bool drain(size_t datlen)
    {
        return evbuffer_drain(ev_buf_, datlen) == 0;
    }

    evbuffer_ptr search(const char *what,
                        size_t len,
                        const struct evbuffer_ptr *start_ptr = nullptr,
                        const struct evbuffer_ptr *end_ptr = nullptr)
    {
        return evbuffer_search_range(ev_buf_, what, len, start_ptr, end_ptr);
    }

    evbuffer_ptr searchEOL()
    {
    }

    unsigned char *pullUp(ev_ssize_t size = -1)
    {
        return evbuffer_pullup(ev_buf_, size);
    }

    boost::string_view readLine(enum evbuffer_eol_style eol_style)
    {
        size_t n_read_out = 0;
        char *data = evbuffer_readln(ev_buf_, &n_read_out, eol_style);

        return {data, n_read_out};
    }
};

class OutBuffer : Buffer
{
  public:
    explicit OutBuffer(evbuffer *ev_buf) : Buffer(ev_buf)
    {
    }

    bool addFile(int fd)
    {
        return evbuffer_add_file(ev_buf_, fd, 0, 0) == 0;
    }

    bool expand(size_t datlen)
    {
        return evbuffer_expand(ev_buf_, datlen) == 0;
    }

    bool add(const void *data, size_t datlen)
    {
        return evbuffer_add(ev_buf_, data, datlen) == 0;
    }

    bool addBuffer(evbuffer *buffer)
    {
        return evbuffer_add_buffer(ev_buf_, buffer) == 0;
    }
};
} // namespace let
#endif //LET_BUFFER_H
