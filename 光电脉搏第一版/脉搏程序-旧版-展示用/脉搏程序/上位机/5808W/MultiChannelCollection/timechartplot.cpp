#include "timechartplot.h"

timeChartPlot::timeChartPlot(QWidget *parent) :QGraphicsView(new QGraphicsScene, parent)
{
    chart = new QChart();
    for(int i=0;i<8;i++)
    {
        chartElement.series[i] = new QLineSeries;
        chartElement.series[i]->setName("Channel "+QString::number(i+1,10));
        chartElement.yValueLabel[i] = new QGraphicsSimpleTextItem(chart);
    }
    chartElement.yValueLabel[8] = new QGraphicsSimpleTextItem(chart);
    chartElement.yValueLabel[9] = new QGraphicsSimpleTextItem(chart);
    chartElement.yValueLabel[10] = new QGraphicsSimpleTextItem(chart);

    chartElement.XplotRange = 200;
    chartElement.YplotRange = 40;

    setRenderHint(QPainter::Antialiasing);//提示
    setMouseTracking(true);
    m_coordX = new QGraphicsSimpleTextItem(chart);
    m_coordY = new QGraphicsSimpleTextItem(chart);
}

timeChartPlot::~timeChartPlot()
{
}

void timeChartPlot::resizeEvent(QResizeEvent *event)
{
    if (scene())
    {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        chart->resize(event->size());
    }
    QGraphicsView::resizeEvent(event);
}

void timeChartPlot::mouseMoveEvent(QMouseEvent *event)//鼠标移动事件
{
     m_coordX->setText(QString("X: %1").arg(chart->mapToValue(event->pos()).x()));

     double temp = fmod(chart->mapToValue(event->pos()).y(),5);
     if(temp > 2.5)
     {
         temp = temp-2.5;
         m_coordY->setText(QString("Y: %1").arg(temp));
     }
     else
     {
         temp=2.5-temp;
         m_coordY->setText(QString("Y: -%1").arg(temp));
     }
    QGraphicsView::mouseMoveEvent(event);
}

void timeChartPlot::chartInit()//图表初始化
{
    QObject::connect(&m_timer, &QTimer::timeout, this, &timeChartPlot::handleTimeout);
    m_timer.setInterval(10);//设置超时间隔为10ms

   //chart->setTitle("8 Channel TimeChart");

    chartElement.axisX = new QValueAxis;
    chartElement.axisX->setTickCount(11);//x轴画11条网格线
    chartElement.axisX->setLabelFormat("%.0f");
    chartElement.axisX->setRange(0,chartElement.XplotRange);
    chart->addAxis(chartElement.axisX, Qt::AlignBottom);//x轴与底部对齐

    chartElement.axisY= new QValueAxis;
    chartElement.axisY->setTickCount(17);//y轴画17条网格线
    chartElement.axisY->setRange(0,chartElement.YplotRange);
    chartElement.axisY->setLabelsVisible(false);//坐标标签不可见
    chartElement.axisY->setLabelFormat("%2.0f");
    chart->addAxis(chartElement.axisY, Qt::AlignRight);//y轴与右边对齐

    chart->createDefaultAxes();

    for(int i=0;i<8;i++)
    {
         chart->addSeries( chartElement.series[i]);
         chartElement.series[i]->attachAxis(chartElement.axisX);
         chartElement.series[i]->attachAxis(chartElement.axisY);
         chartElement.series[i]->setUseOpenGL(true);
    }
    chartElement.axisY_3 = new QCategoryAxis;
    chartElement.axisY_3->append("C1", 5);
    chartElement.axisY_3->append("C2", 10);
    chartElement.axisY_3->append("C3", 15);
    chartElement.axisY_3->append("C4", 20);
    chartElement.axisY_3->append("C5", 25);
    chartElement.axisY_3->append("C6", 30);
    chartElement.axisY_3->append("C7", 35);
    chartElement.axisY_3->append("C8", 40);
    chartElement.axisY_3->setRange(0,chartElement.YplotRange);
    QPen
    pen(QColor(31, 69, 110),
    0.5,
    Qt::DashLine
    );

    m_coordX->setPos(690,14);
    m_coordY->setPos(758,14);

    chartElement.axisY_3->setGridLinePen(pen);
    chart->addAxis(chartElement.axisY_3, Qt::AlignLeft);//与左边对齐

//    for(int i=0;i<8;++i)
//    {
//     chartElement.yValueLabel[i]->setText("0.0");
//     chartElement.yValueLabel[i]->setPos(838,55+i*66);
//    }

    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setMargins(QMargins(0,10,0,0));//距离顶部距离为10，其余边距为0

    scene()->addItem(chart);
    // m_timer.start();
}


