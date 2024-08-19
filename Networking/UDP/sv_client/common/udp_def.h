#pragma once

class udp_def {};

enum class program_error
{
    ok,
    not_implemented,
    invalid_parameter,
    socket_error,           // Socket operation failed. Error code: Windows - WSAGetLastError, Linux - errno
    api_error,              // Non-socket API failed. Error code: Windows - GetLastError, Linux - errno
    invalid_operation,
    not_all_data_sent
};

enum class ip_version
{
    v4,
    v6
};


