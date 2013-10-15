#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "SocamPropertyStream.h"

//#define ARCH_64_BITS    1
#ifdef ARCH_64_BITS
typedef long int LONG64;
#else
typedef long long LONG64;
#endif

using namespace SOCAMCORE;


namespace Ui {
    class MainWindow;
}


void * start_notify_thread(void *ptr);

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

    void startNotifyTimer(int interval);

    void notifyThread();

private slots:


    void on_pushButtonKO_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    LONG64 thread_frec;
    LONG64 offline_timeout;
    bool show_seconds_offline;
    string status_file;

    LONG64 last_risk_date;

    pthread_t nthread;
};

#endif // MAINWINDOW_H
