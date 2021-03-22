#include "cricketapplication.h"
#include "ui_cricketapplication.h"
#include "neuresapplication.h"


#include <QDebug>
#include <QTime>

//## NEURES HID CTRL  I N S T R U C T I O N    D E F I N I T I O N S ###########
typedef enum {
    CID_INIT            = 0x00,
    CID_TRANSFER_ON     = 0x01,     // start data transfer
    CID_TRANSFER_OFF    = 0x02,     // stop data transfer
    CID_READY           = 0x03,     // activate board
    CID_UNREADY         = 0x04,     // deactivate board
    CID_PREASSURE1      = 0x05,     // set preassure1
    CID_PREASSURE2      = 0x06,     // set preassure2
    CID_PREASSURE3      = 0x07,     // set preassure3
    CID_START           = 0x08,     // start
    CID_RESTART         = 0x09,     // restart
    CID_DATA            = 0x11,     // pressure data
    CID_TIME_START      = 0x12,     // time start
//*U3*/    CID_GET_PRESSURE    = 0x0A,     // get pressure
    CID_POTI            = 0x60,     // get analog value
    ID_MESSAGE          = 0x80,
    ID_ERROR            = 0xFF
}COMMANDS;

//--------------------------------------
CricketApplication::CricketApplication(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CricketApplication)
{
//    setWindowTitle("Neures Polska - Cricket 1.0.1");
//    qDebug() << parent;
    ui->setupUi(this);
    USBtimer = new QTimer;
    timer_1s = new QTimer;
    POMIARtimer = new QTimer;
    timer_bar = new QTimer;         //U2021-03-17.1

    connect(ui->btnSTART, SIGNAL(clicked()), this, SLOT(on_btnStart_clicked()));
    connect(ui->btnKONIEC, SIGNAL(clicked()), this, SLOT(on_btnKoniec_clicked()));
    connect(ui->btnCLOSE, SIGNAL(clicked()), this, SLOT(on_btnClose_clicked()));
    connect(ui->timerEdit, SIGNAL(userTimeChanged(QTime)), this, SLOT(on_timerEdit_userTimeChanged(QTime)));

    connect(USBtimer, SIGNAL(timeout()), this, SLOT(rxHandler()));
    connect(POMIARtimer, SIGNAL(timeout()), this, SLOT(timeEnd()));
    connect(timer_1s,SIGNAL(timeout()), this, SLOT(updataDisplayTime()));
    connect(timer_bar,SIGNAL(timeout()), this, SLOT(updateBars()));     //U2021-03-17.1


    QPalette palette = ui->progressObwod1->palette();
    palette.setColor(QPalette::Highlight, Qt::red);

//    ui->progressObwod1->setPalette(palette);
//    ui->progressObwod2->setPalette(palette);
//    ui->progressObwod3->setPalette(palette);
//    ui->progressObwod1->setStyleSheet("QProgressBar::chunk {background: rgb(0, 0, 255, 60%);}");
//    ui->progressObwod1->show();


}

//------------------------------------
CricketApplication::~CricketApplication()
{
    delete ui;
}

void CricketApplication::pressureUpdate(int p1, int p2, int p3)
{
    pressure1 = p1;
    pressure2 = p2;
    pressure3 = p3;
}

//------------------------------------
void CricketApplication::on_btnClose_clicked()
{
    outBuffer[1] = 3;
    outBuffer[2] = CID_UNREADY;
    io_result = hid_write(connected_device, outBuffer , 65);
    outBuffer[1] = 0;
    qDebug() <<  "Exit";
    rxHandler();

    close();
}

//----------------------------------
void CricketApplication::on_btnStart_clicked()
{
    USBtimer->start(10);

    outBuffer[1] = 3;
    outBuffer[2] = CID_READY;
    outBuffer[3] = pressure1>>8;                     //////////////////////
    outBuffer[4] = pressure1;                        //////////////////////
    outBuffer[5] = pressure2>>8;                    //////////////////////
    outBuffer[6] = pressure2;                       //////////////////////
    outBuffer[7] = pressure3>>8;                    //////////////////////
    outBuffer[8] = pressure3;                       //////////////////////

    qDebug() << "Start";

    io_result = hid_write(connected_device, outBuffer , 65);
    outBuffer[1] = 0;

    rxHandler();
}

//----------------------------------
void CricketApplication::on_btnPause_clicked()
{

}

//----------------------------------
void CricketApplication::on_btnKoniec_clicked()
{
    outBuffer[1] = 3;
    outBuffer[2] = CID_UNREADY;
    io_result = hid_write(connected_device, outBuffer , 65);
    outBuffer[1] = 0;

    rxHandler();
}

