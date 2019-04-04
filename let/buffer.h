//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_BUFFER_H
#define LET_BUFFER_H

#include <event2/buffer.h>
#include <string>

#include "string_view.h"

namespace let
{
// c++ wrapper of evbuffer
class Buffer
{
  public:
    explicit Buffer(evbuffer *ev_buf)
        : ev_buf_(ev_buf),
          free_on_deconstruct_(false)
    {
    }

    Buffer()
        : ev_buf_(evbuffer_new()),
          free_on_deconstruct_(true)
    {
    }

    virtual ~Buffer()
    {
        if (free_on_deconstruct_)
        {
            evbuffer_free(ev_buf_);
        }
    }

    size_t length() const
    {
        return evbuffer_get_length(ev_buf_);
    }

    bool empty() const
    {
        return length() == 0;
    }

    // Read data from an evbuffer, and leave the buffer unchanged.
    ev_ssize_t copyOut(void *data_out, size_t datlen, const struct evbuffer_ptr *pos = nullptr)
    {
        return evbuffer_copyout_from(ev_buf_, pos, data_out, datlen);
    }

    int moveToBuffer(struct evbuffer *src, struct evbuffer *dst,
                     size_t datlen)
    {
        return evbuffer_remove(src, dst, datlen);
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

    StringView pullUp(ev_ssize_t size = -1)
    {
        auto buffer = (char *)evbuffer_pullup(ev_buf_, size);
        return StringView(buffer, length());
    }

    StringView readLine(enum evbuffer_eol_style eol_style = EVBUFFER_EOL_ANY)
    {
        size_t n_read_out = 0;
        char *data = evbuffer_readln(ev_buf_, &n_read_out, eol_style);

        return StringView(data, n_read_out);
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(length());
    }

    std::string retrieveAsString(size_t len)
    {
        std::string result;
        result.resize(len);
        copyOut((void *)result.c_str(), len);
        drain(len);
        return result;
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
    bool free_on_deconstruct_ = false; // 是否负责管理ev_buf的生命周期
};
} // namespace let
#endif //LET_BUFFER_H
