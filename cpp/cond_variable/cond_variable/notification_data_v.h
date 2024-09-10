#pragma once

#include <mutex>
#include <vector>
#include <condition_variable>

// condition_variable wrapper with user data.
// Works like message queue for given type T.
// Internally uses vector to store items, returns vector of items from wait function.
// 
// Several set calls - at least one successful wait call.
//
template<class T>
class notification_data_v
{
public:
    void set(const T& data)
    {
        {
            std::lock_guard<std::mutex> lg(data_mutex);
            user_data.push_back(data);
        }

        cond_variable.notify_one();
    }

    void set(T&& data)
    {
        {
            std::lock_guard<std::mutex> lg(data_mutex);
            user_data.push_back(std::move(data));
        }

        cond_variable.notify_one();
    }

    std::vector<T> wait()
    {
        std::vector<T> result;
        std::unique_lock<std::mutex> ul(data_mutex);        // must be unique_lock and not lock_guard

        cond_variable.wait(
            ul,
            [&result, this]
            {
                if (user_data.empty())
                    return false;

                result = std::move(user_data);
                user_data.clear();
                return true;
            });

        return result;
    }

    // returns: true - variable is set, false - timeout
    bool wait_for(int ms, std::vector<T>& result)
    {
        std::unique_lock<std::mutex> ul(data_mutex);

        return cond_variable.wait_for(
            ul,
            std::chrono::milliseconds(ms),
            [&result, this]
            {
                if (user_data.empty())
                    return false;

                result = std::move(user_data);
                user_data.clear();
                return true;
            });
    }

    void reset()
    {
        std::lock_guard<std::mutex> ul(data_mutex);
        user_data.clear();
    }


private:
    std::mutex data_mutex;
    std::condition_variable cond_variable;
    std::vector<T> user_data;
};

