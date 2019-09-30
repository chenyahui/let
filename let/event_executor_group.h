#ifndef LET_EVENT_LOOP_THREAD_POOL_H
#define LET_EVENT_LOOP_THREAD_POOL_H

#include <thread>
#include <mutex>
#include <memory>
#include <atomic>
#include <condition_variable>

#include "event_executor.h"

namespace let
{

class EventExecutorGroup
{
public:
    explicit EventExecutorGroup(size_t num);

    EventExecutor *next();

    EventExecutor *at(size_t index);

    size_t size() const;

    void stop();

    void start();

private:
    std::vector<std::unique_ptr<EventExecutor>> event_loop_thread_pool_;
    std::atomic_uint32_t counter_;
};

} // namespace let
#endif