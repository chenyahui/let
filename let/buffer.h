//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_BUFFER_H
#define LET_BUFFER_H

#include <event2/buffer.h>
#include <string_view>
#include <string>

namespace let
{
// c++ wrapper of evbuffer
class Buffer
{
  public:
    explicit Buffer(evbuffer *ev_buf)
        : ev_buf_(ev_buf)
    {
    }

    Buffer()
        : ev_buf_(evbuffer_new())
    {
    }

    virtual ~Buffer()
    {
        evbuffer_free(ev_buf_);
    }

    size_t length()
    {
        return evbuffer_get_length(ev_buf_);
    }

    bool enableLocking(void *locker)
    {
        return evbuffer_enable_locking(ev_buf_, locker) == 0;
    }

    void lock()
    {
        evbuffer_lock(ev_buf_);
    }

    void unlock()
    {
        evbuffer_unlock(ev_buf_);
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

    evbuffer_ptr search(const std::string &what)
    {
        return search(what.c_str(), what.size());
    }

    std::string_view pullUp(ev_ssize_t size = -1)
    {
        auto buffer = (char *)evbuffer_pullup(ev_buf_, size);
        return std::string_view(buffer, length());
    }

    std::string_view readLine(enum evbuffer_eol_style eol_style = EVBUFFER_EOL_ANY)
    {
        size_t n_read_out = 0;
        char *data = evbuffer_readln(ev_buf_, &n_read_out, eol_style);

        return {data, n_read_out};
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

  protected:
    evbuffer *ev_buf_;
};
} // namespace let
#endif //LET_BUFFER_H
