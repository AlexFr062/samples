#pragma once

#include <mutex>
#include <chrono>
#include <condition_variable>

// condition_variable wrapper.
// Works like Windows API auto-reset event.
// Several set calls - at least one successful wait call.
//
class notification_single
{
public:
    void set()
    {
        {
            std::lock_guard<std::mutex> lg(data_mutex);
            data = true;
        }

        cond_variable.notify_one();
    }

    void wait()
    {
        std::unique_lock<std::mutex> ul(data_mutex);        // must be unique_lock and not lock_guard

        cond_variable.wait(
            ul,
            [this]
            {
                if (! data) return false;       // spurious wakeup
                data = false;
                return true;
            });
    }

    // Returns: true - variable is set, false - timeout.
    // ms - number of milliseconds to wait.
    bool wait_for(int ms)
    {
        std::unique_lock<std::mutex> ul(data_mutex);

        return cond_variable.wait_for(
            ul,
            std::chrono::milliseconds(ms),
            [this]
            {
                if (!data) return false;       // spurious wakeup
                data = false;
                return true;
            });
    }

    void reset()
    {
        std::lock_guard<std::mutex> ul(data_mutex);
        data = false;
    }


private:
    bool data{};
    std::mutex data_mutex;
    std::condition_variable cond_variable;
};
