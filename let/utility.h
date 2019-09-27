//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_UTILITY_H
#define LET_UTILITY_H

#include <string>
#include <sys/time.h>
#include <cstdint>
#include <utility>

namespace let
{
struct timeval milliseconds_to_timeval(long interval);

int64_t get_monotonic_milliseconds();

time_t get_now_time_t();

std::string format_time(time_t, const std::string &format);

void sleep_ms(int64_t milliseconds);

#include <utility>

template <typename T, typename F>
class capture_impl
{
    T x;
    F f;
public:
    capture_impl( T && x, F && f )
        : x{std::forward<T>(x)}, f{std::forward<F>(f)}
    {}

    template <typename ...Ts> auto operator()( Ts&&...args )
        -> decltype(f( x, std::forward<Ts>(args)... ))
    {
        return f( x, std::forward<Ts>(args)... );
    }

    template <typename ...Ts> auto operator()( Ts&&...args ) const
        -> decltype(f( x, std::forward<Ts>(args)... ))
    {
        return f( x, std::forward<Ts>(args)... );
    }
};

template <typename T, typename F>
capture_impl<T,F> capture( T && x, F && f )
{
    return capture_impl<T,F>(
        std::forward<T>(x), std::forward<F>(f) );
}
} // namespace let
#endif //LET_UTILITY_H
