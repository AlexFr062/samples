#ifdef _WIN32

#include <sstream>

#include "udp_socket_impl_w.h"
#include "udp_socket.h"
#include "utilities.h"
#include "program_exception.h"

// File: udp_socket_impl_w.cpp
// UDP socket implementation for Windows


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

    sc = socket(
        ipv == ip_version::v4 ? AF_INET : AF_INET6,
        SOCK_DGRAM,
        IPPROTO_UDP);

    if (sc == INVALID_SOCKET)
    {
        throw program_exception("socket", WSAGetLastError());
    }
}

void udp_socket_impl::close()
{
    release();
}

unsigned short udp_socket_impl::get_local_port()
{
    if (version == ip_version::v4)
    {
        struct sockaddr_in sin {};
        int addrlen = sizeof(sin);

        int result = getsockname(sc, reinterpret_cast<SOCKADDR*>(&sin), &addrlen);

        if (result != 0)
        {
            throw program_exception("getsockname", WSAGetLastError());
        }

        return ntohs(sin.sin_port);
    }
    else
    {
        struct sockaddr_in6 sin {};
        int addrlen = sizeof(sin);

        int result = getsockname(sc, reinterpret_cast<SOCKADDR*>(&sin), &addrlen);

        if (result != 0)
        {
            throw program_exception("getsockname", WSAGetLastError());
        }

        return ntohs(sin.sin6_port);
    }
}

// Connected client.
// This function may be called after create to connect to default server.
// The use start_receive_connected_client and send4/6 (without address) to talk with specific server.
//
// Connected client socket remembers the address passed to connect.
// This is the address any subsequent packets are sent to, and received from.
//
// From MSDN:
// For a connectionless socket (for example, type SOCK_DGRAM), the operation performed by connect
// is merely to establish a default destination address that can be used on subsequent send/ WSASend
// and recv/ WSARecv calls. Any datagrams received from an address other than the destination
// address specified will be discarded. If the address member of the structure specified by name
// is filled with zeros, the socket will be disconnected. Then, the default remote address will be
// indeterminate, so send/ WSASend and recv/ WSARecv calls will return the error code WSAENOTCONN.
// However, sendto/ WSASendTo and recvfrom/ WSARecvFrom can still be used.
// The default destination can be changed by simply calling connect again, even if the socket
// is already connected. Any datagrams queued for receipt are discarded if name is different
// from the previous connect.
//
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
        throw program_exception("connect", WSAGetLastError());
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

    auto t = thread(&udp_socket_impl::client_fn, this);
    t.swap(thread_client);
}

void udp_socket_impl::release()
{
    if (sc != INVALID_SOCKET)
    {
        closesocket(sc);
        sc = INVALID_SOCKET;
    }

    if (thread_client.joinable())
    {
        OutputDebugString(L"read_thread_connected_client.join started\n");

        thread_client.join();

        OutputDebugString(L"read_thread_connected_client.join ended\n");
    }

    if (thread_server.joinable())
    {
        OutputDebugString(L"server_thread.join started\n");

        thread_server.join();

        OutputDebugString(L"server_thread.join ended\n");
    }
}

// Receiving data for connected client.
void udp_socket_impl::client_fn()
{
    int recv_result;

    receive_buffer.resize(max_packet_length);

    for (;;)
    {
        recv_result = recv(
            sc,                                             // [in] descriptor identifying a bound socket.
            reinterpret_cast<char*>(receive_buffer.data()), // [out] Buffer for the incoming data
            max_packet_length,                              // [in] Length of buffer, in bytes
            0);                                             // [in] flags

        int socket_error = WSAGetLastError();

        // If no error occurs, recv returns the number of bytes received
        // and the buffer pointed to by the buf parameter will contain this data received.
        // If the connection has been gracefully closed, the return value is zero.
        // Otherwise, a value of SOCKET_ERROR is returned, and a specific error code
        // can be retrieved by calling WSAGetLastError.

        if (recv_result == 0)
        {
            //OutputDebugString(L"recv returned 0, break\n");
            owner->information("recv returned 0, break");
            break;
        }
        else if (recv_result == SOCKET_ERROR)
        {
            if (socket_error == WSAENOTSOCK)
            {
                // Actually, when socket is closed, we have WSAENOTSOCK
                //OutputDebugString(L"recv: SOCKET_ERROR, WSAENOTSOCK\n");
                owner->information("recv: SOCKET_ERROR, WSAENOTSOCK");
                break;
            }

            // Any other error is not critical - report and continue.
            // For example, if server exits, we get WSAECONNRESET. But when the same server
            // starts again, recvfrom is working.

            ostringstream s;
            s << "recvfrom failed. " <<
                //wsa_error::get_error_string(socket_error);
                utilities::get_win32_error_message(static_cast<DWORD>(socket_error));
            owner->information(s.str().c_str());
        }
        else
        {
            owner->data_received(receive_buffer.data(), recv_result, reinterpret_cast<const sockaddr*>(&connected_address), address_len);
        }
    }
}

