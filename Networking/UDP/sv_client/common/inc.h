#pragma once

#include <cstdint>

#ifdef _WIN32
#include <Winsock2.h>
#include <ws2tcpip.h>
#else
#include <cstddef>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <string.h>
#endif


// Number of elements in array
template <typename T, size_t N>
constexpr size_t countof(T(&)[N])
{
    return N;
}


#ifndef UNREFERENCED_PARAMETER
#ifdef _WIN32
#define UNREFERENCED_PARAMETER(P)          (P)
#else
#define UNREFERENCED_PARAMETER(P) (void)P
#endif
#endif
