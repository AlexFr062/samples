#include <sstream>

#include "program_exception.h"
#include "utilities.h"

#ifdef _WIN32
#else
#include <string.h>
#endif

using namespace std;

// Generic error
program_exception::program_exception(const char* error_message, program_error error)
{
    init(error, 0, error_message);
}

// Socket API call failed. Function: API function name , such as connect.
// socket_error_code: Windows - WSAGetLastError, Linux - errno.
program_exception::program_exception(const char* function, int socket_error_code)
{
    ostringstream s;
    s << function << " failed. ";

#ifdef _WIN32
//    s << wsa_error::get_error_string(socket_error_code);
    s << utilities::get_win32_error_message(static_cast<DWORD>(socket_error_code));
#else
    s << strerror(socket_error_code);
#endif

    init(program_error::socket_error, socket_error_code, s.str().c_str());
}

// Non-socket API call failed. Function: API function name, such as CreateFile or open.
// api_error_code: Windows - GetLastError, Linux - errno.
program_exception::program_exception(int api_error_code, const char* function)
{
    ostringstream s;
    s << function << " failed. ";

#ifdef _WIN32
    s << utilities::get_win32_error_message(static_cast<DWORD>(api_error_code));
#else
    s << strerror(api_error_code);
#endif

    init(program_error::api_error, api_error_code, s.str().c_str());
}

void program_exception::init(program_error pe, int api_error_code, const char* error_message)
{
    result = pe;
    api_error = api_error_code;
    message = error_message;
}
