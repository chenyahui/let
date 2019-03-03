#ifndef LET_EVENT_LOOP_H
#define LET_EVENT_LOOP_H

#include <event.h>
#include <map>
#include <memory>

#include "noncopyable.h"
#include "callback.h"

namespace let
{
class EventLoop : NonCopyAble
{
  public:
    EventLoop();

    ~EventLoop();

    void loop();

    event_base *getEvBase() const;

    void stop();

    TimerId runEvery(long interval, const TimerCallback &cb);

    TimerId runAfter(long interval, const TimerCallback &cb);

    void cancelTimer(TimerId);

    void onSingal(short sig, const SingalCallback&);

  private:
    static void timerWrapper(int, short, void *);

    static void singalWrapper(int, short, void *);

    TimerId runTimer(long interval, const TimerCallback &cb, bool run_every);

    event_base *ev_base_;

    // std::map<TimerId, std::unique_ptr<TimerCallback>> timer_map_;
};
} // namespace let

#endif