#include <iostream>
#include <vector>
#include <thread>

#include "sync_print.h"
#include "notification.h"
#include "notification_single.h"
#include "notification_data.h"
#include "notification_data_v.h"

using namespace std::chrono_literals;           // for this_thread::sleep_for

// Different condition_variable wrappers.
// C++ 17.

void test_notification();
void test_notification_single();
void test_notification_data();
void test_notification_data_v();

std::mutex sync_mutex;          // for sync_print

int main()
{
    test_notification();
    test_notification_single();
    test_notification_data();
    test_notification_data_v();

    return 0;
}

// Expected output:
// wait succeded
// wait succeded
// wait succeded
// wait succeded
// wait succeded
//
void test_notification()
{
    sync_print();
    sync_print(__FUNCTION__);

    notification nt;
    const int count1 = 3;
    const int count2 = 2;

    for (int i = 0; i < count1; ++i)
    {
        nt.set();
    }

    std::thread t([=, &nt]() 
    {
        for(int i = 0; i < count1 + count2; ++i)
        {
            nt.wait();
            sync_print("wait succeded");
        }
    });

    for (int i = 0; i < count2; ++i)
    {
        nt.set();
    }

    t.join();
}


// Expected output:
// wait succeded
// wait failed
//
void test_notification_single()
{
    sync_print();
    sync_print(__FUNCTION__);

    notification_single nt;

    nt.set();
    nt.set();
    nt.set();

    std::thread t([=, &nt]() 
    {
        nt.wait();
        sync_print("wait succeded");

        if (nt.wait_for(1000))
        {
            sync_print("wait succeded");
        }
        else
        {
            sync_print("wait failed");
        }
    });

    t.join();
}

enum command
{
    execute,
    stop
};

struct message
{
    std::string str;
    command cmd;
};

// Expected output:
// string 1
// string 2
// string 3
// string 4
// string 5
//
void test_notification_data()
{
    sync_print();
    sync_print(__FUNCTION__);

    notification_data<message> nt;

    const int count1 = 3;
    const int count2 = 2;
    int message_id{};

    for (int i = 0; i < count1; ++i)
    {
        nt.set(message{ std::string("string ") + std::to_string(++message_id), command::execute });
    }

    std::thread t([=, &nt]() 
    {
        for(;;)
        {
            auto msg = nt.wait();

            if (msg.cmd == command::stop) break;

            sync_print(msg.str);
        }
    });

    for (int i = 0; i < count2; ++i)
    {
        nt.set(message{ std::string("string ") + std::to_string(++message_id), command::execute });
    }

    nt.set(message{ "", command::stop });

    t.join();
}


// By changing the notification container type, wait return type and wait conditions,
// it is possible to make new notification classes for specific purposes.
// For example, notification_data<T> uses std::queue internally.
// For better perfomance I want to use std::vector. The whole vector is returned
// by single wait, so N set calls may result from 1 to N wait calls/
// 
// So, let's write new notification_data_v class, which keeps the data in vector,
// and returns all available data from wait function at once.
// 
// Output:
//
// send string 1
// send string 2
// send string 3
// wait succeeded. count =  3
//    string 1
//    string 2
//    string 3
// send string  4
// send string  5
// wait succeeded. count =  1
// send stop request
//    string 4
// wait succeeded. count =  2
//    string 5
// 
// Exact order of operations can vary.
//
void test_notification_data_v()
{
    sync_print();
    sync_print(__FUNCTION__);


    notification_data_v<message> nt;

    const int count1 = 3;
    const int count2 = 2;
    int message_id{};


    for (int i = 0; i < count1; ++i)
    {
        sync_print("send string", ++message_id);

        nt.set(message{ std::string("string ") + std::to_string(message_id), command::execute });
    }

    std::thread t([=, &nt]()
    {
        for (;;)
        {
            auto v = nt.wait();
            sync_print("wait succeeded. count = ", v.size());

            for (const auto& msg : v)
            {
                if (msg.cmd == command::stop) return;
                sync_print("  ", msg.str);
            }
        }
    });

    std::this_thread::sleep_for(100ms);      // to make output more interesting

    for (int i = 0; i < count2; ++i)
    {
        sync_print("send string ", ++message_id);

        nt.set(message{ std::string("string ") + std::to_string(message_id), command::execute });
    }

    sync_print("send stop request");

    nt.set(message{ "", command::stop });

    t.join();
}