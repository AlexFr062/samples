#ifndef _WIN32

#include <sstream>

#include <sys/epoll.h>
#include <unistd.h>

#include "udp_socket_impl_l.h"
#include "udp_socket.h"
#include "program_exception.h"
#include "utilities.h"
#include "sync_print.h"

// File: udp_socket_impl_l.cpp
// UDP socket implementation for Linux

#ifndef QT_NO_DEBUG
#define DEBUG_PRINT
#endif

#ifdef DEBUG_PRINT
#include "sync_print.h"

template <typename... Args>
void debug_print(Args&&... args)
{
    sync_print(std::forward<Args>(args)...);
}

#else
#define debug_print(...)
#endif


#define PIPE_CHANNEL_READ  0
#define PIPE_CHANNEL_WRITE 1

using namespace std;

udp_socket_impl::udp_socket_impl(udp_socket* s) :
    owner(s)
{

}

udp_socket_impl::~udp_socket_impl()
{
    release();
}

void udp_socket_impl::create_socket(ip_version ipv, int max_length)
{
    utilities::check_param_interval("max_length", max_length, 1, 20000);
    utilities::check_param_interval("ipv", static_cast<int>(ipv), static_cast<int>(ip_version::v4), static_cast<int>(ip_version::v6));

    if (sc != INVALID_SOCKET)
    {
        throw program_exception("socket is already created", program_error::invalid_operation);
    }

    version = ipv;
    max_packet_length = max_length;
    address_len = (version == ip_version::v4 ? sizeof(sockaddr_in) : sizeof(sockaddr_in6));

    sc = ::socket(
        ipv == ip_version::v4 ? AF_INET : AF_INET6,
        SOCK_DGRAM,
        IPPROTO_UDP);

    if (sc == INVALID_SOCKET)
    {
        throw program_exception("socket", errno);
    }

    // Create control pipe
    int result = pipe(pipefd);

    if (result == -1)
    {
        ::close(sc);
        sc = INVALID_SOCKET;
        throw program_exception("pipe", errno);
    }
}

void udp_socket_impl::close()
{
    release();
}

unsigned short udp_socket_impl::get_local_port()
{
    int result;

    if (version == ip_version::v4)
    {
        struct sockaddr_in sin;
        socklen_t addrlen = sizeof(sin);

        result = getsockname(sc, (struct sockaddr*)&sin, &addrlen);

        if (result == -1)
        {
            throw program_exception("getsockname", errno);
        }

        return ntohs(sin.sin_port);
    }
    else
    {
        struct sockaddr_in6 sin;
        socklen_t addrlen = sizeof(sin);

        result = getsockname(sc, (struct sockaddr*)&sin, &addrlen);

        if (result == -1)
        {
            throw program_exception("getsockname", errno);
        }

        return ntohs(sin.sin6_port);
    }
}

void udp_socket_impl::connect(const sockaddr* address)
{
    utilities::check_null_param("address", address);

    int namelen = (version == ip_version::v4 ? sizeof(sockaddr_in) : sizeof(sockaddr_in6));

    int result = ::connect(
                sc,                                 // [in] socket
                address,                            // [in] Server address
                namelen);                           // [in] The length, in bytes, of the value pointed to previous parameter.


    if (result != 0)
    {
        throw program_exception("connect", errno);
    }

    memcpy(&connected_address, address, address_len);     // address used for receive callback
}

// Start receiving data for connected client.
void udp_socket_impl::start_receive_connected_client()
{
    if (thread_client.joinable())
    {
        throw program_exception("receive thread is already running", program_error::invalid_operation);
    }

    auto t = thread(&udp_socket_impl::read_thread_connected_client_function, this);
    t.swap(thread_client);}


// Send data for connected client.
void udp_socket_impl::send(const uint8_t* data, int length)
{
    utilities::check_null_param("data", data);
    utilities::check_param_interval("length", length, 1, max_packet_length);

    ssize_t result = ::send(sc, data, length, 0);

    if (result < 0)
    {
        throw program_exception("send", errno);
    }

    owner->data_sent(data, result, reinterpret_cast<const sockaddr*>(&connected_address), address_len);

    if (result != length)
    {
        ostringstream s;
        s << "send: sent " << result << " bytes, expected " << length;
        throw program_exception(s.str().c_str(), program_error::not_all_data_sent);
    }
}

void udp_socket_impl::release()
{
    if (sc != INVALID_SOCKET)
    {
        debug_print("write to control pipe");

        char ctrl_data = 0;
        ssize_t write_result = write(pipefd[PIPE_CHANNEL_WRITE], &ctrl_data, 1);

        if (write_result == -1)
        {
            debug_print(strerror(errno));
        }
    }

    if (thread_client.joinable())
    {
        debug_print("read_thread_connected_client.join");

        thread_client.join();

        debug_print("read_thread_connected_client.join ended");
    }

    if (thread_server.joinable())
    {
        debug_print("server_thread.join");

        thread_server.join();

        debug_print("server_thread.join ended");
    }

    if (sc != INVALID_SOCKET)
    {
        debug_print("close socket and pipe");

        ::close(sc);
        ::close(pipefd[PIPE_CHANNEL_READ]);
        ::close(pipefd[PIPE_CHANNEL_WRITE]);

        debug_print("socket and pipes are closed");

        sc = INVALID_SOCKET;
    }

}