void timeChartPlot::handleTimeout()//测试用？m_timer没开
{
    qint64 range = 200;

    for(int i=0;i<8;i++)
    {
        QVector<QPointF> oldPoints = chartElement.series[i]->pointsVector();
        QVector<QPointF> points;
        if (oldPoints.count() < range)
        {
            points = chartElement.series[i]->pointsVector();
        }
        else
        {
            for (int i = 1; i < oldPoints.count(); i++)
            {
                points.append(QPointF(i-1, oldPoints.at(i).y()));
            }
        }
        qint64 size = points.count();
        points.append(QPointF(size, qrand()%3+1+i*5));

        chartElement.series[i]->replace(points);
    }
}

void timeChartPlot::updateTimeChart(QList<qreal> (&timeData)[8], int channel_count,int a[8])//更新波形，传来多少个参数就接收多少个
{
    for(int i=0;i<channel_count;i++)
    {
        QVector<QPointF> oldPoints = chartElement.series[i]->pointsVector();
        QVector<QPointF> points;       
        if(channel_count==4)
        {
            if (oldPoints.count() < chartElement.XplotRange)//若旧点范围短，直接在后面加点
            {
                points = chartElement.series[i]->pointsVector();
            }
            else//如果满，删掉前面的点
            {
                for (int j = 128; j < oldPoints.count(); j++)
                {
                    points.append(QPointF(j-128, oldPoints.at(j).y()));
                }
            }
            qint64 size = points.count();//计算点数
            for (int k = 0; k<128; k++)
                if(a[i])
                    points.append(QPointF(k+size, timeData[i].at(k)));
        }
        else if(channel_count==8)
        {
            if (oldPoints.count() < chartElement.XplotRange)//若旧点范围短，直接在后面加点
            {
                points = chartElement.series[i]->pointsVector();
            }
            else//如果满，删掉前面的点
            {
                for (int j = 192; j < oldPoints.count(); j++)
                {
                    points.append(QPointF(j-192, oldPoints.at(j).y()));
                }
            }
            qint64 size = points.count();//计算点数
            for (int k = 0; k<192; k++)
               points.append(QPointF(k+size, timeData[i].at(k)));
        }
          chartElement.series[i]->replace(points);//用新点替换旧点
    }
}

void timeChartPlot::clearTimeChart()
{
    for(int i=0;i<8;i++)
       chartElement.series[i]->clear();
}

//创建button and box
timeWidget::timeWidget(QWidget *parent) :QWidget(parent),timeScale(createTimeScaleBox()),channelSelect(createChSelectBox()),
    startButton(createStartButton()),selectPlayFile(createFileButton()),speedSelect(createSPSelectBox()),plotFlag(false),speed(100)
{
    timeChart = new timeChartPlot;
    playState = new QLabel("");
    timeChart->chartInit();
    this->initWidget();
}
void timeWidget::initWidget()
{
   QVBoxLayout *baseLayout = new QVBoxLayout();//垂直框
   QHBoxLayout *settingsLayout = new QHBoxLayout();//水平框

   settingsLayout->addWidget(selectPlayFile);
   settingsLayout->addSpacing(20);//按钮后面的空间

   settingsLayout->addWidget(startButton);
   settingsLayout->addSpacing(50);

   settingsLayout->addWidget(new QLabel("Channel:"));
   settingsLayout->addWidget(channelSelect);
   settingsLayout->addSpacing(20);

   settingsLayout->addWidget(new QLabel("TimeScale:"));
   settingsLayout->addWidget(timeScale);
   settingsLayout->addSpacing(20);

   settingsLayout->addWidget(new QLabel("Speed:"));
   settingsLayout->addWidget(speedSelect);
   settingsLayout->addSpacing(20);

   settingsLayout->addWidget(playState);
   settingsLayout->addStretch();//将最小大小为零且拉伸系数为拉伸的可拉伸空间（qspaceritem）添加到此方框布局的末尾

   baseLayout->addLayout(settingsLayout,0);
   baseLayout->addWidget(timeChart);
  // qDebug()<<timeChart->size();
   this->setLayout(baseLayout);
}

timeWidget::~timeWidget()
{
}

QComboBox *timeWidget::createTimeScaleBox() const//时间范围选择
{
    QComboBox *timeScale = new QComboBox;
    timeScale->addItem("20Pt(2us)");
    timeScale->addItem("200Pt(20us)");
    timeScale->addItem("2000Pt(200us)");
    connect(timeScale, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&timeWidget::timeScaleChange);
    return timeScale;
}

QComboBox *timeWidget::createSPSelectBox() const//速度选择
{
    QComboBox *speedSelect = new QComboBox;
    speedSelect->addItem("10KB/S");
    speedSelect->addItem("20KB/S");
    speedSelect->addItem("40KB/S");
    connect(speedSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&timeWidget::speedChange);
    return speedSelect;
}

