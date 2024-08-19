#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <string>
#include <vector>

class utilities
{
public:
    static std::string w2a(const wchar_t* s);
    static std::string w2a(const std::wstring& s);
    static std::wstring a2w(const char* s);
    static std::wstring a2w(const std::string& s);

#ifdef _WIN32
    static std::string get_win32_error_message(DWORD dwErrorCode);
#endif

    static void check_param_interval(const char* param_name, int value, int min_value, int max_value);
    static void check_null_param(const char* param_name, const void* param_value);
    static std::string make_hex_string(const unsigned char* data, size_t size);
    static bool read_hex_string(const char* s, std::vector<unsigned char>& v);
};



