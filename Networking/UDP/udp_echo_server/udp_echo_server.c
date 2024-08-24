#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

// UDP echo server.
// Linux.

const int max_len = 9000;


void run(int socket_type, int port);
void fatal_error_errno(const char* function);
void fatal_error(const char* message);


int main(int argc, const char *argv[])
{
#ifdef DEBUG
    printf("UDP echo server started (Debug)\n");
#else
    printf("UDP echo server started\n");
#endif

    int socket_type = AF_INET;
    int port = 41000;

    if (argc >= 2)
    {
        int n = atoi(argv[1]);

        if (n == 0 || n > 0xFFFF)
        {
            printf("Using: udp_server [port] [IP version: 4/6]\n");
            printf("       default: 41000 4\n");
            return 1;
        }

        port = n;
    }

    if (argc >= 3)
    {
        if (strcmp(argv[2], "6") == 0)
        {
            socket_type = AF_INET6;
        }
    }

    run(socket_type, port);

    return 0;
}

void fatal_error_errno(const char* function)
{
    printf("%s failed. %s\n", function, strerror(errno));
    exit(1);
}

void fatal_error(const char* message)
{
    printf("%s\n", message);
    exit(1);
}

void run(int socket_type, int port)
{
    printf("port %d IPv%d\n", port, (socket_type == AF_INET ? 4 : 6));

    // On error, -1 is returned, and errno is set to indicate the error.
    int sc = socket(socket_type, SOCK_DGRAM, IPPROTO_UDP);

    if (sc < 0)
    {
        fatal_error_errno("socket");
    }

    printf("socket is created\n");

    int bind_result;

    if (socket_type == AF_INET)
    {
        struct sockaddr_in addr = {0};

        addr.sin_family = AF_INET;
        addr.sin_port = htons((uint16_t)port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        bind_result = bind(
                sc,                                     // [in] socket
                (struct sockaddr*)(&addr),              // [in] Address to bind
                sizeof(addr));                          // [in] The length, in bytes, of the value pointed to previous parameter.
    }
    else
    {
         struct sockaddr_in6 addr = {0};

        addr.sin6_family = AF_INET6;
        addr.sin6_port = htons((uint16_t)port);
        addr.sin6_addr = in6addr_any;

        bind_result = bind(
            sc,                                     // [in] socket
            (struct sockaddr*)(&addr),                 // [in] Address to bind
            sizeof(addr));                          // [in] The length, in bytes, of the value pointed to previous parameter.
    }

    // bind: on success, zero is returned.  On error, -1 is returned, and errno is set to indicate the error.

    if (bind_result < 0)
    {
        fatal_error_errno("bind");
    }

    printf("bound to port %d\n", port);

    char* buffer = malloc(max_len);

    if (buffer == NULL)
    {
        fatal_error("malloc failed");
    }

    int recv_result;
    struct sockaddr_storage recv_address;
    socklen_t address_size;

    for(;;)
    {
        socklen_t address_size = sizeof(recv_address);

        // Returns the number of bytes received, or -1 if an error occurred.
        // In the event of an error, errno is set to indicate the error.

        recv_result = recvfrom(
            sc,                                             // [in] descriptor identifying a bound socket.
            buffer,                                         // [out] Buffer for the incoming data
            max_len,                                          // [in] Length of buffer, in bytes
            0,                                              // [in] flags
            (struct sockaddr*)(&recv_address),                 // [out] sender address
            &address_size);                                    // [in, out]

        if (bind_result < 0)
        {
            fatal_error_errno("bind");
        }

#ifdef DEBUG
        printf("%d bytes received\n", recv_result);
#endif

        // On success, returns the number of bytes sent.
        // On error, -1 is returned, and errno is set to indicate the error.

        ssize_t send_result = sendto(
            sc,
            buffer,
            recv_result,
            0,
            (struct sockaddr*)(&recv_address),
            address_size);

        if (send_result < 0)
        {
            fatal_error_errno("sendto");
        }
    }
}