QComboBox *timeWidget::createChSelectBox() const//通道选择
{
    QComboBox *channelSelect = new QComboBox;
    channelSelect->addItem("ALL");
    channelSelect->addItem("CH1");
    channelSelect->addItem("CH2");
    channelSelect->addItem("CH3");
    channelSelect->addItem("CH4");
    channelSelect->addItem("CH5");
    channelSelect->addItem("CH6");
    channelSelect->addItem("CH7");
    channelSelect->addItem("CH8");
    connect(channelSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&timeWidget::channelChange);
    return channelSelect;
}
QPushButton *timeWidget::createStartButton() const //start
{
    QPushButton *startButton = new QPushButton("Start");
    connect(startButton,&QPushButton::clicked,this,&timeWidget::stateChange);
    return startButton;
}
QPushButton *timeWidget::createFileButton() const
{
    QPushButton *fileButton = new QPushButton("Select File");
    connect(fileButton,&QPushButton::clicked,this,&timeWidget::playDataFile);
    return fileButton;
}

void timeWidget::playDataFile()//select file选择要播放的文件
{
    QString fileFullPath;
    QFileInfo fileInfo;
    QString fileName;

    if(selectPlayFile->text()=="Select File")
    {
        fileFullPath = QFileDialog::getOpenFileName(this);

        if(!fileFullPath.isEmpty())
        {
            fileInfo = QFileInfo(fileFullPath);
            fileName = fileInfo.absoluteFilePath();
            selectPlayFile->setText("Cancel");
            qDebug()<<fileName;
            emit signalPlayFile(fileName);
        }
    }
    else//若为cancel
    {
        if(startButton->text()=="Start")
        {
            selectPlayFile->setText("Select File");
            for(int i=0;i<8;i++)
            timeChart->chartElement.series[i]->clear();//清除8个波形数据
            emit signalPlayFile("Cancel");
        }
      //  timeChart->chart->close();
    }
}

void timeWidget::timeScaleChange(int index)
{
    timeChart->chart->setAnimationOptions(QChart::AllAnimations);//图表中启用了所有动画类型

    switch (index) {
    case 0:
        timeChart->chartElement.XplotRange = 200;
        break;
    case 1:
       timeChart->chartElement.XplotRange = 2000;
        break;
    case 2:
        timeChart->chartElement.XplotRange = 20000;
        break;
    default:
        break;
    }
   timeChart->chartElement.axisX->setRange(0,timeChart->chartElement.XplotRange);
}


void timeWidget::stateChange()
{
    if(startButton->text()=="Start")
    {
        startButton->setText("Stop");
        plotFlag = true;
    }
    else
    {
        startButton->setText("Start");
        plotFlag =  false;
    }
    emit plotState(plotFlag);
}

void timeWidget::speedChange(int index)//播放延时设置
{
    switch (index)
    {
        case 0:
            speed = 1000;
            break;
        case 1:
            speed = 500;
            break;
        case 2:
            speed =250;
            break;
        default:
            break;
    }
    emit plotSpeed(speed);
}

void timeWidget::updatePlayState(QString state)
{
    playState->setText(state);//小控件右上角显示正在播放的文件大小
    if(state=="Completed.")
    {
        selectPlayFile->setText("Select File");
        startButton->setText("Start");
    }
}

void timeWidget::channelChange(int index)
{
    timeChart->chart->setAnimationOptions(QChart::AllAnimations);

    qDebug()<<index;
    if(index !=0)//仅选择其中一通道
    {
      timeChart->chartElement.YplotRange = index *5;
      timeChart->chartElement.axisY->setRange(timeChart->chartElement.YplotRange-5,timeChart->chartElement.YplotRange);
      timeChart->chartElement.axisY->setTickCount(11);
      timeChart->chartElement.axisY_3->setRange(timeChart->chartElement.YplotRange-5,timeChart->chartElement.YplotRange);
//       for(int i=0;i<11;++i)
//       {
//        timeChart->chartElement.yValueLabel[i]->setText(QString::number(-2.5+i*0.5,'f',1));//y轴标签显示,f表示浮点数
//        timeChart->chartElement.yValueLabel[i]->setPos(838,547-i*53);
//         if(i>7)
//             timeChart->chartElement.yValueLabel[i]->show();
//       }
     }
    else//选择全部通道
    {
        timeChart->chartElement.YplotRange = 40;
        timeChart->chartElement.axisY->setRange(0,timeChart->chartElement.YplotRange);
        timeChart->chartElement.axisY->setTickCount(17);

        timeChart->chartElement.axisY_3->setRange(0,timeChart->chartElement.YplotRange);

//       for(int i=0;i<11;++i)
//       {
//         timeChart->chartElement.yValueLabel[i]->setText("0.0");

//         timeChart-> chartElement.yValueLabel[i]->setPos(838,55+i*66);

//          if(i>7)
//              timeChart->chartElement.yValueLabel[i]->hide();
//        }
    }
}
