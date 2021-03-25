#ifndef CRICKETPRESSURE_ADV_H
#define CRICKETPRESSURE_ADV_H

#include <QDialog>

namespace Ui {
class cricketpressure_adv;
}

class cricketpressure_adv : public QDialog
{
    Q_OBJECT

public:
    explicit cricketpressure_adv(QWidget *parent = nullptr);
    ~cricketpressure_adv();

private:
    Ui::cricketpressure_adv *ui;
};

#endif // CRICKETPRESSURE_ADV_H
