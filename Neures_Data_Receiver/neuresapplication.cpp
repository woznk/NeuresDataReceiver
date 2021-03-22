#include "neuresapplication.h"
#include "ui_neuresapplication.h"

#include <QtWidgets>
#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QElapsedTimer>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QCoreApplication>
#include <QApplication>
#include <QDebug>

#include "neuresapplication.h"
#include "ui_neuresapplication.h"

#define CONNECTED "Neures USB Data Receiver: Connected"
#define NOT_CONNECTED "Neures USB Data Receiver: Not Connected!"

NeuresApplication::NeuresApplication(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NeuresApplication)
{
    ui->setupUi(this);
    setWindowTitle(NOT_CONNECTED);
    CONNECTtimer = new QTimer(this);
    connect(CONNECTtimer, SIGNAL(timeout()), this, SLOT(to_Connect()));
    connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(on_btnClose_clicked()));
    CONNECTtimer->start(1000);
}

NeuresApplication::~NeuresApplication()
{
    delete ui;
}

//----------------------------------

void NeuresApplication::to_Connect()
{
    struct hid_device_info *cur_dev;
    QString s;
    CONNECTtimer->stop();
    iDevice = 1;
    bool key = false;

    hid_free_enumeration(devices);
    devices = hid_enumerate(0x0, 0x0);
    cur_dev = devices;

    while (cur_dev){
        if(cur_dev->vendor_id==0x1cbe || cur_dev->product_id==0x2be){
            connected_device = hid_open_path(cur_dev->path);
            hid_set_nonblocking(connected_device,1);
            setWindowTitle(CONNECTED);
            cur_dev = NULL;
            iDevice = countDevices;
            setWindowTitle(CONNECTED);
            key = true;

        } else {
            if (cur_dev != NULL)
                cur_dev = cur_dev->next;
        }
        iDevice++;
    }
    if(!key){
        if(QMessageBox::question(this,"Brak klcza USB Neures Data Receiver","Podłącz klucz USB Neures Data Receiver \nPraca OffLine (przegląd bazy danych)","Podłącz","Offline") == 0){
            CONNECTtimer->start(100);
        }
    } else {
        ca = new CricketApplication();
        ca->connected_device = connected_device;
        ca->show();
        close();
    }
}

void NeuresApplication::on_btnClose_clicked()
{
    close();
}
