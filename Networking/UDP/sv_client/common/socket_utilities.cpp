#include <sstream>

#include "socket_utilities.h"
#include "utilities.h"


using namespace std;

#ifdef _WIN32

// Resolve host name.
//
// When called with ip_version::v4, fills address vector with sockaddr_in structures.
// When called with ip_version::v6, fills address vector with sockaddr_in6 structures.
//
// On error: fills error parameter with error message and returns false.
bool socket_utilities::resolve_host(
    ip_version version,                         // [in] IP version
    const char* dnsName,                        // [in] DNS name to resolve
    unsigned short port,                        // [in] port number
    std::vector<sockaddr_storage>& address,     // [out] list of resolved IPv4/IPv6 addresses
    std::vector<std::string>& ip_string,        // [out] list of resolved IPv4/IPv6 addresses as strings
    std::string& error)
{
    // Fill hints for getaddrinfo
    struct addrinfo hints {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_TCP;

    string serviceName = std::to_string(static_cast<int>(port));

    struct addrinfo *list{};
    struct addrinfo *ptr;
    struct sockaddr_in *sockaddr_ipv4;
    struct sockaddr_in6 *sockaddr_ipv6;
    wchar_t ipstringbuffer[46];         // enough for IPv4 and IPv6

    INT result = getaddrinfo(
            dnsName,				// [in] address
            serviceName.c_str(),    // [in] port (generally, service name on the host)
            &hints,                 // [in] required socket type
            &list);

    if (result != 0)
    {
        // getaddrinfo return value is socket error code
        ostringstream s;
        s << "getaddrinfo failed. " <<
            //wsa_error::get_error_string(result);
            utilities::get_win32_error_message(static_cast<DWORD>(result));
        error = s.str();
        return false;
    }

    int family = (version == ip_version::v4 ? AF_INET : AF_INET6);
    PCWSTR str;
    sockaddr_storage storage;

    // Read linked list
    for (ptr = list; ptr != NULL; ptr = ptr->ai_next)
    {
        // Read only requested members
        if (ptr->ai_family != family)
        {
            continue;
        }

        if (version == ip_version::v4)
        {
            sockaddr_ipv4 = (struct sockaddr_in*) ptr->ai_addr;

            // InetNtopW function converts an IPv4 or IPv6 Internet network address into a string in Internet standard format.

            str = InetNtopW(
                AF_INET,
                &sockaddr_ipv4->sin_addr,		// address
                ipstringbuffer,					// buffer to fill
                countof(ipstringbuffer));		// number of characters in the buffer
        }
        else
        {
            sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;

            str =  InetNtopW(
                AF_INET6,
                &sockaddr_ipv6->sin6_addr,		// address
                ipstringbuffer,					// buffer to fill
                countof(ipstringbuffer));		// For an IPv6 address, this buffer should be large enough to hold at least 46 characters.
        }

        if (str == nullptr)
        {
            freeaddrinfo(list);

            int last_error = WSAGetLastError();

            ostringstream s;

            s << "InetNtopW failed. " <<
                //wsa_error::get_error_string(last_error);
                utilities::get_win32_error_message(static_cast<DWORD>(last_error));

            error = s.str();
            return false;
        }

        // Fill output arrays
        if (version == ip_version::v4)
        {
            memcpy(&storage, sockaddr_ipv4, sizeof(sockaddr_in));
        }
        else
        {
            memcpy(&storage, sockaddr_ipv6, sizeof(sockaddr_in6));
        }

        address.push_back(storage);
        ip_string.push_back(utilities::w2a(ipstringbuffer));
    }

    freeaddrinfo(list);

    if (address.empty())
    {
        error = "Address cannot be resolved";
        return false;
    }

    return true;
}

#else
// Resolve host name.
//
// When called with ip_version::v4, fills address vector with sockaddr_in structures.
// When called with ip_version::v6, fills address vector with sockaddr_in6 structures.
//
// On error: fills error parameter with error message and returns false.
bool socket_utilities::resolve_host(
    ip_version version,                         // [in] IP version
    const char* dnsName,                        // [in] DNS name to resolve
    unsigned short port,                        // [in] port number
    std::vector<sockaddr_storage>& address,     // [out] list of resolved IPv4/IPv6 addresses
    std::vector<std::string>& ip_string,        // [out] list of resolved IPv4/IPv6 addresses as strings
    std::string& error)
{
    // Fill hints for getaddrinfo
    struct addrinfo hints {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;        // SOCK_DGRAM doesn't work
    hints.ai_protocol = IPPROTO_TCP;

    string serviceName = std::to_string(static_cast<int>(port));

    addrinfo *list{};
    addrinfo* result;
    addrinfo *ptr;
    struct sockaddr_in *sockaddr_ipv4;
    struct sockaddr_in6 *sockaddr_ipv6;
    char buffer[INET6_ADDRSTRLEN];		// enough for IPv4 and IPv6
    const char* a;

    int n = getaddrinfo(dnsName, serviceName.c_str(), &hints, &result);

    if (n != 0)
    {
        const char* message = gai_strerror(n);
        error = string("getaddrinfo failed. ") + message;
        return false;
    }

    int family = (version == ip_version::v4 ? AF_INET : AF_INET6);
    sockaddr_storage storage;

    for (ptr = result; ptr != nullptr; ptr = ptr->ai_next)
    {
        // Read only requested members
        if (ptr->ai_family != family)
        {
            continue;
        }

        if (version == ip_version::v4)
        {
            sockaddr_ipv4 = (struct sockaddr_in*) ptr->ai_addr;

            // convert address to string
            a = inet_ntop(AF_INET, &((struct sockaddr_in *)ptr->ai_addr)->sin_addr, buffer, INET6_ADDRSTRLEN);
        }
        else
        {
            sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;

            // convert address to string
            a = inet_ntop(AF_INET6, &((struct sockaddr_in6 *)ptr->ai_addr)->sin6_addr, buffer, INET6_ADDRSTRLEN);
        }

        if (a == nullptr)
        {
            const char* message = strerror(errno);

            freeaddrinfo(result);
            error = string("inet_ntop failed. ") + message;
            return false;
        }

        // Fill output arrays
        if (version == ip_version::v4)
        {
            memcpy(&storage, sockaddr_ipv4, sizeof(sockaddr_in));
        }
        else
        {
            memcpy(&storage, sockaddr_ipv6, sizeof(sockaddr_in6));
        }

        address.push_back(storage);
        ip_string.push_back(buffer);
    }

    freeaddrinfo(list);

    if (address.empty())
    {
        error = "Address cannot be resolved";
        return false;
    }

    return true;
}

#endif
