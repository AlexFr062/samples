#include "udp_socket.h"


#ifdef _WIN32
#include "udp_socket_impl_w.h"
#else
#include "udp_socket_impl_l.h"
#endif


udp_socket::udp_socket()
{
    impl = new udp_socket_impl(this);
}

udp_socket::~udp_socket()
{
    delete impl;
}

void udp_socket::create_socket(ip_version ipv, int max_length)
{
    impl->create_socket(ipv, max_length);
}

void udp_socket::close()
{
    impl->close();
}

unsigned short udp_socket::get_local_port()
{
    return impl->get_local_port();
}

void udp_socket::connect(const sockaddr* address)
{
    impl->connect(address);
}


void udp_socket::start_receive_connected_client()
{
    impl->start_receive_connected_client();
}

void udp_socket::send(const uint8_t* data, int length)
{
    impl->send(data, length);
}

void udp_socket::bind(unsigned short port)
{
    impl->bind(port);
}

void udp_socket::start_receive_server()
{
    impl->start_receive_server();
}

void udp_socket::send(const uint8_t* data, int size, const sockaddr* addr, int addr_len)
{
    impl->send(data, size, addr, addr_len);
}


void udp_socket::data_received(const uint8_t* data, int size, const sockaddr* addr, int addr_len)
{
   UNREFERENCED_PARAMETER(data);
   UNREFERENCED_PARAMETER(size);
   UNREFERENCED_PARAMETER(addr);
   UNREFERENCED_PARAMETER(addr_len);
}

void udp_socket::data_sent(const uint8_t* data, int size, const sockaddr* addr, int addr_len)
{
    UNREFERENCED_PARAMETER(data);
    UNREFERENCED_PARAMETER(size);
    UNREFERENCED_PARAMETER(addr);
    UNREFERENCED_PARAMETER(addr_len);
}

void udp_socket::information(const char* message)
{
    UNREFERENCED_PARAMETER(message);
}