// Report Linux API failure.
// Can be used for any function which sets errno.
// function - API function name.
// errno_value - errno.
void udp_socket_impl::report_api_error(const char* function, int errno_value)
{
    ostringstream s;
    s << function << " failed. " << strerror(errno_value);
    owner->information(s.str().c_str());
}


// Client function.
//
// Receive packets from connected address.
// Runs in worker thread, doesn't throw exception.
void udp_socket_impl::read_thread_connected_client_function()
{
    ssize_t read_result;
    receive_buffer.resize(max_packet_length);


    // Create epoll for monitoring 2 events

    debug_print("create epoll");

    int ep = epoll_create(2);			// epoll descriptor

    if (ep == -1)
    {
        report_api_error("epoll_create", errno);
        return;
    }

    // epoll_event may contain any data, we fill it with pipe descriptor.
    // When epoll is used with several files, event data is used to see what file
    // is the source of event.

    epoll_event ev;
    ev.data.fd = sc;
    ev.events = EPOLLIN;    // The associated file is available for read(2) operations

    // Add pipe descriptor to epoll

    int ctl_result = epoll_ctl(ep, EPOLL_CTL_ADD, sc, &ev);

    if (ctl_result == -1)
    {
        report_api_error("epoll_ctl(sc)", errno);
        return;
    }

    epoll_event ctrl_ev;
    ctrl_ev.data.fd = pipefd[PIPE_CHANNEL_READ];
    ctrl_ev.events = EPOLLIN;    // The associated file is available for read(2) operations

    ctl_result = epoll_ctl(ep, EPOLL_CTL_ADD, pipefd[PIPE_CHANNEL_READ], &ctrl_ev);

    if (ctl_result == -1)
    {
        report_api_error("epoll_ctl(pipefd)", errno);
        return;
    }

    debug_print("waiting for events");

    epoll_event input_event[2];

    for(;;)
    {
        int wait_result = epoll_wait(
                ep,					// epoll descriptor
                input_event,		// filled by epoll_wait
                2,                  // events count
                -1);				// timeout, infinite


        if (wait_result == -1)
        {
            report_api_error("epoll_wait", errno);
            break;
        }
        else if (wait_result < 1  || wait_result > 2)			// 1 or 2 are expected return values
        {
            ostringstream s;
            s <<  "unexpected wait_result: " <<  wait_result;
            owner->information(s.str().c_str());
            break;
        }

        bool stop_requested = false;
        bool socket_data_available = false;

        for(int i = 0; i < wait_result; ++i)
        {
            if (input_event[i].data.fd == pipefd[PIPE_CHANNEL_READ])
            {
                stop_requested = true;
            }
            else if (input_event[i].data.fd == sc)
            {
                socket_data_available = true;
            }
        }

        if (stop_requested)
        {
            debug_print("data is received from control pipe, break");

            // No need to read the pipe, if any data is available - break and exit the thread
            break;
        }
        else if (socket_data_available)
        {
            // socket is available for reading

            debug_print("read started");

            read_result = read(sc, receive_buffer.data(), max_packet_length);  // recv and recvfrom may be used here

            debug_print("read ended");

            if (read_result == -1)
            {
                int n = errno;

                 ostringstream s;
                 s << "read failed. " << strerror(n);
                 owner->information(s.str().c_str());
                 break;
            }
            else if (read_result == 0)
            {
                owner->information("read returned 0 bytes");
                break;
            }
            else
            {
                owner->data_received(receive_buffer.data(), read_result, reinterpret_cast<const sockaddr*>(&connected_address), address_len);
            }
        }
    }

    debug_print("close epoll");

    ::close(ep);

    debug_print("thread ended");
}

// Server.
// Bind the socket to specific port.
//
// local_port = 0 - use ephemeral port.
void udp_socket_impl::bind(unsigned short port)
{
    int result;

    if (version == ip_version::v4)
    {
        sockaddr_in addr{};

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        result = ::bind(
            sc,                                     // [in] socket
            reinterpret_cast<sockaddr*>(&addr),     // [in] Address to bind
            sizeof(addr));                          // [in] The length, in bytes, of the value pointed to previous parameter.
    }
    else
    {
        sockaddr_in6 addr{};

        addr.sin6_family = AF_INET6;
        addr.sin6_port = htons(port);
        addr.sin6_addr = in6addr_any;

        result = ::bind(
            sc,                                     // [in] socket
            reinterpret_cast<sockaddr*>(&addr),     // [in] Address to bind
            sizeof(addr));                          // [in] The length, in bytes, of the value pointed to previous parameter.
    }

    // If no error occurs, bind returns zero. Otherwise, it returns SOCKET_ERROR,
    // and a specific error code can be retrieved by calling WSAGetLastError.

    if (result != 0)
    {
        throw program_exception("bind", errno);
    }
}

