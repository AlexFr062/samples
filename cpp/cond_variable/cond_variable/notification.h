#pragma once

#include <mutex>
#include <chrono>
#include <condition_variable>

// condition_variable wrapper.
// Works like Windows API PostMessage without parameters.
// Several set calls - the same number of successful wait calls.
//
class notification
{
public:
    void set()
    {
        {
            std::lock_guard<std::mutex> lg(data_mutex);
            ++notification_count;
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
                if (notification_count == 0) return false;      // spurious wakeup
                --notification_count;
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
                if (notification_count == 0) return false;      // spurious wakeup
                --notification_count;
                return true;
            });
    }

    void reset()
    {
        std::lock_guard<std::mutex> ul(data_mutex);
        notification_count = 0;
    }


private:
    int notification_count{};
    std::mutex data_mutex;
    std::condition_variable cond_variable;

};

/*
C++ 20. This class can be implemented using std::counting_semaphore -
according to documentation, it must be faster.

#include <semaphore>
...

class notification
{
public:
    // Post message
    void set()
    {
        sm.release();
    }

    // Wait for message
    void wait()
    {
        sm.acquire();
    }

    // Wait for message with timeout.
    // Returns: true - wait successful, false - timeout.
    bool wait_for(int ms)
    {
        return sm.try_acquire_for(std::chrono::milliseconds(ms));
    }

    void reset()
    {
        while (sm.try_acquire_for(std::chrono::milliseconds(0))) {}
    }

private:
    std::counting_semaphore<> sm{ 0 };
};
*/




*/
