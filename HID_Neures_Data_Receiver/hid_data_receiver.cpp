#include <QtWidgets>
#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QElapsedTimer>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QCoreApplication>
#include <QApplication>

#include "hid_data_receiver.h"
#include "ui_hid_data_receiver.h"
#include "pressgraph.h"

#define CONNECTED "Neures USB Data Receiver: Connected"
#define NOT_CONNECTED "Neures USB Data Receiver: Not Connected!"

//## NEURES HID CTRL  I N S T R U C T I O N    D E F I N I T I O N S ###########
typedef enum {
    CID_INIT            = 0x00,
    CID_ON              = 0x01,     // start data transfer
    CID_OFF             = 0x02,     // stop data transfer
    CID_PUMP_ON         = 0x03,     // activate board
    CID_PUMP_OFF        = 0x04,     // deactivate board
    CID_PREASSURE1      = 0x05,     // set preassure1
    CID_PREASSURE2      = 0x06,     // set preassure2
    CID_PREASSURE3      = 0x07,     // set preassure3
    CID_START           = 0x08,     // start
    CID_STOP            = 0x09,     // stop

    CID_DATA            = 0x11,     // pressure data

    CID_POTI            = 0x60,     // get analog value
    ID_MESSAGE          = 0x80,
    ID_ERROR            = 0xFF
}COMMANDS;

HID_Data_Receiver::HID_Data_Receiver(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HID_Data_Receiver)
{
    ui->setupUi(this);
    setWindowTitle(NOT_CONNECTED);

     m_sSettingsFile = QApplication::applicationDirPath().left(1) + ":\\Qt_projekty\\HID_Neures_Data_Receiver\\demosettings.ini";

    devices = 0;
    outBuffer[0] = 0;   // endpoint number

    ui->btnLED->setVisible(false);
    ui->btnLED->setEnabled(false);
    ui->btnSend->setVisible(false);
    ui->btnSend->setEnabled(false);
    ui->teInHEX->setVisible(false);
    ui->lbObwod_I->setVisible(false);
    ui->sbPreassure1->setVisible(false);
    ui->sbPreassure1->setEnabled(false);
    ui->lbObwod_II->setVisible(false);
    ui->sbPreassure2->setVisible(false);
    ui->sbPreassure2->setEnabled(false);
    ui->lbObwod_III->setVisible(false);
    ui->sbPreassure3->setVisible(false);
    ui->sbPreassure3->setEnabled(false);
    ui->btnSave->setVisible(false);
    ui->btnSave->setEnabled(false);
    ui->btnGraph->setVisible(false);
    ui->btnGraph->setEnabled(false);   
    ui->checkmmHg->setVisible(false);
    ui->checkmmHg->setEnabled(false);
    ui->l1->setVisible(false);
    ui->l2->setVisible(false);
    ui->l3->setVisible(false);

//    static QTime time(QTime::currentTime());

    etimer = new QElapsedTimer;

    RXtimer = new QTimer(this);
    SCANtimer = new QTimer(this);
    CONNECTtimer = new QTimer(this);

    connect(RXtimer, SIGNAL(timeout()), this, SLOT(rxHandler()));
    connect(ui->btnConnect, SIGNAL(clicked()), this, SLOT(on_btnConnect()));
    connect(ui->btnScan, SIGNAL(clicked()), this, SLOT(on_btnScan()));
    connect(ui->btnSend, SIGNAL(clicked()), this, SLOT(on_btnTransfer()));
    connect(ui->btnLED, SIGNAL(clicked()), this, SLOT(on_btnLED()));
    connect(ui->sbPreassure1, SIGNAL(valueChanged(int)),this, SLOT(on_sbPreassure_valueChanged(int)));
    connect(ui->sbPreassure2, SIGNAL(valueChanged(int)),this, SLOT(on_sbPreassure2_valueChanged(int)));
    connect(ui->sbPreassure3, SIGNAL(valueChanged(int)),this, SLOT(on_sbPreassure3_valueChanged(int)));
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(on_btnSave()));
    connect(ui->btnGraph,SIGNAL(clicked()), this, SLOT(on_btnGraph()));
    connect(ui->checkmmHg, SIGNAL(stateChanged(int)), this, SLOT(on_checkmmHg(int)));

    connect(SCANtimer, SIGNAL(timeout()), this, SLOT(on_btnScan()));
    connect(CONNECTtimer, SIGNAL(timeout()), this, SLOT(to_Connect()));

    loadSettings();

    CONNECTtimer->start(1000);
}
//    on_btnScan();

