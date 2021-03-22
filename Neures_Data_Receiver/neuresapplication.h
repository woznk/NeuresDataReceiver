#ifndef NEURESAPPLICATION_H
#define NEURESAPPLICATION_H

#include <QMainWindow>
#include <QTime>
#include <QElapsedTimer>
#include <QtCore>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QDialog>

#include "hidapi.h"
#include "cricketapplication.h"


QT_BEGIN_NAMESPACE
namespace Ui { class NeuresApplication; }
QT_END_NAMESPACE

class NeuresApplication : public QMainWindow
{
    Q_OBJECT

public:
    NeuresApplication(QWidget *parent = nullptr);
    ~NeuresApplication();  

private slots:
    void to_Connect();

    void on_btnClose_clicked();

private:
    Ui::NeuresApplication *ui;
    CricketApplication *ca;

    QTimer *CONNECTtimer;

    struct hid_device_info *devices;
    hid_device *connected_device;
    int iDevice=0;
    int countDevices = 99;
};
#endif // NEURESAPPLICATION_H
