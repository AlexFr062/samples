#include <sstream>

#include <QScrollBar>
#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "socket_utilities.h"
#include "program_exception.h"
#include "utilities.h"


#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

using namespace std;

// UDP client. Qt GUI, native Windows/Linux networking code.
// Supports IPv4 and IPv6.
//
// May be tested with UDP echo server or any other UDP server.


const int max_datagram_length = 10000;
const int msg_log_string = 1;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sc(this)
{
    ui->setupUi(this);

    setFixedSize(size());           // prevent resizing

    LoadSettings();
    InitControls();
    SetControlsState();

    // Asynchronous initialization. Continued asynchronously in MainWindow::event.
    // QEvent must be allocated on the heap, released by receiver.
    QCoreApplication::postEvent(
          this,                                   // receiver
          new QEvent(QEvent::User));              // event to post
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ReadControls();
    SaveSettings();
    event->accept();
}

void MainWindow::PostLogString(const QString& s)
{
    {
        std::lock_guard<std::recursive_mutex> lg(log_list_mutex);
        log_list.emplace_back(s);
    }

    QCoreApplication::postEvent(
          this,                                                                                         // receiver
          new QEvent(static_cast<QEvent::Type>(static_cast<int>(QEvent::User) + msg_log_string)));      // event to post
}



// Called asynchronoulsy from event method.
void MainWindow::PrintLog()
{
    std::vector<QString> v;

    {
        std::lock_guard<std::recursive_mutex> lg(log_list_mutex);

        if ( log_list.empty() )
        {
            return;
        }

        v = std::move(log_list);
        log_list.clear();
    }

    QString text = ui->textEditLog->toPlainText();

    for(auto& s : v)
    {
        text += s;
        text += "\n";
    }

    ui->textEditLog->setPlainText(text);
    ui->textEditLog->verticalScrollBar()->setValue(ui->textEditLog->verticalScrollBar()->maximum());   // scroll to bottom
}

// Load QSettings from external storage (Registry, configuration file).
void MainWindow::LoadSettings()
{
    // Default constructor is enough, since organization and application name are set in main function
    QSettings s;

    settings.ipAddress = s.value(KeyIpAddress, IpAddressDefault).toString();    // on error returns empty string
    settings.data = s.value(KeyData, DataDefault).toString();                   // on error returns empty string

    settings.ipVersion = static_cast<ip_version>(s.value(KeyIpVersion, static_cast<int>(IpVersionDefault)).toInt());
    ValidateIpVersion(settings.ipVersion);

    settings.port = s.value(KeyPort, PortDefault).toInt();                // on error returns 0
    ValidatePort(settings.port);
}

// Save settings to external storage (Registry, configuration file).
void MainWindow::SaveSettings()
{
    QSettings s;

    s.setValue(KeyIpAddress, settings.ipAddress);
    s.setValue(KeyData, settings.data);
    s.setValue(KeyIpVersion, static_cast<int>(settings.ipVersion));
    s.setValue(KeyPort, settings.port);
}

void MainWindow::on_pushButtonClose_clicked()
{
    this->close();
}


void MainWindow::on_pushButtonClearLog_clicked()
{
    ui->textEditLog->setPlainText("");
}

// Initialize dialog controls and fill them from QSettings
void MainWindow::InitControls()
{
    ui->lineEditIpAddress->setText(settings.ipAddress);
    ui->lineEditData->setText(settings.data);

    auto v = GetIpVersions();

    for(auto c: v)
    {
        ui->comboBoxIpVersion->addItem(IpVersionToString(c));
    }

    ui->comboBoxIpVersion->setCurrentIndex(static_cast<int>(settings.ipVersion));
    ui->lineEditPort->setText(QString::number(settings.port));
}

void MainWindow::SetControlsState()
{
    ui->pushButtonConnect->setEnabled(!connected);
    ui->pushButtonDisconnect->setEnabled(connected);
    ui->pushButtonSend->setEnabled(connected);
}

