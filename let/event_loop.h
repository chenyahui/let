#ifndef LET_EVENT_LOOP_H
#define LET_EVENT_LOOP_H

#include <queue>
#include <functional>
#include <mutex>
#include <map>
#include <thread>
#include <event.h>

#include "noncopyable.h"

namespace let
{
class EventLoop;
struct TimerInfo;

class TimerId
{
public:
    friend class EventLoop;

    TimerId(TimerInfo *timer_info) : info(timer_info) {}

private:
    TimerInfo *info = nullptr;
};

class EventLoop : NonCopyAble
{
public:
    EventLoop();

    ~EventLoop();

    using Task = std::function<void()>;

    event_base *lowLevelEvBase() const;

    // ms
    TimerId runEvery(long milliseconds, Task task);

    // ms
    TimerId runAfter(long milliseconds, Task task);

    void cancelTimer(TimerId &timer_id);

    void execute(Task, bool queued = true);

    // 是否处于event_loop的线程中
    bool isInEventLoop();

    void wakeUp();

    void stop();

    void startLoop();

private:
    static void handleWakeUpEvent(evutil_socket_t, short, void *);

    TimerId registerTimer(long milliseconds, Task task, bool run_every);

    static void onTimer(int, short, void *);

private:
    std::mutex mutex_;
    std::vector<Task> pendding_jobs_;
    std::map<event *, Task> timer_callbacks_;

    event_base *ev_base_ = nullptr;

    event *wakeup_event_ = nullptr;
    int wakeup_pipe_[2];

    std::thread::id current_loop_thread_id_;
};
} // namespace let

#endif