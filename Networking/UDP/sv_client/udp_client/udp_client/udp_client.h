#pragma once

#include "udp_socket.h"

class MainWindow;

// Class used to redirect udp_socket callbacks to main window.
class udp_client : public udp_socket
{
public:
    udp_client(MainWindow* m);


protected:
    void data_received(const uint8_t* data, int size, const sockaddr* addr, int addr_len) override;
    void data_sent(const uint8_t* data, int size, const sockaddr* addr, int addr_len) override;
    void information(const char* message) override;
private:
    MainWindow* owner;
};
