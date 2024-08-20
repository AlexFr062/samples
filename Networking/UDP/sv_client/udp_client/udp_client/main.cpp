#include "mainwindow.h"

#include <QApplication>

// UDP client (connected, IPv4+IPv6) with Qt GUI.


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Allows to use default QSettings constructor.
    // Registry key: HKEY_CURRENT_USER\Software\AlexF\udp_client
    QCoreApplication::setOrganizationName("AlexF");
    QCoreApplication::setApplicationName("udp_client");

    MainWindow w;
    w.show();
    return a.exec();
}


std::mutex sync_mutex;    // sir sync_print
