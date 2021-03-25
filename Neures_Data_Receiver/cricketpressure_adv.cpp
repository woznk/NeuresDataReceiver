#include "cricketpressure_adv.h"
#include "ui_cricketpressure_adv.h"

cricketpressure_adv::cricketpressure_adv(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cricketpressure_adv)
{
    ui->setupUi(this);
}

cricketpressure_adv::~cricketpressure_adv()
{
    delete ui;
}
