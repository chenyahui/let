#include <event.h>
#include <boost/noncopyable.hpp>

namespace let
{
class EventLoop : boost::noncopyable
{
  public:
    EventLoop()
        : ev_base_(event_base_new())
    {
    }

    ~EventLoop()
    {
        stop();
        event_base_free(ev_base_);
    }

    void loop()
    {
        event_base_loop(ev_base_, EVLOOP_NO_EXIT_ON_EMPTY);
    }

    event_base *getEvBase() const
    {
        return ev_base_;
    }

    void stop()
    {
        event_base_loopbreak(ev_base_);
    }

  private:
    event_base *ev_base_;
};
} // namespace let