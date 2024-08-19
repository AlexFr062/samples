#include "udp_client.h"
#include "mainwindow.h"

udp_client::udp_client(MainWindow* m) :
    owner(m)
{

}

void udp_client::data_received(const uint8_t* data, int size, const sockaddr* addr, int addr_len)
{
    UNREFERENCED_PARAMETER(addr);
    UNREFERENCED_PARAMETER(addr_len);

    owner->data_received(data, size);
}

void udp_client::data_sent(const uint8_t* data, int size, const sockaddr* addr, int addr_len)
{
    UNREFERENCED_PARAMETER(addr);
    UNREFERENCED_PARAMETER(addr_len);

    owner->data_sent(data, size);
}

void udp_client::information(const char* message)
{
    owner->information(message);
}
