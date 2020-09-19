#ifndef PRESSGRAPH_H
#define PRESSGRAPH_H

#include <QWidget>

namespace Ui {
class pressGraph;
}

class pressGraph : public QWidget
{
    Q_OBJECT

public:
    explicit pressGraph(QWidget *parent = nullptr);
    ~pressGraph();
    void setupAxis();
    void addGraph(bool status);
    void addGraph(int GraphNo);
    void openGraph(bool status);
    void addPressure(int GraphNo, double time, double press);
    bool checkAddGraph();
    bool checkOpenGraph();


signals:
    void closeWin();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::pressGraph *ui;
    bool add_Graph = false;
    bool open_Graph = false;
};

#endif // PRESSGRAPH_H
