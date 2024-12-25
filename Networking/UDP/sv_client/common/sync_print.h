#pragma once

#include <mutex>
#include <sstream>
#include <iostream>

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


// Note: In C++20 it is possible to use osyncstream class for synchronized outpur, so sync_print is not needed:
//
// #include <syncstream>
// ...
// std::osyncstream(std::cout) << "This output is synchronized" << std::endl;