//----------------------------------
void CricketApplication::rxHandler()
{
    QString s = "";
    io_result = hid_read(connected_device, inBuffer, 64);
    if (io_result == 64){
//        qDebug() << "Test";
        switch (inBuffer[1] - ID_MESSAGE)	//---------------------- MESSAGE
        {
            case CID_TRANSFER_OFF:
                        POMIARtimer->stop();
                        timer_1s->stop();
//                      if(ui->btnPAUSE->isEnabled()){
                        ui->timerEdit->setTime(ui->timerEdit->time().addSecs(-(QTime(0,0,0).secsTo(ui->timerEdit->time()))));
//                      };
                        if(QTime(0,0,0).secsTo(ui->timerEdit->time())<=0) {
                          ui->btnSTART->setEnabled(false);
//                        ui->btnSTART->setEnabled(true);
                        } else {
                        };
                        ui->btnKONIEC->setEnabled(false);
                        ui->btnPAUSE->setText("Pauza");
                        ui->btnPAUSE->setEnabled(false);
                        ui->btnPRESS->setEnabled(true);
                        ui->timerEdit->setReadOnly(false);
                        ui->progressObwod1->setValue(0);
                        ui->progressObwod2->setValue(0);
                        ui->progressObwod3->setValue(0);
/*U4001*/               qDebug() << "UnReady";
                        break;
            case CID_READY:
   //                     qDebug() << ((inBuffer[2]<<8)|inBuffer[3]);
   //                     qDebug() << ((inBuffer[4]<<8)|inBuffer[5]);
   //                     qDebug() << ((inBuffer[6]<<8)|inBuffer[7]);
                        ui->btnSTART->setEnabled(false);
                        ui->btnKONIEC->setEnabled(true);
                        ui->btnPRESS->setEnabled(false);
                        ui->timerEdit->setReadOnly(true);
/*U4001*/               qDebug() <<"Ready";
                        break;
            case CID_UNREADY:
                        POMIARtimer->stop();
                        timer_1s->stop();
                  //      if(ui->btnPAUSE->isEnabled()){
                            ui->timerEdit->setTime(ui->timerEdit->time().addSecs(-(QTime(0,0,0).secsTo(ui->timerEdit->time()))));
                  //      };
                        if(QTime(0,0,0).secsTo(ui->timerEdit->time())<=0) {
                           ui->btnSTART->setEnabled(false);
                  //         ui->btnSTART->setEnabled(true);
                        } else {
                        };
                        ui->btnKONIEC->setEnabled(false);
                        ui->btnPAUSE->setText("Pauza");
                        ui->btnPAUSE->setEnabled(false);
                        ui->btnPRESS->setEnabled(true);
                        ui->timerEdit->setReadOnly(false);
                        ui->progressObwod1->setValue(0);
                        ui->progressObwod2->setValue(0);
                        ui->progressObwod3->setValue(0);
/*U4001*/               qDebug() << "UnReady";
                        break;
            case CID_START:
//                        timer_1s->start(1000);
                        ui->btnPAUSE->setEnabled(true);
                        ui->btnPAUSE->setText("Pauza");
/*U4001*/               qDebug() << inBuffer[1];
//                        POMIARtimer->start(QTime(0,0,0).msecsTo(ui->timerEdit->time()));
                        break;
            case CID_RESTART:
                        timer_1s->stop();
                        POMIARtimer->stop();
                        ui->btnPAUSE->setEnabled(true);
                        ui->btnPAUSE->setText("Wznów");
                        ui->btnSTART->setEnabled(false);
                        ui->btnKONIEC->setEnabled(true);
                        ui->progressObwod1->setValue(0);
                        ui->progressObwod2->setValue(0);
                        ui->progressObwod3->setValue(0);
                  //      ui->btnPAUSE->setText("Pauza ");
                  //      disconnect(ui->timerEdit, SIGNAL(userTimeChanged(QTime)), this, SLOT(on_timerEdit_userTimeChanged(QTime)));
                  //      ui->timerEdit->setTime(ui->timerEdit->time().addSecs(-(QTime(0,0,0).secsTo(ui->timerEdit->time()))));
                  //      connect(ui->timerEdit, SIGNAL(userTimeChanged(QTime)), this, SLOT(on_timerEdit_userTimeChanged(QTime)));
/*U4001*/               qDebug() << inBuffer[1];
                        break;
           case CID_TIME_START:
                       POMIARtimer->start(QTime(0,0,0).msecsTo(ui->timerEdit->time()));
                       timer_1s->start(1000);
//                       start = true;
/*U4001*/               qDebug() << inBuffer[1];
                       break;

//*U3*/      case CID_GET_PRESSURE:
//*U3*/            //update pressure

//*U3*/                 break;

/*U1*/      case CID_DATA:
/*U1*/                          s += QString("%1").arg(inBuffer[0],3,10);
/*U1*/                          s += QString("%1").arg(inBuffer[1],3,16);
/*U1*/                          s += QString("%1").arg(inBuffer[2],3,10);                   // number of sensors
/*U1*/                          // loop time
/*U1*/                          outdata = 0;
/*U1*/                          outdata = (inBuffer[3]<<8)|inBuffer[4];
/*U1*/                          s += QString("%1").arg(outdata,6,10);
/*U1*/                          s += "      ";
/*U1*/                          // sensors
/*U1*/                          for(int i=0; i<inBuffer[2]; i++) {
/*U1*/                             s += "   S. no:  ";
/*U1*/                             s += QString("%1").arg(inBuffer[i*5+5],4,16);            // sensor number
                    //                qDebug() << s;
/*U1*/                             outdata = 0;
/*U1*/                             outdata = ((inBuffer[i*5+6]<<8)|inBuffer[i*5+7]);
/*U1*/                             s += QString("%1").arg(outdata,6,10);
/*U1*/                             s += "       ";
                   //                qDebug() << s;
                   //                qDebug() << i+1;
/*U2*/                             switch (i) {
/*U2*/                                  case 0:
//R2021-03-16                                            if(pressure1==0){
/*U2021-03-18.1*/                           outdata1 = outdata;
/*U2021-03-16*/                             if(outdata>=pressure1){
                                                ui->progressObwod1->setValue(100);
                                            } else {
/*U2*/                                          ui->progressObwod1->setValue((int)((float)outdata/pressure1)*100);
                                            }
//R2021-03-07                                            qDebug() << outdata;

                                     //       qDebug() << ((float)outdata/pressure1)*100;
                                     //       qDebug() << pressure1;
/*U2*/                                      break;
/*U2*/                                  case 1:
//R2021-03-16                                            if(pressure2==0){
/*U2021-03-18.1*/                           outdata2 = outdata;
/*U2021-03-16*/                             if(outdata>=pressure2){
                                                ui->progressObwod2->setValue(100);
                                            } else {
//R2021-03-07                                            qDebug() << outdata;
/*U2*/                                          ui->progressObwod2->setValue((int)((float)outdata/pressure2)*100);
                                            }
/*U2*/                                      break;
/*U2*/                                  case 2:
//R2021-03-07                                            qDebug() << outdata;
//R2021-03-16                                            if(pressure3==0){
/*U2021-03-18.1*/                           outdata3 = outdata;
/*U2021-03-16*/                             if(outdata>=pressure3){
/*U2*/                                          ui->progressObwod3->setValue(100);
                                            } else {
                                                ui->progressObwod3->setValue((int)((float)outdata/pressure3)*100);
                                            }
/*U2*/                                      break;
/*U2*/                             }
/*U1*/                          }
                                qDebug() << s;
        }
    }
}

