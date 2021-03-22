#include "cricketpressure.h"
#include "ui_cricketpressure.h"

#include <QDebug>

CricketPressure::CricketPressure(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CricketPressure)
{
    ui->setupUi(this);
     connect(ui->btnOK, SIGNAL(clicked()), this, SLOT(on_btnOK_clicked()));
//     connect(ui->mmHgCheck, SIGNAL(stateChanged(int)), this, SLOT(on_mmHgCheck_stateChanged(int)));
     setWindowTitle("Ustawienie ciśnień");
}
CricketPressure::~CricketPressure()
{
    delete ui;
}

void CricketPressure::on_btnOK_clicked()
{
    setResult(1);
    close();
}

void CricketPressure::on_btnCANCEL_clicked()
{
    setResult(0);
    close();
}

void CricketPressure::setPress1(int press1)
{
    if(ui->mmHgCheck->isChecked()){
        ui->pressSpin1->setMaximum(1024/3);
        ui->pressSpin1->setValue(press1/3);
    } else {
        ui->pressSpin1->setMaximum(1024);
        ui->pressSpin1->setValue(press1);
    }
};

void CricketPressure::setPress2(int press2)
{
    if(ui->mmHgCheck->isChecked()){
        ui->pressSpin2->setMaximum(1024/3);
        ui->pressSpin2->setValue(press2/3);
    } else {
        ui->pressSpin2->setMaximum(1024);
        ui->pressSpin2->setValue(press2);
    }
};

void CricketPressure::setPress3(int press3)
{
    if(ui->mmHgCheck->isChecked()){
        ui->pressSpin3->setMaximum(1024/3);
        ui->pressSpin3->setValue(press3/3);
    } else {
        ui->pressSpin3->setMaximum(1024);
        ui->pressSpin3->setValue(press3);
    }
};

int CricketPressure::getPress1()
{
    if(ui->mmHgCheck->isChecked()){
        return (ui->pressSpin1->value())*3;
    } else {
        return ui->pressSpin1->value();
    }
}

int CricketPressure::getPress2()
{
    if(ui->mmHgCheck->isChecked()){
        return (ui->pressSpin2->value())*3;
    } else {
        return ui->pressSpin2->value();
    }
}

int CricketPressure::getPress3()
{
    if(ui->mmHgCheck->isChecked()){
        return (ui->pressSpin3->value())*3;
    } else {
        return ui->pressSpin3->value();
    }
}

void CricketPressure::on_mmHgCheck_stateChanged(int arg1)
{
    if(ui->mmHgCheck->isChecked()){;
        ui->pressSpin1->setValue((ui->pressSpin1->value()/3));
        ui->pressSpin2->setValue((ui->pressSpin2->value()/3));
        ui->pressSpin3->setValue((ui->pressSpin3->value()/3));
    } else {
        ui->pressSpin1->setValue((ui->pressSpin1->value()*3));
        ui->pressSpin2->setValue((ui->pressSpin2->value()*3));
        ui->pressSpin3->setValue((ui->pressSpin3->value()*3));
    }
}