//
//------------------------------------
//
HID_Data_Receiver::~HID_Data_Receiver()
{

    outBuffer[1] = 3;
    outBuffer[2] = CID_PUMP_OFF;
    io_result = hid_write(connected_device, outBuffer , 65);
    outBuffer[1] = 3;
    outBuffer[2] = CID_OFF;
    io_result = hid_write(connected_device, outBuffer , 65);
    saveSettings();
    delete ui;
}

void HID_Data_Receiver::closeEvent(QCloseEvent *event)
{
    if(!ui->btnGraph->isEnabled()){
        PressGraph->close();
    }
    outBuffer[1] = 3;
    outBuffer[2] = CID_PUMP_OFF;
    io_result = hid_write(connected_device, outBuffer , 65);
    outBuffer[1] = 3;
    outBuffer[2] = CID_OFF;
    io_result = hid_write(connected_device, outBuffer , 65);
    saveSettings();
    event->accept();
}

void HID_Data_Receiver::en_btnGraph()
{
    ui->btnGraph->setEnabled(true);
}

//----------------------------------

void HID_Data_Receiver::on_btnScan()
{
    struct hid_device_info *cur_dev;
    QString s;

    ui->teDevices->clear();
    if(devices){
        hid_free_enumeration(devices);
    }
    devices = hid_enumerate(0x0, 0x0);
    cur_dev = devices;

    iDevice = 1;
    while (cur_dev) {
        s  = QString("%1").arg(iDevice,0,16);
        s += ": ";
//        s += QString("%1").arg(cur_dev->vendor_id,7,16);
//        s += "  ";
//        s += QString("%1").arg(cur_dev->product_id,7,16);
        s += "  ";
        s += QString::fromWCharArray(cur_dev->manufacturer_string);
        s += "  ";
        s += QString::fromWCharArray(cur_dev->product_string);

        ui->teDevices->append(s);
        iDevice++;
        countDevices = iDevice;
        cur_dev = cur_dev->next;
    }
}

//----------------------------------