//--------------------------------------------------------
void CricketApplication::timeEnd()
{
    outBuffer[1] = 3;
    outBuffer[2] = CID_UNREADY;
    io_result = hid_write(connected_device, outBuffer , 65);
    qDebug() << io_result;
    io_result = hid_write(connected_device, outBuffer , 65);
    outBuffer[1] = 0;

    rxHandler();

    ui->btnSTART->setEnabled(false);
    ui->btnKONIEC->setEnabled(false);
    ui->btnPAUSE->setText("Pauza");
    ui->btnPAUSE->setEnabled(false);
    timer_1s->stop();
    disconnect(ui->timerEdit, SIGNAL(userTimeChanged(QTime)), this, SLOT(on_timerEdit_userTimeChanged(QTime)));
    ui->timerEdit->setTime(ui->timerEdit->time().addSecs(-(QTime(0,0,0).secsTo(ui->timerEdit->time()))));
    connect(ui->timerEdit, SIGNAL(userTimeChanged(QTime)), this, SLOT(on_timerEdit_userTimeChanged(QTime)));
//    qDebug() << "end time";
//    qDebug() << QTime(0,0,0).msecsTo(ui->timerEdit->time());
}

//--------------------------------------------------------
void CricketApplication::updataDisplayTime()
{
   disconnect(ui->timerEdit, SIGNAL(userTimeChanged(QTime)), this, SLOT(on_timerEdit_userTimeChanged(QTime)));
   ui->timerEdit->setTime(ui->timerEdit->time().addSecs(-1));
   connect(ui->timerEdit, SIGNAL(userTimeChanged(QTime)), this, SLOT(on_timerEdit_userTimeChanged(QTime)));
}

void CricketApplication::updateBars()           //U2021-03-17.1
{                                               //U2021-03-17.1
                                                //U2021-03-17.1
}                                               //U2021-03-17.1

//--------------------------------------------------------
void CricketApplication::on_timerEdit_userTimeChanged(const QTime &time)
{
//     qDebug() << "set time";
//     qDebug() << QTime(0,0,0).msecsTo(ui->timerEdit->time());
     if(QTime(0,0,0).secsTo(time)<=0){
        ui->btnSTART->setEnabled(false);
     } else {
        ui->btnSTART->setEnabled(true);
     }
}

void CricketApplication::on_btnPRESS_clicked()
{
    //  get pressure from CRICKET
//*U3*/     outBuffer[1] = 3;
//*U3*/     outBuffer[2] = CID_GET_PRESSURE;
//*U3*/     io_result = hid_write(connected_device, outBuffer , 65);
//*U3*/     outBuffer[1] = 0;

//*U3*/     rxHandler();


     cp = new CricketPressure(this);
     cp->setPress1(pressure1);
     cp->setPress2(pressure2);
     cp->setPress3(pressure3);
     cp->setModal(true);
     cp->exec();

//     qDebug() << cp->result();

     if(cp->result() == 1){
         pressure1 = cp->getPress1();
         pressure2 = cp->getPress2();
         pressure3 = cp->getPress3();

 //        saveSettings();
     }


}