// Read data from dialog controls to QSettings
void MainWindow::ReadControls()
{
    settings.ipAddress = ui->lineEditIpAddress->text().trimmed();
    settings.data = ui->lineEditData->text().trimmed();
    settings.ipVersion = static_cast<ip_version>(ui->comboBoxIpVersion->currentIndex());

    bool ok;
    int n = ui->lineEditPort->text().toInt(&ok);

    if (!ok)
    {
        // Correct silently
        n = PortDefault;
        ui->lineEditPort->setText(QString::number(n));
    }

    ValidatePort(settings.port);

    settings.port = n;
}

bool MainWindow::event(QEvent *evt)
{
    if (evt->type() == QEvent::User)    // posted from constructor for asynchronous initialization
    {
        AsyncInit();
        return true;
    }
    else if (evt->type() == static_cast<QEvent::Type>(static_cast<int>(QEvent::User)) + msg_log_string)
    {
        PrintLog();
        return true;
    }

    return QMainWindow::event(evt);
}

void MainWindow::AsyncInit()
{
#ifdef _WIN32
    WSADATA wsa_data;

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        PostLogString("WSAStartup failed");
        ui->pushButtonConnect->setEnabled(false);
    }
#endif
}


void MainWindow::on_pushButtonConnect_clicked()
{
    ReadControls();

    if (settings.ipAddress.isEmpty())
    {
        PostLogString("IP address is empty");
        return;
    }

    ip_version ipv = static_cast<ip_version>(ui->comboBoxIpVersion->currentIndex());

    PostLogString(ipv == ip_version::v4 ? "IPv4" : "IPv6");

    vector<sockaddr_storage> address;
    vector<std::string> ip_string;
    string error;

    bool result = socket_utilities::resolve_host(
        ipv,
        settings.ipAddress.toStdString().c_str(),
        settings.port,
        address,
        ip_string,
        error);

    if (!result)
    {
        PostLogString(QString("resolve_host failed. ") + error.c_str());
        return;
    }

    {
        ostringstream s;
        s << "IP address is resolved to: " << ip_string[0] << "  port " << settings.port;
        PostLogString(s.str().c_str());
    }


    try
    {
        PostLogString("create_socket");
        sc.create_socket(ipv, max_datagram_length);

        PostLogString("connect");
        sc.connect(reinterpret_cast<const sockaddr*>(&address[0]));

        PostLogString("get_local_port");
        unsigned short port = sc.get_local_port();
        PostLogString(QString("local port ") + std::to_string(static_cast<int>(port)).c_str());

        PostLogString("start_receive_connected_client");
        sc.start_receive_connected_client();

        connected = true;
    }
    catch(const program_exception& ex)
    {
        PostLogString(ex.get_error_message());
    }

    if (!connected)
    {
        PostLogString("close");
        sc.close();
    }

    SetControlsState();
}

void MainWindow::on_pushButtonDisconnect_clicked()
{
    PostLogString("close");
    sc.close();

    connected = false;
    SetControlsState();
}


void MainWindow::on_pushButtonSend_clicked()
{
    string s = ui->lineEditData->text().trimmed().toStdString();

    if (s.empty())
    {
        PostLogString("Not data to send");
        return;
    }

    vector<unsigned char> v;

    if (!utilities::read_hex_string(s.c_str(), v))
    {
        PostLogString("Incorrect data");
        return;
    }

    s = utilities::make_hex_string(v.data(), v.size());
    ui->lineEditData->setText(s.c_str());                   // refresh print formatted string

    try
    {
        sc.send(reinterpret_cast<const uint8_t*>(v.data()), static_cast<int>(v.size()));
    }
    catch(const program_exception& ex)
    {
        PostLogString(ex.get_error_message());
    }
}

void MainWindow::data_received(const uint8_t* data, int size)
{
    if (data && size)
    {
        ostringstream s;
        s << "<< " << utilities::make_hex_string(reinterpret_cast<const unsigned char*>(data), size) << " (" << size << " bytes)";
        PostLogString(s.str().c_str());
    }
}

void MainWindow::data_sent(const uint8_t* data, int size)
{
    if (data && size)
    {
        ostringstream s;
        s << ">> " << utilities::make_hex_string(reinterpret_cast<const unsigned char*>(data), size) << " (" << size << " bytes)";
        PostLogString(s.str().c_str());
    }
}

void MainWindow::information(const char* message)
{
    if (message)
    {
        PostLogString(QString("information: ") + message);
    }
}
