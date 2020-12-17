#ifndef MULTICHANNEL_H
#define MULTICHANNEL_H

#include <QWidget>
#include "csocketserver.h"
#include "csocketprocess.h"
#include "common.h"
#include "multifunc.h"
#include "cdataprocess.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QSplineSeries>
#include <QtWidgets/QGraphicsView>
#include "timechartplot.h"
#include <QFile>
#include <QElapsedTimer>
#include <QToolTip>


QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MultiChannel;
}

class MultiChannel : public QWidget
{
    Q_OBJECT

public:
    explicit MultiChannel(QWidget *parent = nullptr);
    ~MultiChannel();

private slots:
    void slotAddNewClient(QString qsip,int iflag);//新客户端上线
    void slotDelClient(int iflag);
    void UIinitialization();
    void plotInit();
    void fileDealInfo(QString info);
    void deleteTab(int index);

    void adChannelChoose();//ad通道选择子函数
    void adRateChoose();//采样率选择函数
    void daChannelChoose();//da通道选择子函数

    void on_siglePB_clicked();
    void on_start_clicked();
    void on_sendPB_clicked();
    void on_multiPB_clicked();
    void on_startPB_clicked();
    void on_stopPB_clicked();
    void on_resetPB_clicked();
    void on_sendFile_clicked();

    void on_multiPB_2_clicked();

    void on_upSensor_clicked();

    //void on_upCollect_clicked();

    void on_upHistory_clicked();

    void on_verticalSlider_valueChanged(int value);

    void on_horizontalSlider_valueChanged(int value);

    void on_Stop_clicked();

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_doubleSpinBox_2_valueChanged(double arg1);

    void on_adSure_clicked();

    void on_daSure_clicked();


    void on_setPath_clicked();

private:
    Ui::MultiChannel *ui;

    CsocketServer *MserverHandle;

    CsocketProcess *MprocessHandle;

    int clientCount;
    QFile *sfile;

    int speedflag;
    int speedtemp;
    int speedflag_lr;
    int speedtemp_lr;

    QString rate;
    QString channel;
    QString datas;
    QString point;
    QString changeRate;
    QString filePath;

    int adChannelSelectSum;

};

#endif // MULTICHANNEL_H