void HID_Data_Receiver::on_btnConnect()
{
    struct hid_device_info *cur_dev;
    QString s;


    if(ui->btnConnect->text() == "Connect USB"){
        if(ui->sbDeviceNr->value() == 0){
            ui->edDevice->setText("Please choose HID");
            return;
        }

        hid_free_enumeration(devices);
        devices = hid_enumerate(0x0, 0x0);
        cur_dev = devices;
        while (cur_dev){

            for (iDevice = 1 ; iDevice <= countDevices; iDevice++){
                if( iDevice == ui->sbDeviceNr->value()){
                    s  = QString("%1").arg(iDevice,0,16);
                    s += ": ";
//                    s += QString("%1").arg(cur_dev->vendor_id,4,16);
//                    s += "  ";
//                    s += QString("%1").arg(cur_dev->product_id,4,16);
                    s += "  ";
                    s += QString::fromWCharArray(cur_dev->manufacturer_string);
                    s += "  ";
                    s += QString::fromWCharArray(cur_dev->product_string);

                    ui->edDevice->setText(s);

                    connected_device = hid_open_path(cur_dev->path);
                    hid_set_nonblocking(connected_device,1);
                    ui->btnConnect->setText("Disconnect USB");
                    setWindowTitle(CONNECTED);
                    ui->btnSend->setEnabled(true);
                    ui->btnSend->setVisible(true);
                    ui->sbDeviceNr->setValue(0);
                    cur_dev = NULL;
                    iDevice = countDevices;
                    ui->btnConnect->setText("Disconnect USB");
                    setWindowTitle(CONNECTED);
                    ui->btnSend->setEnabled(true);
                    ui->btnSend->setVisible(true);
                    ui->btnLED->setEnabled(true);
                    ui->btnLED->setVisible(true);
                    ui->teInHEX->clear();
                    ui->teInHEX->setEnabled(true);
                    ui->teInHEX->setVisible(true);
                    ui->lbObwod_I->setVisible(true);
                    ui->sbPreassure1->setVisible(true);
                    ui->sbPreassure1->setEnabled(true);
                    ui->lbObwod_II->setVisible(true);
                    ui->sbPreassure2->setVisible(true);
                    ui->sbPreassure2->setEnabled(true);
                    ui->lbObwod_III->setVisible(true);
                    ui->sbPreassure3->setVisible(true);
                    ui->sbPreassure3->setEnabled(true);
                    ui->btnSave->setVisible(true);
                    ui->btnSave->setEnabled(true);
                    ui->btnGraph->setVisible(true);
                    ui->btnGraph->setEnabled(true);
                    ui->checkmmHg->setVisible(true);
                    ui->checkmmHg->setEnabled(true);
                    if(ui->checkmmHg->isChecked()) {
                       ui->l1->setVisible(true);
                       ui->l2->setVisible(true);
                       ui->l3->setVisible(true);
                    }


                    RXtimer->start(10);
                }
                else{
                    if (cur_dev != NULL)
                        cur_dev = cur_dev->next;
                }
            }//for (iDevice = 1 ; iDevice <= countDevices ...
        }//while (cur_dev){
            on_btnScan();
    }//if(ui->btnConnect->text() == "Connect")
    else {

        outBuffer[1] = 3;
        outBuffer[2] = CID_PUMP_OFF;
        io_result = hid_write(connected_device, outBuffer , 65);
        ui->btnLED->setText("Activate Board");
        outBuffer[1] = 3;
        outBuffer[2] = CID_OFF;
        io_result = hid_write(connected_device, outBuffer , 65);
        ui->btnSend->setText("Start Transfer USB");


        RXtimer->stop();
        //if(POTItimer != NULL)
        //    POTItimer->stop();      // TODO ;-)
        hid_close(connected_device);
        connected_device = NULL;
        ui->btnConnect->setText("Connect USB");
        setWindowTitle(NOT_CONNECTED);
        ui->btnSend->setEnabled(false);
        ui->btnSend->setVisible(false);
        ui->btnLED->setVisible(false);
        ui->btnLED->setEnabled(false);
        ui->edDevice->setText("Please choose HID");
        ui->teInHEX->clear();
        ui->teInHEX->setVisible(false);
        ui->lbObwod_I->setVisible(false);
        ui->sbPreassure1->setVisible(false);
        ui->sbPreassure1->setEnabled(false);
        ui->lbObwod_II->setVisible(false);
        ui->sbPreassure2->setVisible(false);
        ui->sbPreassure2->setEnabled(false);
        ui->lbObwod_III->setVisible(false);
        ui->sbPreassure3->setVisible(false);
        ui->sbPreassure3->setEnabled(false);
        ui->btnSave->setVisible(false);
        ui->btnSave->setEnabled(false);
        ui->btnGraph->setVisible(false);
        ui->btnGraph->setEnabled(false);
        ui->checkmmHg->setVisible(false);
        ui->checkmmHg->setEnabled(false);
        ui->l1->setVisible(false);
        ui->l2->setVisible(false);
        ui->l3->setVisible(false);
    }
    on_btnScan();
}

//----------------------------------

