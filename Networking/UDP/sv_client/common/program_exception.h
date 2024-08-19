#ifndef PROGRAM_EXCEPTION_H
#define PROGRAM_EXCEPTION_H

#include <string>
#include "udp_def.h"

class program_exception
{
public:
    // Generic error
    program_exception(const char* error_message, program_error error);

    // Socket API call failed. Function: API function name , such as connect.
    // socket_error_code: Windows - WSAGetLastError, Linux - errno.
    program_exception(const char* function, int socket_error_code);

    // Non-socket API call failed. Function: API function name, such as CreateFile or open.
    // api_error_code: Windows - GetLastError, Linux - errno.
    program_exception(int api_error_code, const char* function);

    program_error get_error() const { return result; }
    int get_api_error() const  { return api_error; }                    // socket or other API error code
    const char* get_error_message() const { return message.c_str(); }

private:
    void init(program_error pe, int api_error_code, const char* error_message);

    program_error result;
    int api_error;          // Windows: WSAGetLastError or GetLastError, Linux: errno. Use if result == socket_error or api_error
    std::string message;    // error message
};


#endif // PROGRAM_EXCEPTION_H
