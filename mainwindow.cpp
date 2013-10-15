#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QMessageBox>
#include <QtCore/QTimer>

#include <iostream>
#include <fstream>
#include <pthread.h>
using namespace std;

#define CONFIG_FILE                 "/etc/socam/notifier.conf"
#define DEFAULT_OFFLINE_TIMEOUT     60
#define DEFAULT_STATUS_FILE         "~/shared/.status"
#define DEFAULT_THREAD_FRECUENCY    5000000 //microseconds
#define DEFAULT_OFFLINE_SECONDS     true

#define always  while(1)

void * start_notify_thread(void *ptr) {
    MainWindow * me=(MainWindow*)ptr;
    me->notifyThread();
    return NULL;
}



void MainWindow::notifyThread() {

    always {
        QString str = QString::fromStdString(this->status_file);
        //cerr << "STATUS FILE PATH = "<<str.toStdString()<<endl;
        QFile file(str);
        if(!file.open(QIODevice::ReadOnly)) {
            cerr << "FAILED TO OPEN FILE: "<<this->status_file<<" !!!" <<endl;
            ui->labelStatus->setText("UNABLE TO OPEN STATUS FILE!!");
            ui->labelKO->setText("");
            usleep(this->thread_frec);
            continue;
        }

        QTextStream in(&file);
        QString line = "";
        while(!in.atEnd()) {
            line = in.readLine();
        }
        file.close();

        if(line.length()>0) {
            QStringList list=line.split(' ');

            //Check last online push!!!!+
            LONG64 now=time(NULL);
            LONG64 date=atoll(list.at(1).toStdString().c_str());
            date=date/1000; //parse to seconds
            QString dateStr=ctime((const time_t*)&date);
            dateStr.data()[dateStr.size()-1]=' ';
            LONG64 difft=now-date;
            QString difftStr;
            difftStr.setNum(difft);

            if (difft>this->offline_timeout) {
                if (this->show_seconds_offline) {
                    ui->labelStatus->setText((const QString)"Offline [ "+dateStr+"] "+difftStr+" seconds ago");
                }
                else {
                    ui->labelStatus->setText((const QString)"Offline [ "+dateStr+"]");
                }
            }
            else {
                ui->labelStatus->setText((const QString)"Online [ "+dateStr+"]");
            }

            if (list.at(0).compare("KO")==0 && last_risk_date!=date) {
                ui->pushButtonKO->setEnabled(true);
                ui->labelKO->setText((const QString)"RISK [ "+dateStr+"]");
                last_risk_date=date;
                //QMessageBox::warning(0, "NOTIFY", "Risk detected!");
            }
        }

        usleep(this->thread_frec);
    }
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    this->last_risk_date=0;
    //Read config file
    SocamPropertyStream * pstream=new SocamPropertyStream(CONFIG_FILE);
    if (pstream->isEmpty() || !pstream->isAlready() || pstream->Status()!=P_NO_ERROR)
    {
        cerr << "SocamPropertyStream OPEN FAILED!!"<<endl;
        cerr << "Property Config [STATUS-FILE] not found!!.. Default path "<<DEFAULT_STATUS_FILE<<" sec.."<<endl;
        this->status_file=DEFAULT_STATUS_FILE;
        cerr << "Property Config [OFFLINE-TIMEOUT] not found!!.. Default set "<<DEFAULT_OFFLINE_TIMEOUT<<" sec.."<<endl;
        this->offline_timeout=DEFAULT_OFFLINE_TIMEOUT;
        cerr << "Property Config [THREAD-FRECUENCY] not found!!.. Default set "<<DEFAULT_THREAD_FRECUENCY<<" sec.."<<endl;
        this->thread_frec=DEFAULT_THREAD_FRECUENCY;
        cerr << "Property Config [SHOW-SECONDS-OFFLINE] not found!!.. Default set "<<DEFAULT_OFFLINE_SECONDS<<endl;
        this->show_seconds_offline=DEFAULT_OFFLINE_SECONDS;
    }
    else
    {
        this->status_file=pstream->getValue("STATUS-FILE");
        if (pstream->Status()!=P_NO_ERROR)
        {
            cerr << "Property Config [STATUS-FILE] not found!!.. Default path "<<DEFAULT_STATUS_FILE<<" sec.."<<endl;
            this->status_file=DEFAULT_STATUS_FILE;
        }
        else
        {
            cerr << "Property Config [STATUS-FILE] = "<<this->status_file<<endl;
        }

        this->offline_timeout=atoll(pstream->getValue("OFFLINE-TIMEOUT").c_str());
        if (pstream->Status()!=P_NO_ERROR && this->offline_timeout>0)
        {
            cerr << "Property Config [OFFLINE-TIMEOUT] not found!!.. Default set "<<DEFAULT_OFFLINE_TIMEOUT<<" sec.."<<endl;
            this->offline_timeout=DEFAULT_OFFLINE_TIMEOUT;
        }
        else
        {
            cerr << "Property Config [OFFLINE-TIMEOUT] = "<<this->offline_timeout<<endl;
        }

        this->thread_frec=atoll(pstream->getValue("THREAD-FRECUENCY").c_str());
        if (pstream->Status()!=P_NO_ERROR && this->thread_frec>0)
        {
            cerr << "Property Config [THREAD-FRECUENCY] not found!!.. Default set "<<DEFAULT_THREAD_FRECUENCY<<" sec.."<<endl;
            this->offline_timeout=DEFAULT_THREAD_FRECUENCY;
        }
        else
        {
            cerr << "Property Config [THREAD-FRECUENCY] = "<<this->thread_frec<<endl;
        }

        string showSec=pstream->getValue("SHOW-SECONDS-OFFLINE");
        cerr << "show second = "<<showSec<<endl;
        if (pstream->Status()!=P_NO_ERROR && showSec.compare("n")==0)
        {
            this->show_seconds_offline=false;
            cerr << "Property Config [SHOW-SECONDS-OFFLINE] = "<<this->show_seconds_offline<<endl;
        }
        else
        {
            cerr << "Property Config [SHOW-SECONDS-OFFLINE::DEFAULT] = "<<DEFAULT_OFFLINE_SECONDS<<endl;
            this->show_seconds_offline=DEFAULT_OFFLINE_SECONDS;
        }
    }

    delete pstream;

    ui->setupUi(this);
    ui->pushButtonKO->setEnabled(false);

    int ret=pthread_create(&this->nthread,NULL,&start_notify_thread,(void *)this);
    if (ret!=0) {
        QMessageBox::information(0, "error", "Thread error");
        exit(1);
    }
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}





void MainWindow::on_pushButtonKO_clicked()
{
    ui->labelKO->setText("");
    ui->pushButtonKO->setEnabled(false);
}

void MainWindow::on_pushButton_clicked()
{
    exit(0);
}
