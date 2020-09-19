#ifndef HID_DATA_RECEIVER_H
#define HID_DATA_RECEIVER_H

#include <QMainWindow>
#include <QTime>
#include <QElapsedTimer>
#include <QtCore>
#include <QDebug>
#include <QSettings>

#include "hidapi.h"
#include "pressgraph.h"


QT_BEGIN_NAMESPACE
namespace Ui { class HID_Data_Receiver; }
QT_END_NAMESPACE

class HID_Data_Receiver : public QMainWindow
{
    Q_OBJECT

public:
    HID_Data_Receiver(QWidget *parent = nullptr);
    ~HID_Data_Receiver();

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:

private slots:

    void on_btnScan();
    void on_btnConnect();
    void to_Connect();
    void to_Scan();
    void on_btnLED();
    void on_btnTransfer();
    void on_btnSave();
    void on_btnGraph();
    void en_btnGraph();

    void rxHandler();

    void on_sbPreassure_valueChanged(int Preassure);
    void on_sbPreassure2_valueChanged(int Preassure2);
    void on_sbPreassure3_valueChanged(int Preassure3);

    void on_checkmmHg(int mmHgstate);

private:
    Ui::HID_Data_Receiver *ui;

    struct hid_device_info *devices;
    hid_device *connected_device;
    int iDevice;
    int countDevices = 99;

    union rd{
        uint8_t inbyte[2];
        uint16_t outbyte;
    } rdane;

    int outdata;

    unsigned char outBuffer[65];    //outBuffer[0] is for endpoint number
    unsigned char inBuffer[64];
    int io_result;
    bool pump = false;
    double Time;
    bool test = false;

    QTimer *RXtimer;
    QTimer *SCANtimer;
    QTimer *CONNECTtimer;

    QElapsedTimer *etimer;

    pressGraph *PressGraph;
    QString m_sSettingsFile;

    void loadSettings();

    void saveSettings();

};
#endif // HID_DATA_RECEIVER_H
