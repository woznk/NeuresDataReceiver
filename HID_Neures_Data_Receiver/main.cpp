#include "hid_data_receiver.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HID_Data_Receiver w;
    w.show();
    return a.exec();
}
