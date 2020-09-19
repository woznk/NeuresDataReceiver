#ifndef MISSINGRECEIVER_H
#define MISSINGRECEIVER_H

#include <QDialog>

namespace Ui {
class MissingReceiver;
}

class MissingReceiver : public QDialog
{
    Q_OBJECT

public:
    explicit MissingReceiver(QWidget *parent = nullptr);
    ~MissingReceiver();

private:
    Ui::MissingReceiver *ui;
};

#endif // MISSINGRECEIVER_H
