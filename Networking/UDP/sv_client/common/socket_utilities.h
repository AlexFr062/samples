#pragma once

#include <vector>
#include <string>

#include "inc.h"

#include "udp_def.h"

class socket_utilities
{
public:
    static bool resolve_host(
        ip_version version,
        const char* dnsName,
        unsigned short port,
        std::vector<sockaddr_storage>& address,
        std::vector<std::string>& ip_string,
        std::string& error);
};



