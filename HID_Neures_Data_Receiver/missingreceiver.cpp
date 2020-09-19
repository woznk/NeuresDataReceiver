#include "missingreceiver.h"
#include "ui_missingreceiver.h"

MissingReceiver::MissingReceiver(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MissingReceiver)
{
    ui->setupUi(this);
}

MissingReceiver::~MissingReceiver()
{
    delete ui;
}