void HID_Data_Receiver::to_Connect()
{
    struct hid_device_info *cur_dev;
    QString s;

    iDevice = 1;

    hid_free_enumeration(devices);
    devices = hid_enumerate(0x0, 0x0);
    cur_dev = devices;

    while (cur_dev){
        if(cur_dev->vendor_id==0x1cbe || cur_dev->product_id==0x2be){
            s  = QString("%1").arg(iDevice,0,16);
            s += ": ";
//            s += QString("%1").arg(cur_dev->vendor_id,4,16);
//            s += "  ";
//            s += QString("%1").arg(cur_dev->product_id,4,16);
            s += "  ";
            s += QString::fromWCharArray(cur_dev->manufacturer_string);
            s += "  ";
            s += QString::fromWCharArray(cur_dev->product_string);

            ui->edDevice->setText(s);

            connected_device = hid_open_path(cur_dev->path);
            hid_set_nonblocking(connected_device,1);
            ui->btnConnect->setText("Disconnect USB");
            setWindowTitle(CONNECTED);
            ui->btnSend->setEnabled(true);
            ui->btnSend->setVisible(true);
            ui->sbDeviceNr->setValue(0);
            cur_dev = NULL;
            iDevice = countDevices;
 //           ui->btnConnect->setText("Disconnect USB");
            setWindowTitle(CONNECTED);
            ui->btnSend->setEnabled(true);
            ui->btnSend->setVisible(true);
            ui->btnLED->setEnabled(true);
            ui->btnLED->setVisible(true);
            ui->teInHEX->clear();
            ui->teInHEX->setEnabled(true);
            ui->teInHEX->setVisible(true);
            ui->lbObwod_I->setVisible(true);
            ui->sbPreassure1->setVisible(true);
            ui->sbPreassure1->setEnabled(true);
            ui->lbObwod_II->setVisible(true);
            ui->sbPreassure2->setVisible(true);
            ui->sbPreassure2->setEnabled(true);
            ui->lbObwod_III->setVisible(true);
            ui->sbPreassure3->setVisible(true);
            ui->sbPreassure3->setEnabled(true);
            ui->btnSave->setVisible(true);
            ui->btnSave->setEnabled(true);
            ui->btnGraph->setVisible(true);
            ui->btnGraph->setEnabled(true);
            ui->checkmmHg->setVisible(true);
            ui->checkmmHg->setEnabled(true);
            if(ui->checkmmHg->isChecked()) {
                ui->l1->setVisible(true);
                ui->l2->setVisible(true);
                ui->l3->setVisible(true);
            };

            CONNECTtimer->stop();
            RXtimer->start(10);
        } else {
            if (cur_dev != NULL)
                cur_dev = cur_dev->next;
        }
        iDevice++;
    }
}


void HID_Data_Receiver::to_Scan()
{

}

//----------------------------------

void HID_Data_Receiver::on_btnTransfer()
{
  if(ui->btnSend->text()=="Start Transfer USB"){
    outBuffer[1] = 3;
    outBuffer[2] = CID_ON;
    io_result = hid_write(connected_device, outBuffer , 65);
  } else {
    outBuffer[1] = 3;
    outBuffer[2] = CID_OFF;
    io_result = hid_write(connected_device, outBuffer , 65);
  }
  rxHandler();
}

//----------------------------------

void HID_Data_Receiver::on_btnLED()
{

  if(ui->btnLED->text()=="Activate Board"){
    outBuffer[1] = 3;
    outBuffer[2] = CID_PUMP_ON;
    if(ui->checkmmHg->isChecked()) {
      outBuffer[3] = (ui->sbPreassure1->value())*3>>8;                    //////////////////////
      outBuffer[4] = (ui->sbPreassure1->value())*3;                      //////////////////////
      outBuffer[5] = (ui->sbPreassure2->value())*3>>8;                    //////////////////////
      outBuffer[6] = (ui->sbPreassure2->value())*3;                      //////////////////////
      outBuffer[7] = (ui->sbPreassure3->value())*3>>8;                    //////////////////////
      outBuffer[8] = (ui->sbPreassure3->value())*3;                      //////////////////////
    } else {
      outBuffer[3] = ui->sbPreassure1->value()>>8;                    //////////////////////
      outBuffer[4] = ui->sbPreassure1->value() ;                      //////////////////////
      outBuffer[5] = ui->sbPreassure2->value()>>8;                    //////////////////////
      outBuffer[6] = ui->sbPreassure2->value() ;                      //////////////////////
      outBuffer[7] = ui->sbPreassure3->value()>>8;                    //////////////////////
      outBuffer[8] = ui->sbPreassure3->value() ;                      //////////////////////
    }
    io_result = hid_write(connected_device, outBuffer , 65);
    outBuffer[1] = 0;
  } else {
    outBuffer[1] = 4;
    outBuffer[2] = CID_PUMP_OFF;
    io_result = hid_write(connected_device, outBuffer , 65);
  }
  rxHandler();
}

