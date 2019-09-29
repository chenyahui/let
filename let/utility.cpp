#include <chrono>
#include <thread>

#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "logger.h"
#include "utility.h"

namespace let {
std::string format_time(time_t rawtime, const std::string &format)
{
    char buffer[80];
    struct tm *timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), format.c_str(), timeinfo);
    return {buffer};
}

struct timeval milliseconds_to_timeval(long interval)
{
    struct timeval result {
    };
    result.tv_sec = interval / 100;
    result.tv_usec = (interval % 1000) * 1000;
    return result;
}

int64_t get_monotonic_milliseconds()
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t) == 0) {
        return (int64_t)t.tv_sec * 1000 + (int64_t)t.tv_nsec / (1000 * 1000);
    } else {
        struct timeval t;
        memset(&t, 0, sizeof(t));
        gettimeofday(&t, NULL);
        return (int64_t)t.tv_sec * 1000 + (int64_t)t.tv_usec / 1000;
    }

    return -1;
}

time_t get_now_time_t()
{
    time_t raw_time;
    time(&raw_time);
    return raw_time;
}

void sleep_ms(int64_t milliseconds)
{
    std::chrono::milliseconds timespan(milliseconds);  // or whatever

    std::this_thread::sleep_for(timespan);
}

#if defined(OS_MACOSX)
typedef sig_t SignalHandler;
#else
typedef sighandler_t SignalHandler;
#endif

// from brpc
static volatile bool s_signal_quit = false;
static SignalHandler s_prev_sigint_handler = nullptr;
static SignalHandler s_prev_sigterm_handler = nullptr;

static void quit_handler(int signo)
{
    s_signal_quit = true;
    if (SIGINT == signo && s_prev_sigint_handler) {
        s_prev_sigint_handler(signo);
    }
    if (SIGTERM == signo && s_prev_sigterm_handler) {
        s_prev_sigterm_handler(signo);
    }
}

static pthread_once_t register_quit_signal_once = PTHREAD_ONCE_INIT;

// from brpc
static void register_quit_signal_or_die()
{
    // Not thread-safe.
    SignalHandler prev = signal(SIGINT, quit_handler);
    if (prev != SIG_DFL &&
        prev != SIG_IGN) {  // shell may install SIGINT of background jobs with SIG_IGN
        if (prev == SIG_ERR) {
            LOG(ERROR) << "Fail to register SIGINT, abort";
            abort();
        } else {
            s_prev_sigint_handler = prev;
            LOG(WARN) << "SIGINT was installed with " << prev;
        }
    }

    prev = signal(SIGTERM, quit_handler);
    if (prev != SIG_DFL && prev != SIG_IGN) {  // shell may install SIGTERM of
                                               // background jobs with SIG_IGN
        if (prev == SIG_ERR) {
            LOG(ERROR) << "Fail to register SIGTERM, abort";
            abort();
        } else {
            s_prev_sigterm_handler = prev;
            LOG_WARN << "SIGTERM was installed with " << prev;
        }
    }
}

bool is_ask_to_quit()
{
    pthread_once(&register_quit_signal_once, register_quit_signal_or_die);
    return s_signal_quit;
}

void loop_util_ask_to_quit()
{
    while (!is_ask_to_quit()) {
        sleep_ms(1000);
    }
}

}  // namespace let