// Send data for connected client.
void udp_socket_impl::send(const uint8_t* data, int length)
{
    utilities::check_null_param("data", data);
    utilities::check_param_interval("length", length, 1, max_packet_length);

    int result = ::send(sc, reinterpret_cast<const char*>(data), length, 0);

    if (result == 0)
    {
        throw program_exception("send", WSAGetLastError());
    }

    owner->data_sent(data, result, reinterpret_cast<const sockaddr*>(&connected_address), address_len);

    if (result != length)
    {
        ostringstream s;
        s << "send: sent " << result << " bytes, expected " << length;
        throw program_exception(s.str().c_str(), program_error::not_all_data_sent);
    }
}

// Server.
// Bind the socket to specific port.
//
// port = 0 - use ephemeral port.
//
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
        throw program_exception("bind", WSAGetLastError());
    }

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

    const int result = sendto(sc, reinterpret_cast<const char*>(data), size, 0, reinterpret_cast<const sockaddr*>(addr), addr_len);

    if (result == SOCKET_ERROR)
    {
        throw program_exception("sendto", WSAGetLastError());
    }

    owner->data_sent(data, size, addr, addr_len);

    if (result != size)
    {
        ostringstream s;
        s << "send: sent " << result << " bytes, expected " << size;
        throw program_exception(s.str().c_str(), program_error::not_all_data_sent);
    }

}

void udp_socket_impl::start_receive_server()
{
    if (thread_server.joinable())
    {
        throw program_exception("receive thread is already running", program_error::invalid_operation);
    }

    auto t = thread(&udp_socket_impl::server_fn, this);
    t.swap(thread_server);

}

void udp_socket_impl::server_fn()
{
    sockaddr_storage recv_address;
    int address_size;

    receive_buffer.resize(max_packet_length);

    for (;;)
    {
        address_size = sizeof(recv_address);

        const int recv_result = recvfrom(
            sc,                                             // [in] descriptor identifying a bound socket.
            reinterpret_cast<char*>(receive_buffer.data()), // [out] Buffer for the incoming data
            max_packet_length,                              // [in] Length of buffer, in bytes
            0,                                              // [in] flags
            reinterpret_cast<sockaddr*>(&recv_address),     // [out] sender address
            &address_size);                                 // [in, out] recv_address length

        const int socket_error = WSAGetLastError();

        // If no error occurs, recv returns the number of bytes received
        // and the buffer pointed to by the buf parameter will contain this data received.
        // If the connection has been gracefully closed, the return value is zero.
        // Otherwise, a value of SOCKET_ERROR is returned, and a specific error code
        // can be retrieved by calling WSAGetLastError.

        if (recv_result == 0)
        {
            //OutputDebugString(L"recvfrom returned 0, break\n");
            owner->information("recvfrom returned 0, break");
            break;
        }
        else if (recv_result == SOCKET_ERROR)
        {
            if (socket_error == WSAENOTSOCK)
            {
                // Actually, when socket is closed, we have WSAENOTSOCK
                owner->information("recvfrom: SOCKET_ERROR, WSAENOTSOCK");
                break;
            }

            // Any other error is not critical - report and continue.
            // For example, if server exits, we get WSAECONNRESET. But when the same server
            // starts again, recvfrom is working.

            ostringstream s;
            s << "recvfrom failed (server). " <<
                //wsa_error::get_error_string(socket_error);
                utilities::get_win32_error_message(static_cast<DWORD>(socket_error));
            owner->information(s.str().c_str());
        }
        else
        {
            owner->data_received(receive_buffer.data(), recv_result, reinterpret_cast<const sockaddr*>(&recv_address), address_size);
        }
    }
}


#endif