//----------------------------------

void HID_Data_Receiver::rxHandler()
{
    QString s = "";

    io_result = hid_read(connected_device, inBuffer, 64);

    if (io_result == 64){

        switch (inBuffer[1] - ID_MESSAGE)	//---------------------- MESSAGE
        {
            case CID_ON:
                ui->btnSend->setText("Stop Transfer USB");
                break;
            case CID_OFF:
                ui->btnSend->setText("Start Transfer USB");
                break;
            case CID_PUMP_ON:
                ui->btnLED->setText("DeActivate Board");
                ui->sbPreassure1->setDisabled(true);
                ui->sbPreassure2->setDisabled(true);
                ui->sbPreassure3->setDisabled(true);
                // number of data
                s += QString("%1").arg(inBuffer[0],3,10);
                // command
                s += QString("%1").arg(inBuffer[1],3,16);
                if(ui->checkmmHg->isChecked()) {
                    outdata = ((inBuffer[2]<<8)|inBuffer[3]);       //pressure1
                    outdata /= 3;
                    s += QString("%1").arg(outdata,6,10);
                    s += " mmHg      ";
                    outdata = ((inBuffer[4]<<8)|inBuffer[5]);       //pressure1
                    outdata /= 3;
                    s += QString("%1").arg(outdata,6,10);
                    s += " mmHg      ";
                    outdata = ((inBuffer[6]<<8)|inBuffer[7] );       //pressure1
                    outdata /= 3;
                    s += QString("%1").arg(outdata,6,10);
                    s += " mmHg      ";
                } else {
                    outdata = ((inBuffer[2]<<8)|inBuffer[3]);       //pressure1
                    s += QString("%1").arg(outdata,6,10);
                    s += "      ";
                    outdata = ((inBuffer[4]<<8)|inBuffer[5]);       //pressure1
                    s += QString("%1").arg(outdata,6,10);
                    s += "      ";
                    outdata = ((inBuffer[6]<<8)|inBuffer[7]);       //pressure1
                    s += QString("%1").arg(outdata,6,10);
                    s += "      ";
                }
        /*U1*/  ui->teInHEX->append(s);             //->setText(s);
                break;
            case CID_PUMP_OFF:
                ui->btnLED->setText("Activate Board");
                ui->sbPreassure1->setDisabled(false);
                ui->sbPreassure2->setDisabled(false);
                ui->sbPreassure3->setDisabled(false);
                // number of data
        //        s += QString("%1").arg(inBuffer[0],3,10);
                // command
        //        s += QString("%1").arg(inBuffer[1],3,16);
                ui->btnLED->setDisabled(false);
                ui->btnSend->setDisabled(false);
                break;
            case CID_INIT:
                break;

            case CID_PREASSURE1:
                                // number of data
                                s += QString("%1").arg(inBuffer[0],3,10);
                                // command
                                s += QString("%1").arg(inBuffer[1],3,16);
                                outdata = ((inBuffer[2]<<8)|inBuffer[3]);       //pressure1
                                s += "    set Pressue1 to ";
                                if(ui->checkmmHg->isChecked()) {
                                    outdata /= 3;
                                    s += QString("%1").arg(outdata,6,10);
                                    s += " mmHg";
                                } else {
                                    s += QString("%1").arg(outdata,6,10);
                                }

/*U1*/                          ui->teInHEX->append(s);             //->setText(s);
                                break;

            case CID_PREASSURE2:
                                // number of data
                                s += QString("%1").arg(inBuffer[0],3,10);
                                // command
                                s += QString("%1").arg(inBuffer[1],3,16);
                                outdata = ((inBuffer[2]<<8)|inBuffer[3]);       //pressure1
                                s += "    set Pressue2 to ";
                                if(ui->checkmmHg->isChecked()) {
                                    outdata /= 3;
                                    s += QString("%1").arg(outdata,6,10);
                                    s += " mmHg";
                                } else {
                                    s += QString("%1").arg(outdata,6,10);
                                }
/*U1*/                          ui->teInHEX->append(s);             //->setText(s);
                                break;

            case CID_PREASSURE3:
                                // number of data
                                s += QString("%1").arg(inBuffer[0],3,10);
                                // command
                                s += QString("%1").arg(inBuffer[1],3,16);
                                outdata = ((inBuffer[2]<<8)|inBuffer[3]);       //pressure1
                                s += "    set Pressue3 to ";
                                if(ui->checkmmHg->isChecked()) {
                                    outdata /= 3;
                                    s += QString("%1").arg(outdata,6,10);
                                    s += " mmHg";
                                } else {
                                    s += QString("%1").arg(outdata,6,10);
                                }
/*U1*/                          ui->teInHEX->append(s);             //->setText(s);
                                break;

            case CID_START:
                                ui->btnLED->setText("RUNNING");
                                ui->btnLED->setDisabled(true);
                                ui->btnSend->setDisabled(true);
/*U1*/                          break;
/*U1*/      case CID_STOP:
/*U1*/                          ui->btnLED->setText("Activate Board");
/*U1*/                          ui->btnLED->setDisabled(false);
/*U1*/                          ui->btnSend->setDisabled(false);
/*U1*/                          ui->sbPreassure1->setDisabled(false);
/*U1*/                          ui->sbPreassure2->setDisabled(false);
/*U1*/                          ui->sbPreassure3->setDisabled(false);
/*U1*/                          break;


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
/*U1*/                             outdata = 0;
/*U1*/                             outdata = ((inBuffer[i*5+6]<<8)|inBuffer[i*5+7]);
/*U1*/                             if(ui->checkmmHg->isChecked()) {
/*U1*/                                outdata = outdata/3;
/*U1*/                                s += QString("%1").arg(outdata,6,10);
/*U1*/                                s += " mmHg";
/*U1*/                             } else {
/*U1*/                                s += QString("%1").arg(outdata,6,10);
/*U1*/                                s += "       ";
/*U1*/                             }
/*U1*/                          }
                        /*U1*/  ui->teInHEX->append(s);                                     //->setText(s);
                                break;
//------------------------------------------------------------------------------
            case CID_POTI:
//                ui->lbPoti->setText( QString("%1").arg(inBuffer[2]+256*inBuffer[3]));
//                if(ui->rbPotiAuto->isChecked()){
//                    for (int i = 0; i < (data.size() -1); i++){
//                        data[i] = data[i+1];
//                    }
//                    data[data.size()-1] = (inBuffer[2] + 256*inBuffer[3])
//                                        * ui->dSBCalibVal->value();
//                    ui->plot->graph(0)->setData(time,data);
//                    ui->plot->replot();
//                }
            break;
//            default:
//                edErrors->Text = "ERROR: unknown Device message";
        }

//R1      if (inBuffer[1]==0x91){                                       // if inBuffer[1] = 81 receive stream of data
//R1
//R1        s += QString("%1").arg(inBuffer[0],3,10);
//R1        s += QString("%1").arg(inBuffer[1],3,16);
//R1        s += QString("%1").arg(inBuffer[2],3,10);                   // number of sensors
//R1        // loop time
//R1        outdata = 0;
//R1        outdata = (inBuffer[3]<<8)|inBuffer[4];
//R1        s += QString("%1").arg(outdata,6,10);
//R1        s += "      ";
//R1        // sensors
//R1        for(int i=0; i<inBuffer[2]; i++) {
//R1           s += "   ";
//R1           s += QString("%1").arg(inBuffer[i*5+5],4,16);            // sensor number
//R1           outdata = 0;
//R1           outdata = ((inBuffer[i*5+6]<<8)|inBuffer[i*5+7]);
//R1           s += QString("%1").arg(outdata,6,10);
//R1           s += "       ";
//R1        }
//R1      }

//          static QTime time(QTime::currentTime());
//          double key =time.elapsed();
          // add to first graph...
          // Plot graph
/*
          if(PressGraph->checkOpenGraph()){
              if(!PressGraph->checkAddGraph()){
                  // add and setup graphs
                  for (int i=0;i<2*inBuffer[2];i++) {
                    PressGraph->addGraph(i);     //add new plot
                  }
                  PressGraph->addGraph(true);   //plot was added
                  etimer->start();

              } else {
                  // add points
                  double pressTime = etimer->elapsed();
                  for (int i=0; i<2*inBuffer[2];i++) {
                      PressGraph->addPressure(2*i, pressTime, ((inBuffer[i*5+6]<<8)|inBuffer[i*5+7]));
                      PressGraph->addPressure(2*i+1, pressTime, ((inBuffer[i*5+8]<<8)|inBuffer[i*5+9]));
                  }
              }


          }
*/
// /////////////////////////////////////////////////////////////////////////        }

//----------------------------------------

//      } else {                                                      // command exit code
          // number of data
//          s += QString("%1").arg(inBuffer[0],3,10);
          // command
//          s += QString("%1").arg(inBuffer[1],3,16);

//          for (int i = 2; i<=inBuffer[0]; i++){
//              s += QString("%1").arg(inBuffer[i],4,10);
//          }
//R1  }

//R1    ui->teInHEX->append(s);             //->setText(s);
/*
        switch (inBuffer[1] - ID_MESSAGE)	//---------------------- MESSAGE
        {
            case CID_ON:
                ui->btnSend->setText("Stop Transfer USB");
                break;
            case CID_OFF:
                ui->btnSend->setText("Start Transfer USB");
                break;
            case CID_PUMP_ON:
                ui->btnLED->setText("PUMP OFF");
                break;
            case CID_PUMP_OFF:
                ui->btnLED->setText("PUMP ON");
                break;
            case CID_INIT:
                break;
            case CID_PREASSURE:
                break;
//------------------------------------------------------------------------------
            case CID_POTI:
//                ui->lbPoti->setText( QString("%1").arg(inBuffer[2]+256*inBuffer[3]));
//                if(ui->rbPotiAuto->isChecked()){
//                    for (int i = 0; i < (data.size() -1); i++){
//                        data[i] = data[i+1];
//                    }
//                    data[data.size()-1] = (inBuffer[2] + 256*inBuffer[3])
//                                        * ui->dSBCalibVal->value();
//                    ui->plot->graph(0)->setData(time,data);
//                    ui->plot->replot();
//                }
            break;
//            default:
//                edErrors->Text = "ERROR: unknown Device message";
        }
              */
    }
}


