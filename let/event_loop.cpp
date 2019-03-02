#include <cstring>

#include "event_loop.h"
#include "util.h"
#include "logger.h"

namespace let
{
EventLoop::EventLoop()
    : ev_base_(event_base_new())
{
}

EventLoop::~EventLoop()
{

    stop();
    event_base_free(ev_base_);
}

void EventLoop::loop()
{
    LOG_DEBUG << "start event loop";
    event_base_loop(ev_base_, EVLOOP_NO_EXIT_ON_EMPTY);
}

event_base *EventLoop::getEvBase() const
{
    return ev_base_;
}

void EventLoop::stop()
{
    LOG_DEBUG << "end event loop";
    event_base_loopbreak(ev_base_);
}

TimerId EventLoop::runEvery(long interval, const TimerCallback &cb)
{
    return runTimer(interval, cb, true);
}

TimerId EventLoop::runAfter(long interval, const TimerCallback &cb)
{
    return runTimer(interval, cb, false);
}

TimerId EventLoop::runTimer(long interval, const TimerCallback &cb, bool run_every)
{
    // todo TimerId如果用户不释放，则会有内存泄漏的风险
    auto timer_ev = new event;
    memset(timer_ev, 0, sizeof(struct event));

    auto timeFunc = new TimerCallback([&]() {
        cb();
    });

    short flags = static_cast<short>(run_every ? EV_PERSIST : 0);

    event_assign(timer_ev, ev_base_, -1,
                 flags,
                 timerWrapper,
                 timeFunc);

    struct timeval time_val = timestamp_to_timeval(interval);

    evtimer_add(timer_ev, &time_val);

    return timer_ev;
}

void cancelTimer(TimerId ev)
{
    auto func = (TimerCallback *)event_get_callback_arg(ev);
    evtimer_del(ev);
    event_free(ev);
}

void EventLoop::timerWrapper(int, short event, void *args)
{
    auto func = (TimerCallback *)(args);
    (*func)();

    if (!event)
    {
        delete func;
    }
}

} // namespace let