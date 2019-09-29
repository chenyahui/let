#include <string.h>
#include <unistd.h>

#include "event_loop.h"
#include "utility.h"
#include "logger.h"

namespace let {
EventLoop::EventLoop()
    : ev_base_(event_base_new()),
      current_loop_thread_id_(std::this_thread::get_id())
{
    if (pipe(wakeup_pipe_) != 0) {
        LOG_FATAL << "failed create pipe";
    }

    wakeup_event_ = event_new(ev_base_,
                              wakeup_pipe_[0],
                              EV_READ | EV_PERSIST,
                              handleWakeUpEvent,
                              this);

    event_add(wakeup_event_, nullptr);
}

EventLoop::~EventLoop()
{
    stop();
    event_base_free(ev_base_);
}

bool EventLoop::isInEventLoop()
{
    return current_loop_thread_id_ == std::this_thread::get_id();
}

event_base *EventLoop::lowLevelEvBase() const
{
    return ev_base_;
}

void EventLoop::execute(Task task, bool queued)
{
    if (isInEventLoop()) {
        task();
    } else if (queued) {
        {
            std::lock_guard<std::mutex> lock_guard(mutex_);
            pendding_jobs_.emplace_back(std::move(task));
        }

        wakeUp();
    } else {
        std::lock_guard<std::mutex> lock_guard(mutex_);
        task();
    }
}

void EventLoop::wakeUp()
{
    // close the read pipe

    uint64_t one = 1;
    ssize_t n = ::write(wakeup_pipe_[1], &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventLoop::handleWakeUpEvent(evutil_socket_t fd, short, void *ctx)
{
    auto self = (EventLoop *)ctx;

    // 将数据读出来，以免可读事件一直通知
    uint64_t one;
    while (::read(fd, &one, sizeof one) > 0) {
    }

    // 先把事件都取出来, 减少锁空间
    std::vector<Task> tasks;
    {
        // add lock
        std::lock_guard<std::mutex> lock_guard(self->mutex_);
        tasks.swap(self->pendding_jobs_);
    }

    for (auto &task : tasks) {
        task();
    }
}

struct TimerInfo {
    TimerInfo(struct event *ev, EventLoop *loop)
        : timer_event(ev), loop(loop)
    {
    }
    struct event *timer_event = nullptr;
    EventLoop *loop = nullptr;
    bool is_presist = false;
};

// ms
TimerId EventLoop::runEvery(long milliseconds, Task task)
{
    return registerTimer(milliseconds, std::move(task), true);
}

// ms
TimerId EventLoop::runAfter(long milliseconds, Task task)
{
    return registerTimer(milliseconds, std::move(task), false);
}

TimerId EventLoop::registerTimer(long milliseconds, Task task, bool run_every)
{
    auto timer_ev = new event;
    memset(timer_ev, 0, sizeof(struct event));
    auto timer_info = new TimerInfo(timer_ev, this);
    timer_info->is_presist = run_every;
    TimerId timer_id(timer_info);
    execute([job = std::move(task),
             this,
             run_every,
             timer_ev,
             timer_info,
             milliseconds]() {
        timer_callbacks_[timer_ev] = std::move(job);

        short flags = static_cast<short>(run_every ? EV_PERSIST : 0);

        event_assign(timer_ev, ev_base_, -1, flags, onTimer, timer_info);

        struct timeval time_val = milliseconds_to_timeval(milliseconds);

        evtimer_add(timer_ev, &time_val);
    });

    return timer_id;
}

void EventLoop::onTimer(int, short, void *ctx)
{
    auto timer_info = (TimerInfo *)ctx;
    if (timer_info && timer_info->timer_event) {
        auto self = timer_info->loop;
        self->timer_callbacks_[timer_info->timer_event]();

        // 如果是一次性的, 则释放空间
        if (!timer_info->is_presist) {
            // cancelTimer();
        }
    }
}

void EventLoop::cancelTimer(TimerId &timer_id)
{
    if (timer_id.info && timer_id.info->timer_event) {
        execute([this, &timer_id]() {
            auto timer_ev = timer_id.info->timer_event;
            if (timer_ev && timer_callbacks_.find(timer_ev) != timer_callbacks_.end()) {
                evtimer_del(timer_ev);
                timer_callbacks_.erase(timer_ev);
                event_free(timer_ev);

                // 释放空间
                delete timer_id.info;
                timer_id.info = nullptr;
            }
        });
    }
}

void EventLoop::stop()
{
    event_base_loopbreak(ev_base_);
}

void EventLoop::startLoop()
{
    LOG_VERBOSE << "start loop";
    // 没有事件时也不退出，一直循环
    event_base_loop(ev_base_, EVLOOP_NO_EXIT_ON_EMPTY);
}
}  // namespace let
