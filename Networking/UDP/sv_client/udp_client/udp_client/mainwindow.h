#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <mutex>
#include <vector>


#include <QMainWindow>
#include <QCloseEvent>
#include <QTextStream>

#include "app_settings.h"
#include "udp_client.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void data_received(const uint8_t* data, int size);
    void data_sent(const uint8_t* data, int size);
    void information(const char* message);

protected:
    void closeEvent(QCloseEvent *event) override;
    bool event(QEvent *evt) override;

private slots:
    void on_pushButtonClose_clicked();

    void on_pushButtonClearLog_clicked();

    void on_pushButtonConnect_clicked();

    void on_pushButtonDisconnect_clicked();

    void on_pushButtonSend_clicked();

private:
    Ui::MainWindow *ui;

    void PostLogString(const QString& s);
    void LoadSettings();
    void SaveSettings();
    void InitControls();
    void SetControlsState();
    void ReadControls();
    void AsyncInit();
    void PrintLog();

    ApplicationSettings settings;
    bool connected{};
    udp_client sc;
    std::recursive_mutex log_list_mutex;
    std::vector<QString> log_list;
};
#endif // MAINWINDOW_H
