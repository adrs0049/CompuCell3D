#ifndef LOCK_HELPER_H
#define LOCK_HELPER_H

#include <mutex>

template <typename Lockable, typename Function>
inline auto with_lock(Lockable& lockable, Function fn) -> decltype(fn())
{
    std::lock_guard<Lockable> local_lock(lockable);
    return fn();
}

#endif
