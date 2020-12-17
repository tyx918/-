#ifndef UPDATECHARTS_H
#define UPDATECHARTS_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <timechartplot.h>
#include "multifunc.h"


class updateCharts : public QThread
{
    Q_OBJECT

    inline qreal getValue(QByteArray twoBytes);

public:
    explicit updateCharts(QThread  *parent = nullptr);
    ~updateCharts();
    timeWidget *tWidget;
    //updateCharts *Charts;

signals:
    void signalCurrentPlayState(QString state);

public slots:
    void run();
    void startPlotFile(QString fName);


private slots:
    void stateChange(bool);
    void speedChange(unsigned long);

private:
    QString fileName;
    qint64 fileSize;
    QFile *rFile;
    bool runState;
    unsigned long runSpeed;
    double KBcount;
    int cc,sequence,n;
    //int a[8];

};

#endif // UPDATECHARTS_H