// Start receiving data for server.
void udp_socket_impl::start_receive_server()
{
    auto t = thread(&udp_socket_impl::server_fn, this);
    t.swap(thread_server);
}

// Receiving data for server.
void udp_socket_impl::server_fn()
{
    int recv_result;
    sockaddr_storage recv_address;
    socklen_t address_size;

    receive_buffer.resize(max_packet_length);

    // Create epoll for monitoring 2 events

    debug_print("create epoll");

    int ep = epoll_create(2);			// epoll descriptor

    if (ep == -1)
    {
        report_api_error("epoll_create", errno);
        return;
    }

    // epoll_event may contain any data, we fill it with pipe descriptor.
    // When epoll is used with several files, event data is used to see what file
    // is the source of event.

    epoll_event ev;
    ev.data.fd = sc;
    ev.events = EPOLLIN;    // The associated file is available for read(2) operations

    // Add pipe descriptor to epoll

    int ctl_result = epoll_ctl(ep, EPOLL_CTL_ADD, sc, &ev);

    if (ctl_result == -1)
    {
        report_api_error("epoll_ctl(sc)", errno);
        return;
    }

    epoll_event ctrl_ev;
    ctrl_ev.data.fd = pipefd[PIPE_CHANNEL_READ];
    ctrl_ev.events = EPOLLIN;    // The associated file is available for read(2) operations

    ctl_result = epoll_ctl(ep, EPOLL_CTL_ADD, pipefd[PIPE_CHANNEL_READ], &ctrl_ev);

    if (ctl_result == -1)
    {
        report_api_error("epoll_ctl(pipefd)", errno);
        return;
    }

    debug_print("waiting for events");

    epoll_event input_event[2];

    for(;;)
    {
        int wait_result = epoll_wait(
            ep,					// epoll descriptor
            input_event,		// filled by epoll_wait
            2,                  // events count
            -1);				// timeout, infinite


        if (wait_result == -1)
        {
            report_api_error("epoll_wait", errno);
            break;
        }
        else if (wait_result < 1  || wait_result > 2)			// 1 or 2 are expected return values
        {
            ostringstream s;
            s <<  "unexpected wait_result: " <<  wait_result;
            owner->information(s.str().c_str());
            break;
        }

        bool stop_requested = false;
        bool socket_data_available = false;

        for(int i = 0; i < wait_result; ++i)
        {
            if (input_event[i].data.fd == pipefd[PIPE_CHANNEL_READ])
            {
                stop_requested = true;
            }
            else if (input_event[i].data.fd == sc)
            {
                socket_data_available = true;
            }
        }

        if (stop_requested)
        {
            debug_print("data is received from control pipe, break");

            // No need to read the pipe, if any data is available - break and exit the thread
            break;
        }
        else if (socket_data_available)
        {
            // socket is available for reading

            debug_print("recvfrom started");

            address_size = sizeof(recv_address);

            recv_result = recvfrom(
                sc,                                             // [in] descriptor identifying a bound socket.
                reinterpret_cast<char*>(receive_buffer.data()), // [out] Buffer for the incoming data
                max_packet_length,                              // [in] Length of buffer, in bytes
                0,                                              // [in] flags
                reinterpret_cast<sockaddr*>(&recv_address),     // [out] sender address
                &address_size);

            debug_print("recvfrom ended");

            if (recv_result == -1)
            {
                int n = errno;

                ostringstream s;
                s << "recvfrom failed. " << strerror(n);
                owner->information(s.str().c_str());
                break;
            }
            else if (recv_result == 0)
            {
                owner->information("recvfrom returned 0 bytes");
                break;
            }
            else
            {
                owner->data_received(receive_buffer.data(), recv_result, reinterpret_cast<const sockaddr*>(&recv_address), address_len);
            }
        }
    }

    debug_print("close epoll");

    ::close(ep);

    debug_print("thread ended");

}

// Server:
// Send data to client.
// address should point to IPv4/IPv6 address, depending on the server socket type
// (as was set in create_socket).
// addr_len should be sizeof(sockaddr_in) or sizeof(sockaddr_in6).
// Typically, sockaddr* address from data_received is used,
// which ensures that client address is OK.
//
void udp_socket_impl::send(const uint8_t* data, int size, const sockaddr* addr, int addr_len)
{
    utilities::check_param_interval("size", size, 1, max_packet_length);

    ssize_t result = sendto(sc, reinterpret_cast<const char*>(data), size, 0, reinterpret_cast<const sockaddr*>(addr), addr_len);

    if (result < 0)
    {
        throw program_exception("sendto", errno);
    }

    owner->data_sent(data, size, addr, addr_len);

    if (result != size)
    {
        ostringstream s;
        s << "send: sent " << result << " bytes, expected " << size;
        throw program_exception(s.str().c_str(), program_error::not_all_data_sent);
    }
}


#endif
