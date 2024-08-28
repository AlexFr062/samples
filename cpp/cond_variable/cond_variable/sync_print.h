#pragma once

#include <mutex>
#include <sstream>
#include <iostream>

// Synchronized print to stdout.

extern std::mutex sync_mutex;		// Caller must create sync_mutex instance in its code

template<typename... Args>
void sync_print_impl([[maybe_unused]] std::ostringstream& str, Args&&... args)
{
    // maybe_unused - when sync_print is called without parameters, this prevents unused variable warning

    //((str << std::forward<Args>(args) << " "), ...);  // both versions working
    (..., (str << std::forward<Args>(args) << " "));    // ( ... op pack ) : ((( pack1 op pack2 ) op pack3 ) ... op packN )  seems to be better for this case
}

template <typename... Args>
void sync_print(Args&&... args)
{
    std::ostringstream s;
    sync_print_impl(s, std::forward<Args>(args)...);

    {
        std::lock_guard<std::mutex> lg(sync_mutex);
        std::cout << s.str() << std::endl;
    }
}
