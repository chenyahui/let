#ifndef LET_BUFFER_H
#define LET_BUFFER_H
#include <event2/buffer.h>

namespace let
{
using BufferPos = struct evbuffer_ptr;

class Buffer
{
public:
    explicit Buffer(evbuffer *ev_buf)
        : ev_buf_(ev_buf),
          auto_free_(false)
    {
    }

    Buffer(bool create_buf = false)
    {
        if (create_buf)
        {
            ev_buf_ = evbuffer_new();
            auto_free_ = true;
        }
    }

    size_t size() const
    {
        return evbuffer_get_length(ev_buf_);
    }

    bool empty() const
    {
        return size() == 0;
    }

    void setBuffer(evbuffer *buffer)
    {
        ev_buf_ = buffer;
    }

    evbuffer *buffer() const
    {
        return ev_buf_;
    }

    ev_ssize_t copy(void *data_out, size_t data_len, BufferPos *pos = nullptr)
    {
        return evbuffer_copyout_from(ev_buf_, pos, data_out, data_len);
    }

    bool consume(size_t len)
    {
        return evbuffer_drain(ev_buf_, len) == 0;
    }

    char *pullUp(size_t len = -1)
    {
        auto buffer = (char *)evbuffer_pullup(ev_buf_, len);
        return buffer;
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(size());
    }

    std::string retrieveAsString(size_t len)
    {
        std::string result;
        result.resize(len);

        copy((void *)result.c_str(), len);
        consume(len);

        return result;
    }

    bool addFile(int fd)
    {
        return evbuffer_add_file(ev_buf_, fd, 0, 0) == 0;
    }

    bool add(const void *data, size_t datlen)
    {
        return evbuffer_add(ev_buf_, data, datlen) == 0;
    }

    bool addBuffer(evbuffer *buffer)
    {
        return evbuffer_add_buffer(ev_buf_, buffer) == 0;
    }

    BufferPos search(const char *what,
                     size_t len,
                     BufferPos *start_ptr = nullptr,
                     BufferPos *end_ptr = nullptr)
    {
        return evbuffer_search_range(ev_buf_, what, len, start_ptr, end_ptr);
    }

    BufferPos search(const std::string &what,
                     BufferPos *start_ptr = nullptr,
                     BufferPos *end_ptr = nullptr)
    {
        return search(what.c_str(), what.size());
    }

protected:
    evbuffer *ev_buf_ = nullptr;
    bool auto_free_ = false;
};

} // namespace let

#endif