#ifdef _WIN32
#include <conio.h>
#endif

#include <limits>

#include "sync_print.h"
#include "udp_socket.h"
#include "program_exception.h"

using namespace std;

// UDP echo server.
//
// Command line parameters:
// [port] [IP version: 4/6]


std::mutex sync_mutex;          // for sync_print
#define USE_GETCH               // optional, for Windows

#ifdef max
#undef max
#endif

// Note: If there are problems with built-in Qt Creator terminal, click Projects, Run,
//       check Run - Run in terminal.

class echo_server : public udp_socket
{
public:
    echo_server(){}


protected:
    virtual void data_received(const uint8_t* data, int size, const sockaddr* addr, int addr_len)
    {
        //sync_print("Received ", size, " bytes");

        try
        {
            // Send data back to the client
            send(data, size, addr, addr_len);
        }
        catch (const program_exception& ex)
        {
            sync_print(ex.get_error_message());
        }
    }

    virtual void information(const char* message)
    {
        sync_print("Information: ", message);
    }

};


int main(int argc, const char *argv[])
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    int port = 41000;
    ip_version version = ip_version::v4;

    if (argc >= 2)
    {
        int n = atoi(argv[1]);

        if (n == 0 || n > numeric_limits<unsigned short>::max())
        {
            sync_print("Using: udp_server [port] [IP version: 4/6]");
            sync_print("       default: 41000 4");
            return 1;
        }

        port = n;
    }

    if (argc >= 3)
    {
        if (strcmp(argv[2], "6") == 0)
        {
            version = ip_version::v6;
        }
    }

#ifdef _WIN32
    WSADATA wsa_data;

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        sync_print("WSAStartup failed");
        return 1;
    }
#endif

    echo_server sv;

    try
    {
        sync_print("create_socket");
        sv.create_socket(version, 9000);

        sync_print("bind to port ", port);
        sv.bind(port);

        sync_print("start_receive_server");
        sv.start_receive_server();
    }
    catch (const program_exception& ex)
    {
        sync_print(ex.get_error_message());
        return 1;
    }


    sync_print("Server is running. Port ", port, (version == ip_version::v4 ? " IPv4" : " IPv6"));

#if defined _WIN32 && defined USE_GETCH
    sync_print("Press any key to exit");
    _getch();
#else
    sync_print("Enter any string to exit");
    string s;
    cin >> s;
#endif


    return 0;
}
