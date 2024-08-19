#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING       // for gcc
#include <codecvt>
#include <locale>       // for Linux
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <string.h>


#include "utilities.h"
#include "program_exception.h"

#ifndef _WIN32
#define strtok_s strtok_r
#endif


using namespace std;


std::string utilities::w2a(const wchar_t* s)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    const std::string str = converter.to_bytes(s);
    return str;
}

std::string utilities::w2a(const std::wstring& s)
{
    return w2a(s.c_str());
}


std::wstring utilities::a2w(const char* s)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    const std::wstring str = converter.from_bytes(s);
    return str;
}

std::wstring utilities::a2w(const std::string& s)
{
    return a2w(s.c_str());
}

#ifdef _WIN32
std::string utilities::get_win32_error_message(DWORD dwErrorCode)
{
    void* pMsgBuf = nullptr;

    ::FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        dwErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&pMsgBuf),
        0,
        nullptr);

    std::string result;

    if (pMsgBuf)
    {
        result = reinterpret_cast<LPSTR>(pMsgBuf);
        LocalFree(pMsgBuf);
    }

    if (result.empty())
    {
        ostringstream s;

        s << "HRESULT = 0x" << std::hex << static_cast<unsigned int>(dwErrorCode);

        result = s.str();
    }

    // Remove unwanted formatted characters produced by FormatMessage
    for (auto& c : result)
    {
        if (c == '\r' || c == '\n')
        {
            c = ' ';
        }
    }

    return result;
}
#endif

void utilities::check_param_interval(const char* param_name, int value, int min_value, int max_value)
{
    if (value < min_value || value > max_value)
    {
        ostringstream s;
        s << "parameter " << param_name << " " << value << " is out of range " << min_value << "-" << max_value;
        throw program_exception(s.str().c_str(), program_error::invalid_parameter);
    }
}

void utilities::check_null_param(const char* param_name, const void* param_value)
{
    if (param_value == nullptr)
    {
        ostringstream s;
        s << "parameter " << param_name << " is null";
        throw program_exception(s.str().c_str(), program_error::invalid_parameter);
    }
}

std::string utilities::make_hex_string(const unsigned char* data, size_t size)
{
    ostringstream s;
    char buffer[4];

    if (data)
    {
        for(size_t i = 0; i < size; ++i)
        {
            //sprintf_s(buffer, sizeof(buffer), "%02X", static_cast<int>(data[i]));
            snprintf(buffer, sizeof(buffer), "%02X", static_cast<int>(data[i]));
            s << buffer << " ";
        }
    }

    return s.str();
}

// Convert hexadecimal string NN NN ... NN
// to vector of bytes.
// Returns: true - OK, false - invalid data.
bool utilities::read_hex_string(const char* s, std::vector<unsigned char>& v)
{
    v.clear();

    if (s == nullptr || s[0] == 0) return false;

    size_t length = strlen(s) + 1;

    char* str_copy = new char[length + 1];
    //strcpy_s(str_copy, length, s);
    strncpy(str_copy, s, length);

    const char *delim = " \t";
    char* token;
    char *next_token{};
    unsigned int n;
    bool result = true;

    token = strtok_s(str_copy, delim, &next_token);
//    token = strtok_r(str_copy, delim, &next_token);

    while(token)
    {
#ifdef _WIN32
        if (sscanf_s(token, "%x", &n) < 1)
#else
        if (sscanf(token, "%x", &n) < 1)
#endif
        {
            result = false;
            break;
        }

        if ( n > 0xFF )
        {
            result = false;
            break;
        }

        v.push_back(static_cast<unsigned char>(n));

        token = strtok_s(nullptr, delim, &next_token);
//        token = strtok_r(nullptr, delim, &next_token);
    }

    delete[] str_copy;

    if (v.empty()) return false;

    return result;
}



