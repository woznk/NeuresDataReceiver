#ifndef CRICKETPRESSURE_H
#define CRICKETPRESSURE_H

#include <QDialog>

namespace Ui {
class CricketPressure;
}

class CricketPressure : public QDialog
{
    Q_OBJECT

public:
    explicit CricketPressure(QWidget *parent = nullptr);
    ~CricketPressure();
public slots:
    void setPress1(int press1);
    void setPress2(int press2);
    void setPress3(int press3);

    int getPress1();
    int getPress2();
    int getPress3();

private slots:
    void on_btnOK_clicked();

    void on_btnCANCEL_clicked();

    void on_mmHgCheck_stateChanged(int arg1);

private:
    Ui::CricketPressure *ui;
};

#endif // CRICKETPRESSURE_H
