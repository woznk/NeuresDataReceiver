#include <QtWidgets>

#include "pressgraph.h"
#include "ui_pressgraph.h"

typedef enum {
    RED             = 0x00,
    BLUE            = 0x01,     // start data transfer
    YELLOW          = 0x02,     // stop data transfer
    GREEN           = 0x03,
}Colors;

pressGraph::pressGraph(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pressGraph)
{
    ui->setupUi(this);
    setupAxis();

}

pressGraph::~pressGraph()
{
    delete ui;
}

void pressGraph::setupAxis()
{
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s:%z");
    ui->pressGraph_2->xAxis->setTicker(timeTicker);
    ui->pressGraph_2->axisRect()->setupFullAxesBox();
    ui->pressGraph_2->yAxis->setRange(0, 1050);
}

void pressGraph::addGraph(bool status)
{
    add_Graph = status;
}

void pressGraph::addGraph(int GraphNo)
{
    ui->pressGraph_2->addGraph();
    switch (GraphNo){
        case RED:
            ui->pressGraph_2->graph(GraphNo)->setPen(QPen(QColor(255, 110, 40)));
            break;
        case BLUE:
            ui->pressGraph_2->graph(GraphNo)->setPen(QPen(QColor(40, 110, 255)));
            break;
    }
    connect(ui->pressGraph_2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->pressGraph_2->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->pressGraph_2->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->pressGraph_2->yAxis2, SLOT(setRange(QCPRange)));
}

void pressGraph::openGraph(bool status)
{
    open_Graph = status;
}

void pressGraph::addPressure(int GraphNo, double time, double press)
{
    ui->pressGraph_2->graph(GraphNo)->addData(time, press);
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->pressGraph_2->xAxis->setRange(time, 8, Qt::AlignRight);
    ui->pressGraph_2->replot();
}

bool pressGraph::checkAddGraph()
{
    return add_Graph;
}

bool pressGraph::checkOpenGraph()
{
    return open_Graph;
}

void pressGraph::closeEvent(QCloseEvent *event)
{
    emit closeWin();
    openGraph(false);
    event->accept();
}
