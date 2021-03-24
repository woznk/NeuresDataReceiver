#ifndef CRICKETAPPLICATION_H
#define CRICKETAPPLICATION_H

#include <QMainWindow>
#include <QTime>
#include <QElapsedTimer>
#include <QtCore>
#include <QDebug>
#include <QSettings>
#include <QDialog>

#include "hidapi.h"
#include "cricketpressure.h"


namespace Ui {
class CricketApplication;
}

class CricketApplication : public QMainWindow
{
    Q_OBJECT

public:
    explicit CricketApplication(QWidget *parent = nullptr);
    ~CricketApplication();

    hid_device *connected_device;
    CricketPressure *cp;


    int pressure1 = 150;//255;
    int pressure2 = 200;//690;
    int pressure3 = 250;//540;

public slots:
    void pressureUpdate(int p1, int p2, int p3);


private slots:

    void on_btnStart_clicked();
    void on_btnPause_clicked();
    void on_btnKoniec_clicked();
    void on_btnClose_clicked();
    void on_btnPRESS_clicked();


    void rxHandler();
    void timeEnd();
    void updataDisplayTime();
    void updateBars();              //U2021-03-17.1

    void on_timerEdit_userTimeChanged(const QTime &time);



private:
    Ui::CricketApplication *ui;

    int outdata = 0;
    float outdata1 = 0;               //U2021-03-17.1
    float outdata2 = 0;               //U2021-03-17.1
    float outdata3 = 0;               //U2021-03-17.1
    unsigned char outBuffer[65];    //outBuffer[0] is for endpoint number
    unsigned char inBuffer[64];
    int io_result = 0;

    QTimer *POMIARtimer;
    QTimer *USBtimer;
    QTimer *timer_1s;
    QTimer *timer_bar;              //U2021-03-17.1


    bool start = false;

};

#endif // CRICKETAPPLICATION_H
