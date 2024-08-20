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

    // Client
    void connect(const sockaddr* address);
    void start_receive_connected_client();
    void send(const uint8_t* data, int length);

    // Server
    void bind(unsigned short port);
    void start_receive_server();
    void send(const uint8_t* data, int size, const sockaddr* addr, int addr_len);


    void create_socket(ip_version ipv, int max_length);
    void close();
    unsigned short get_local_port();

private:
    void release();
    void read_thread_connected_client_function();
    void server_fn();
    void report_api_error(const char* function, int errno_value);


    udp_socket* owner;
    int sc = INVALID_SOCKET;
    ip_version version{};
    int address_len{};                              // sizeof(sockaddr_in) or sizeof(sockaddr_in6), depending on version
    sockaddr_storage connected_address{};           // connected address (client), bound address (server)
    int max_packet_length{};
    std::thread thread_client;
    std::thread thread_server;
    std::vector<uint8_t> receive_buffer;
    int pipefd[2];						// control pipe


};

#endif