void HID_Data_Receiver::on_sbPreassure_valueChanged(int Preassure)
{
    int tempPreassure = Preassure;
    if(ui->checkmmHg->isChecked()) {
        tempPreassure *= 3;
    }
    outBuffer[1] = 5;
    outBuffer[2] = CID_PREASSURE1;
    outBuffer[3] = tempPreassure>>8;                                    //////////////////////
    outBuffer[4] = tempPreassure;                                       //////////////////////

    io_result = hid_write(connected_device, outBuffer , 65);
    rxHandler();
}


void HID_Data_Receiver::on_sbPreassure2_valueChanged(int Preassure2)
{
    int tempPreassure = Preassure2;
    if(ui->checkmmHg->isChecked()) {
        tempPreassure *= 3;
    }
    outBuffer[1] = 5;
    outBuffer[2] = CID_PREASSURE2;
    outBuffer[3] = tempPreassure>>8;                                    //////////////////////
    outBuffer[4] = tempPreassure;                                       //////////////////////

    io_result = hid_write(connected_device, outBuffer , 65);
    rxHandler();
}

void HID_Data_Receiver::on_sbPreassure3_valueChanged(int Preassure3)
{
    int tempPreassure = Preassure3;
    if(ui->checkmmHg->isChecked()) {
        tempPreassure *= 3;
    }
    outBuffer[1] = 5;
    outBuffer[2] = CID_PREASSURE3;
    outBuffer[3] = tempPreassure>>8;                                    //////////////////////
    outBuffer[4] = tempPreassure;                                       //////////////////////

    io_result = hid_write(connected_device, outBuffer , 65);
    rxHandler();
}

