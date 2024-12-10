#pragma once

#include <semaphore>
#include <chrono>

// notification implemented with counting_semaphore.
// Requires C++20 standard.
//
// Simpler code, better performance - according to counting_semaphore documentation.
// 
// Note: For more advanced scenario we still  need condition_variable.
//       Semaprore message doesn't contain any data.
//
class notification_20
{
public:
    // Post message. 
    // Better to call post, set is for compatibility with notification class.
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
    // ms - number of milliseconds to wait.
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
