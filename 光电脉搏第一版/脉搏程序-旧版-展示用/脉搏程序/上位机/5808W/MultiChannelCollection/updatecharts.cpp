#include "updatecharts.h"

int a[8]={0,0,0,0,0,0,0,0};
updateCharts::updateCharts(QThread  *parent) : QThread (parent)
  ,runState(false),runSpeed(1000),KBcount(0),cc(0),sequence(0),n(0)
{
    tWidget = new timeWidget;
    //Charts = new updateCharts();
    connect(tWidget,&timeWidget::plotState,this,&updateCharts::stateChange);//start or stop
    connect(tWidget,&timeWidget::plotSpeed,this,&updateCharts::speedChange);
    connect(tWidget,&timeWidget::signalPlayFile,this,&updateCharts::startPlotFile);//若选项卡控件在timewidget这个对象中发出信号xx，则当前对象转到updatecharts的槽中
    connect(this,&updateCharts::signalCurrentPlayState,tWidget,&timeWidget::updatePlayState);
}

updateCharts::~updateCharts()
{
    delete tWidget;
    requestInterruption();
    quit();
    wait();
}

void updateCharts::startPlotFile(QString fName)//timechartplot中选择文件后的槽函数
{
   qDebug() << "进入文件显示槽";
   tWidget->timeChart->clearTimeChart();//清除坐标轴上的波形
    if(fName =="Cancel") //取消播放操作
    {
        requestInterruption();
        quit();
        wait();
        KBcount=0;
        emit signalCurrentPlayState("");
        return;
    }
        ///若传来的是文件名
        fileName = fName;
        rFile = new QFile(fileName);
        if(!rFile->isOpen())//若该文件未曾打开
           rFile->open(QIODevice::ReadOnly);//打开
        fileSize = rFile->size();//获取其大小
        qDebug() << fileSize/1024 << "kB";
        if (this->isRunning())//若该文件正在播放中
        {
            this->wait();  //等待
        }
        //判断序列号
        QString head = rFile->read(1024).toHex();//读取帧头
        //qDebug()<< head;
        QString medium = head.at(7);//获取通道选择4or8
        sequence = medium.toInt();//0或2为8通道，1为4通道
        if(sequence == 1) cc=4;
        else if(sequence == 0||sequence == 2) cc=8;
        //qDebug()<< sequence;

        //判断哪几通道选通
        rFile->seek(8);//指到第八个字节
        QByteArray byte0 = rFile->read(1);//byte为十六进制字符
        char byte = byte0[0];
        //qDebug()<<byte;//打印出3
        rFile->seek(1024);
        n=0;
        for(int i=0;i<8;i++)
            a[i]=0;
        for(int i=0; i<8; i++)
        {
            if(byte&0x01)
            {
                a[i]=1;
                n++;
                byte = byte>>1;
            }
            else
                byte = byte>>1;
        }
//        for(int i=0;i<8;i++)
//          qDebug()<< a[i];

        KBcount=0;//文件大小
        emit signalCurrentPlayState("Played: "+QString::number(KBcount)+"KB");//发出携带字符串的信号，那么槽也要有参数字符串

        this->start();//开启线程
}

void updateCharts::stateChange(bool state)
{
    runState = state;
}

void updateCharts::speedChange(unsigned long speed)
{
    runSpeed = speed;
}

qreal updateCharts::getValue(QByteArray twoBytes)//每次取2个字节（1个点）画图
{
    if(sequence==0)
    {
        if(twoBytes[0]&0x0020)//负值  设置为0-2.5
        {
            return (qreal)((((twoBytes[0]&0x001f)<<8)|(twoBytes[1]&0x00ff))/8192.0*2.5);
           //  qDebug()<<(int)(twoBytes[1]&0x001f);
        }
        else
        {
            return (qreal)((((twoBytes[0]&0x001f)<<8)|(twoBytes[1]&0x00ff))/8192.0*2.5+2.5);
        }
    }
    else if (sequence==1)
         return (qreal)((((twoBytes[0]&0x000f)<<8)|(twoBytes[1]&0x00ff))/4096.0*5);
    else if (sequence==2)
         return (qreal)((((twoBytes[1]&0x000f)<<8)|(twoBytes[0]&0x00ff))/4096.0*5);
}

//绘图进程
void updateCharts::run()
{
    QByteArray dataPack;
    QList<qreal> timeYData[8];
    while(!isInterruptionRequested())
    {
      if(KBcount<(fileSize/1024-1))//判断数据是否小于所选文件大小
      {
            while(!runState)//暂停与否（暂停）,由start键决定，一开始为false
            {
                if(isInterruptionRequested())//取消操作
                {
                    rFile->close();
                    return;
                }
            }
           // qDebug()<<"开始绘图";
           // qDebug()<<rFile->pos();

            if(cc==4)
            {  
                if(n==1)
                    dataPack=rFile->read(256);
                else if(n==2)
                    dataPack=rFile->read(512);
                else if(n==3)
                    dataPack=rFile->read(768);
                else if(n==4)
                    dataPack=rFile->read(1024);

                double dataPackSize = dataPack.size();
                for(int j=0;j<128;j++)//2个字节1个点,1个通道128个点（4通道）
                {
                  int p=0;
                  for(int k=0;k<4;k++)//4个通道
                  {
                      if(a[k])
                      {
                          timeYData[k] << getValue(dataPack.mid(j*2*n+p*2,2)) + k*5;//mid()返回一个从pos位置开始，包含len（默认值-1）字节
                          p++;
                      }
                      else
                         timeYData[k]<<k*5;
                  }
                }
                KBcount=KBcount + dataPackSize/1024;
            }
            else if(cc==8)
            {
                dataPack = rFile->read(3072);//160k-1k帧头=159k整除3k
                for(int j=0;j<192;j++)//2个字节1个点,1个通道192个点（8通道）
                {
                  for(int k=0;k<8;k++)//8个通道
                    timeYData[k] << getValue(dataPack.mid(j*16+k*2,2)) + k*5;//mid()返回一个从pos位置开始，包含len（默认值-1）字节
                }
                KBcount+=3.0;
            }
            tWidget->timeChart->updateTimeChart(timeYData,cc,a);//绘制波形（8个数组数据）
            //qDebug()<<"绘图完毕";
            msleep(runSpeed);//设置绘制速度
            for(int l=0;l<cc;l++)
                timeYData[l].clear();//删除赋给列表的数据
            emit signalCurrentPlayState("Played: "+QString::number(KBcount)+"KB");//绘制完1KB数据就显示在右上角
      }
      else
      {
            qDebug()<<"read done";
            qDebug()<<rFile->pos();
            rFile->close();
            KBcount=0;

            emit signalCurrentPlayState("Completed.");
            runState=false;
            this->requestInterruption();
            this->quit();
        }
   }
}