void HID_Data_Receiver::on_checkmmHg(int mmHgstate)
{
    disconnect(ui->sbPreassure1, SIGNAL(valueChanged(int)),this, SLOT(on_sbPreassure_valueChanged(int)));
    disconnect(ui->sbPreassure2, SIGNAL(valueChanged(int)),this, SLOT(on_sbPreassure2_valueChanged(int)));
    disconnect(ui->sbPreassure3, SIGNAL(valueChanged(int)),this, SLOT(on_sbPreassure3_valueChanged(int)));

    if(mmHgstate == 2) {
        ui->l1->setEnabled(true);
        ui->sbPreassure1->setValue(ui->sbPreassure1->value()/3);
        ui->sbPreassure1->setMaximum(341);

        ui->l2->setEnabled(true);
        ui->sbPreassure2->setValue(ui->sbPreassure2->value()/3);
        ui->sbPreassure2->setMaximum(341);

        ui->l3->setEnabled(true);
        ui->sbPreassure3->setValue(ui->sbPreassure3->value()/3);
        ui->sbPreassure3->setMaximum(341);

    } else {
        ui->l1->setEnabled(false);
        ui->sbPreassure1->setMaximum(1024);
        ui->sbPreassure1->setValue(ui->sbPreassure1->value()*3);

        ui->l2->setEnabled(false);
        ui->sbPreassure2->setMaximum(1024);
        ui->sbPreassure2->setValue(ui->sbPreassure2->value()*3);


        ui->l3->setEnabled(false);
        ui->sbPreassure3->setMaximum(1024);
        ui->sbPreassure3->setValue(ui->sbPreassure3->value()*3);


    }

    connect(ui->sbPreassure1, SIGNAL(valueChanged(int)),this, SLOT(on_sbPreassure_valueChanged(int)));
    connect(ui->sbPreassure2, SIGNAL(valueChanged(int)),this, SLOT(on_sbPreassure2_valueChanged(int)));
    connect(ui->sbPreassure3, SIGNAL(valueChanged(int)),this, SLOT(on_sbPreassure3_valueChanged(int)));
}

