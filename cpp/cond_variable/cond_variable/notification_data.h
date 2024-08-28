#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>

// condition_variable wrapper with user data.
// Works like message queue for given type T.
//
template<class T>
class notification_data
{
public:
    void set(const T& data)
    {
        {
            std::lock_guard<std::mutex> lg(data_mutex);
            user_data.push(data);
        }

        cond_variable.notify_one();
    }

    void set(T&& data)
    {
        {
            std::lock_guard<std::mutex> lg(data_mutex);
            user_data.push(std::move(data));
        }

        cond_variable.notify_one();
    }

    T wait()
    {
        T result;
        std::unique_lock<std::mutex> ul(data_mutex);        // must be unique_lock and not lock_guard

        cond_variable.wait(
            ul,
            [&result, this]
        {
            if (user_data.empty())
                return false;

            result = std::move(user_data.front());
            user_data.pop();
            return true;
        });

        return result;
    }

    // returns: true - variable is set, false - timeout
    bool wait_for(int ms, T& result)
    {
        std::unique_lock<std::mutex> ul(data_mutex);

        return cond_variable.wait_for(
            ul,
            std::chrono::milliseconds(ms),
            [&result, this]
        {
            if (user_data.empty())
                return false;

            result = std::move(user_data.front());
            user_data.pop();
            return true;
        });
    }

    void reset()
    {
        std::lock_guard<std::mutex> ul(data_mutex);

        while (!user_data.empty())
        {
            user_data.pop();
        }
    }


private:
    std::mutex data_mutex;
    std::condition_variable cond_variable;
    std::queue<T> user_data;
};
