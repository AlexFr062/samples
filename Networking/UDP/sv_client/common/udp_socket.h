#pragma once

#include "inc.h"
#include "udp_def.h"

class udp_socket_impl;

class udp_socket
{
public:
    udp_socket();
    ~udp_socket();

    void create_socket(ip_version ipv, int max_length);
    void close();                   // doesn't throw exception

    // Client
    void connect(const sockaddr* address);
    void start_receive_connected_client();
    void send(const uint8_t* data, int length);

    // Server
    void bind(unsigned short port);
    void start_receive_server();
    void send(const uint8_t* data, int size, const sockaddr* addr, int addr_len);

    unsigned short get_local_port();

protected:
    virtual void data_received(const uint8_t* data, int size, const sockaddr* addr, int addr_len);
    virtual void data_sent(const uint8_t* data, int size, const sockaddr* addr, int addr_len);
    virtual void information(const char* message);

private:
    udp_socket_impl* impl;
    friend class udp_socket_impl;
};