void HID_Data_Receiver::loadSettings()
{
    QSettings settings("NeuresReceiver","Parameters");
    settings.beginGroup("Parameters");
    ui->sbPreassure1->setValue(settings.value("Pressure1").toInt());     //settings.setValue ("Pressure1",ui->sbPreassure->value());
    ui->sbPreassure2->setValue(settings.value("Pressure2").toInt());
    ui->sbPreassure3->setValue(settings.value("Pressure3").toInt());
    settings.endGroup();

}

void HID_Data_Receiver::saveSettings()
{
    QSettings settings("NeuserReceiver","Parameters");
    settings.beginGroup("Parameters");
    settings.setValue("Pressure1",ui->sbPreassure1->value());
    settings.setValue("Pressure2",ui->sbPreassure2->value());
    settings.setValue("Pressure3",ui->sbPreassure3->value());
    settings.endGroup();
}

void HID_Data_Receiver::on_btnSave()
{
    QString filter = "Dokument tekstowy (*.txt) ;; Wszystkie dokumenty (*.*)";
    QString file_name = QFileDialog::getSaveFileName(this,"Nazwa zbioru", QDir::homePath(), filter);
    QFile file(file_name);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {

    } else {
        QTextStream out(&file);
        QString text = ui->teInHEX->toPlainText();
        out << text;
        file.flush();
        file.close();
    }
}

void HID_Data_Receiver::on_btnGraph()
{
    PressGraph = new pressGraph;
    connect(PressGraph, SIGNAL(closeWin()), this, SLOT(en_btnGraph()));
    ui->btnGraph->setEnabled(false);
    PressGraph->openGraph(true);
    PressGraph->show();
}
