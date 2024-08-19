#pragma once


#ifndef _WIN32

#include <thread>
#include <vector>

#include "inc.h"
#include "udp_def.h"

class udp_socket;

#define INVALID_SOCKET -1

// File: udp_socket_impl_l.h
// UDP socket implementation for Linux
class udp_socket_impl
{
public:
    udp_socket_impl(udp_socket* s);
    ~udp_socket_impl();

    void create_socket(ip_version ipv, int max_length);
    void close();
    void connect(const sockaddr* address);
    unsigned short get_local_port();
    void start_receive_connected_client();
    void send(const char* data, int length);


private:
    void release();
    void read_thread_connected_client_function();
    void report_api_error(const char* function, int errno_value);


    udp_socket* owner;
    ip_version version{};
    int max_packet_length{};
    int sc = INVALID_SOCKET;
    std::thread read_thread_connected_client;
    std::vector<char> receive_buffer;
    int pipefd[2];						// control pipe
};

#endif
