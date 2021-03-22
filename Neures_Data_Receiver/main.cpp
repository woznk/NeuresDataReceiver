#include "neuresapplication.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NeuresApplication w;
    w.show();
    return a.exec();
}
