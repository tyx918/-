#include "multichannel.h"
#include "ui_multichannel.h"
#include "pcap.h"

QT_CHARTS_USE_NAMESPACE
MultiChannel::MultiChannel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiChannel),clientCount(0),adChannelSelectSum(0)
{
    ui->setupUi(this);
    MprocessHandle = CsocketProcess::instance();
    MserverHandle = CsocketServer::instance();

    connect(MprocessHandle,SIGNAL(signalAddNewClient(QString,int)),this,SLOT(slotAddNewClient(QString,int)));
    connect(MprocessHandle,SIGNAL(signalDelClient(int)),this,SLOT(slotDelClient(int)));
    connect(MprocessHandle,SIGNAL(signalToMainUI(QString)),this,SLOT(fileDealInfo(QString)));

    UIinitialization();

    //test funciton
//    pcap_if_t *alldevs;
//    pcap_if_t *d;
//    int i = 0;
//    char errbuf[PCAP_ERRBUF_SIZE];
//    if(pcap_findalldevs(&alldevs, errbuf) == -1)
//    {
//        qDebug() << errbuf;
//    }
//    for(d = alldevs; d; d = d->next)
//    {
//        qDebug() << ++i << d->name;
//        ui->infoShow->append(d->name);
//        if(d->description)
//            qDebug() << d->description;
//        else
//            qDebug("(No description available)");
//    }
//    if(0 == i)
//    {
//        qDebug("No interfaces found! Make sure WinPcap is installed.");
//    }
//    pcap_freealldevs(alldevs);
}

MultiChannel::~MultiChannel()
{
    delete ui;
}

void MultiChannel::UIinitialization()//ui界面初始化
{
    ui->portEdit->setText("5001");//设置端口号

    QString filePath = QDir::currentPath();  //静态成员需要在类定义体外定义
    ui->currentPath->setText(filePath);

    ui->adSure->setToolTip("每通道每帧采集数据长度 = （通道总数 / 当前通道数量）* 设置采样点数                       ");
//    ui->tip2->setWhatsThis("");
//    ui->tip->setWhatsThis("");

    //采集参数设置
    //AD采集点数
    ui->adNum->addItem(tr(" "));
    ui->adNum->addItem(tr("1K"));
    ui->adNum->addItem(tr("10K"));
    ui->adNum->addItem(tr("100K"));
    ui->adNum->addItem(tr("1M"));

    //AD采样率
    ui->adRate->addItem(tr(" "));
    ui->adRate->addItem(tr("0.352MSPS"));
    ui->adRate->addItem(tr("0.705MSPS"));
    ui->adRate->addItem(tr("1.411MSPS"));
    ui->adRate->addItem(tr("2.117MSPS"));

    //DA转换率
    ui->daRate->addItem(tr(" "));
    ui->daRate->addItem(tr("0.333MSPS"));
    ui->daRate->addItem(tr("0.667MSPS"));
    ui->daRate->addItem(tr("1.333MSPS"));
    ui->daRate->addItem(tr("2.000MSPS"));

    ui->folders->setPlaceholderText("num1");//上传文件夹
    ui->files->setPlaceholderText("num2");//上传文件

    MultiChannel::plotInit();//调用函数
}

void MultiChannel::plotInit()//初始界面显示
{
   updateCharts *charts = new updateCharts;
   charts->tWidget->setEnabled(true);

   ui->tabWidget->addTab(charts->tWidget,"播放波形：");//添加一个画轴

   connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(deleteTab(int)));
}

void MultiChannel::slotAddNewClient(QString qsip, int iflag)//客户端上线
{
    if(clientCount == 0)//初始状态
    {
        ui->tabWidget->clear();
        ui->tabWidget->addTab(MprocessHandle->getClientHandle(iflag)->udCharts->tWidget,"TPlot-"+qsip);//time series添加一个空白波形图
        ui->tabWidget->currentWidget()->setWhatsThis(QString::number(iflag));//设置客户端名字
        ui->tabWidget->setTabsClosable(true);
    }
    else
    {
        ui->tabWidget->addTab(MprocessHandle->getClientHandle(iflag)->udCharts->tWidget,"TPlot-"+qsip);
        ui->tabWidget->setCurrentWidget(MprocessHandle->getClientHandle(iflag)->udCharts->tWidget);//使新添加的空白波形图成为当前空白波形图
        ui->tabWidget->currentWidget()->setWhatsThis(QString::number(iflag));
    }
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"新的客户端连接");

    ++clientCount;
    qDebug()<<"slotAddNewClient :"<<qsip<<iflag;
}


void MultiChannel::deleteTab(int index)//主动关闭小部件窗口
{
    bool ok;
    int iflag = ui->tabWidget->widget(index)->whatsThis().toInt(&ok,10);

    ui->tabWidget->removeTab(index);
    MprocessHandle->proClientDisconnect(iflag);
    --clientCount;

    if(clientCount ==0)//所有客户端已下线 设置默认界面
    {
//        ui->tabWidget->setTabsClosable(false);
//        timeWidget *initalTimeWidget = new timeWidget;
//        initalTimeWidget->setEnabled(true);
//        ui->tabWidget->addTab(initalTimeWidget,"播放波形：");

        updateCharts *charts = new updateCharts;
        charts->tWidget->setEnabled(true);

        ui->tabWidget->addTab(charts->tWidget,"播放波形：");//添加一个画轴

        connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(deleteTab(int)));
        ui->tabWidget->setTabsClosable(false);

        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"无客户端连接");
    }
    qDebug()<<"deleteTab:"<<iflag<<index;
}

void MultiChannel::slotDelClient(int iflag) //客户端主动下线
{
    ui->tabWidget->removeTab(iflag);
    --clientCount;

    if(clientCount == 0)
    {
//        ui->tabWidget->setTabsClosable(false);
//        timeWidget *initalTimeWidget = new timeWidget;
//        initalTimeWidget->setEnabled(true);
//        ui->tabWidget->addTab(initalTimeWidget,"播放波形：");
        updateCharts *charts = new updateCharts;
        charts->tWidget->setEnabled(true);

        ui->tabWidget->addTab(charts->tWidget,"播放波形：");//添加一个画轴

        connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(deleteTab(int)));
        ui->tabWidget->setTabsClosable(false);

        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"无客户端连接");
    }
    qDebug()<<"slotDelClient:"<<iflag;
}

void MultiChannel::fileDealInfo(QString info)
{
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+info);
}

void MultiChannel::on_siglePB_clicked()//单帧采集
{
    if(clientCount ==0)
        return;
    else
    {
        if(ui->sigleCheck->isChecked())
            MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D44FFFFFFFFFFFFFFFFFFFF000001"));
        else
            MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D4400000000000000000000000001"));
    }
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"单帧采集指令发送成功");
}

void MultiChannel::on_start_clicked()//启动or停止
{
    if(ui->portEdit->text().isEmpty())//若端口号为空
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"请输入端口号");
    else
    {
        if (ui->start->text()=="启动")
        {
            MserverHandle->serverStart(ui->portEdit->text().toInt());
            ui->start->setText("停止");
            ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"启动监听成功");
        }
        else//若点击的是“停止”
        {
          MserverHandle->serverStop();
          MprocessHandle->clearAllConnect();
          ui->tabWidget->clear();
//          timeWidget *initalTimeWidget = new timeWidget;
//          initalTimeWidget->setEnabled(true);
//          ui->tabWidget->addTab(initalTimeWidget,"播放波形:");

          updateCharts *charts = new updateCharts;
          charts->tWidget->setEnabled(true);
          ui->tabWidget->addTab(charts->tWidget,"播放波形：");//添加一个画轴
          connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(deleteTab(int)));
          ui->tabWidget->setTabsClosable(false);

          clientCount =0;
          ui->start->setText("启动");
          ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"停止监听成功");
        }
    }
}

void MultiChannel::on_sendPB_clicked()//下发指令
{
     if(clientCount ==0)
        return;

    if (ui->hexCheck->isChecked())//若勾选十六进制
    {
        QString data=ui->sendText->text();//获取输入的文本内容
        if (data==""){return;}  //没有输入则返回

        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex(data.toLatin1()));//发送该文本内容
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
    else
    {
        QByteArray data=ui->sendText->text().toLocal8Bit();//将QString以自己正确的编码方式读取并转换成编译器编码方式的字节流
        if (data==""){return;}  //没有输入则返回

        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(data);
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"数据发送成功");
    }
}

void MultiChannel::on_multiPB_clicked()//多帧采集
{
    if(clientCount ==0)
       return;
    else
    {
        if(ui->mutiCheck->isChecked())
             MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D44FFFFFFFFFFFFFFFFFFFF000002"));
        else
             MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D4400000000000000000000000002"));
    }
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"多帧采集指令发送成功");
}

void MultiChannel::on_startPB_clicked()//开始刺激
{
   if(clientCount ==0)
     return;
   MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D4400000000000000000000000003"));
   ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"开始刺激指令发送成功");
}

void MultiChannel::on_stopPB_clicked()//停止刺激
{
    if(clientCount ==0)
      return;
    MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D4400000000000000000000000004"));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"停止刺激指令发送成功");
}

void MultiChannel::on_resetPB_clicked()//复位
{
    if(clientCount ==0)
        return;
    MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D4452535400000000000000000000"));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"复位指令发送成功");
}

void MultiChannel::on_sendFile_clicked()//下发文件
{
    if(clientCount ==0)
        return;
    QString DAFileFullPath;
    QFileInfo DAFileInfo;
    QString DAFileName;
    QElapsedTimer t;

    if(ui->sendFile->text()=="下发文件")
    DAFileFullPath = QFileDialog::getOpenFileName(this);

    if(ui->sendFile->text()=="发送中")
    {
        ui->sendFile->setText("下发文件");  //发送不成功时复位按键
        sfile->close();
        //return;
    }

    if(!DAFileFullPath.isEmpty())
    {
        int i = 0;
        DAFileInfo = QFileInfo(DAFileFullPath);
        DAFileName = DAFileInfo.absoluteFilePath();
        ui->sendFile->setText("发送中");
        sfile=new QFile(DAFileName);
        qint64 filesize = sfile->size();
        qDebug()<<filesize;
        sfile->open(QIODevice::ReadOnly);

//        QByteArray suffixMsg = QByteArray::fromHex(QString::asprintf(QByteArray("444143%020X434144"),(filesize+1)/3).toLatin1());//%020X用filesize填充，表示20位的16进制字符
//        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(suffixMsg);//发送先行数据
//        //发送选择的文件
//        do
//        {
//        QByteArray DataPack =sfile->read(1024); //返回QBytearrary;
//        qDebug()<<DataPack;
//       //MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(DataPack);//詹
//        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(multifunc::HexStrToByteArray(DataPack));//鸿森and建伍
//        qDebug()<<multifunc::HexStrToByteArray(DataPack);
//        }while(sfile->pos()<sfile->size());

       QByteArray DataPack;
       DataPack =sfile->read(3056);//1024*3=3076 3076-16=3056
       QByteArray suffixMsg = QString::asprintf(QByteArray("444143%020X434144"),(filesize+1)/3).toLatin1();//%020X用filesize填充，表示20位的16进制字符
       MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex(suffixMsg+DataPack));//发送先行数据+第一帧

       DataPack=sfile->readLine();

       //发送选择的文件
       t.start();
       while(t.elapsed()<1000);//让程序等待一段时间
       while(sfile->pos()<sfile->size())
       {
           DataPack = sfile->read(3072); //返回QBytearrary;
           QByteArray realPack = QByteArray::fromHex(DataPack);
        //  qDebug()<<realPack;
           MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(realPack);
           t.restart();
           while(t.elapsed()<1000);
       }
       ui->sendFile->setText("下发文件");
       ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"文件下发成功");
    }
}

void MultiChannel::on_multiPB_2_clicked()//停止采集
{
    if(clientCount ==0)
      return;
    MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D440000000000000000000000000C"));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"停止采集发送成功");
}

void MultiChannel::on_upSensor_clicked()//上发最新传感器数据
{
    if(clientCount ==0)
      return;
    MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D4400000000000000000000000007"));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"上发最新传感数据指令发送成功");
}

//void MultiChannel::on_upCollect_clicked()//上发最新采集数据
//{
//    if(clientCount ==0)
//      return;
//    MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D4400000000000000000000000001434D4400000000000000000000000008"));
//    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"上发最新采集数据指令发送成功");
//}


void MultiChannel::on_upHistory_clicked()//上发历史采集数据
{
    if(clientCount ==0)
      return;

    QString folder =ui->folders->text();
    QString file =ui->files->text();
    //十进制字符串转十进制字符
    int temp1 = multifunc::StrDecimalToDecimal(folder);
    int temp2 = multifunc::StrDecimalToDecimal(file);

    QString upload= "434D44";

    //十进制字符转十六进制字符,如：23——17
    QString t1 = multifunc::IntDecToStrHex(temp1);
    QString t2 = multifunc::IntDecToStrHex(temp2);

    if(temp1<16 && temp1>0)//判断输入的十进制字符是否小于16，小于则用1位十六进制字符表示，并在前面补0
      {
        upload += "0";
        upload += t1;
      }
    else
      upload += t1;

    if(temp2<16 && temp2>0)
      {
        upload += "0";
        upload += t2;
        upload += "0000000000000000000009";
      }
    else
      {
        upload += t2;
        upload += "0000000000000000000009";
      }

    QByteArray suffixMsg = multifunc::HexStrToByteArray(upload);//将字符串转为字节数组

    MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(suffixMsg);
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"上发历史采集数据指令发送成功");
}

void MultiChannel::on_verticalSlider_valueChanged(int value)//垂直滑块
{
    //前进8档
    if(value>=1&&value <=25)
        speedflag = 1;
    else if(value >=26&& value<=50)
        speedflag = 2;
    else if(value >=51 && value <=75)
        speedflag =3;
    else if(value >=76 && value <=100)
        speedflag =4;
    else if(value >=101 && value <=125)
        speedflag =5;
    else if(value >=126 && value <=150)
        speedflag =6;
    else if(value >=151 && value <=175)
        speedflag =7;
    else if(value >=176 && value <=200)
        speedflag =8;

    //后退8档
    else if(value >=-25&& value<=-1)
        speedflag = -1;
    else if(value >=-50&& value<=-26)
        speedflag = -2;
    else if(value >=-75 && value <=-51)
        speedflag =-3;
    else if(value >=-100 && value <=-76)
        speedflag =-4;
    else if(value >=-125 && value <=-101)
        speedflag =-5;
    else if(value >=-150&& value<=-126)
        speedflag = -6;
    else if(value >=-175&& value<=-151)
        speedflag = -7;
    else if(value >=-200 && value <=-176)
        speedflag =-8;
    else
        speedflag = 0;

    ui->label_vspeed->setText(tr("%1\%").arg(speedflag*12.5));//将滑块值设定范围为-100% ~ 100%
    if(speedflag == 1 && speedtemp!=1)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B46010000000000000000000B"));
        speedtemp = 1;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 2 && speedtemp !=2)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B46020000000000000000000B"));
        speedtemp = 2;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 3 && speedtemp !=3)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B46030000000000000000000B"));
        speedtemp = 3;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 4 && speedtemp !=4)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B46040000000000000000000B"));
        speedtemp = 4;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 5 && speedtemp !=5)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B46050000000000000000000B"));
        speedtemp =5;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 6 && speedtemp !=6)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B46060000000000000000000B"));
        speedtemp = 6;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 7 && speedtemp !=7)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B46070000000000000000000B"));
        speedtemp = 7;
        qDebug()<<speedtemp;
    }
    else if(speedflag == 8 && speedtemp !=8)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B46080000000000000000000B"));
        speedtemp = 8;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -1 && speedtemp != -1)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B42010000000000000000000B"));
        speedtemp = -1;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -2 && speedtemp !=-2)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B42020000000000000000000B"));
        speedtemp = -2;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -3 && speedtemp !=-3)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B42030000000000000000000B"));
        speedtemp = -3;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -4 && speedtemp != -4)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B42040000000000000000000B"));
        speedtemp = -4;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -5 && speedtemp != -5)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B42050000000000000000000B"));
        speedtemp = -5;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -6 && speedtemp !=-6)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B42060000000000000000000B"));
        speedtemp = -6;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -7 && speedtemp !=-7)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B42070000000000000000000B"));
        speedtemp = -7;
        qDebug()<<speedtemp;
    }
    else if(speedflag == -8 && speedtemp != -8)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B42080000000000000000000B"));
        speedtemp = -8;
        qDebug()<<speedtemp;
    }
}

void MultiChannel::on_horizontalSlider_valueChanged(int value)//水平滑块
{
    //右转8档
    if(value>=1&&value <=25)
        speedflag_lr = 1;
    else if(value >=26&& value<=50)
        speedflag_lr = 2;
    else if(value >=51 && value <=75)
        speedflag_lr =3;
    else if(value >=76 && value <=100)
        speedflag_lr =4;
    else if(value >=101 && value <=125)
        speedflag_lr =5;
    else if(value >=126 && value <=150)
        speedflag_lr =6;
    else if(value >=151 && value <=175)
        speedflag_lr =7;
    else if(value >=176 && value <=200)
        speedflag_lr =8;

    //左转8档
    else if(value >=-25&& value<=-1)
        speedflag_lr = -1;
    else if(value >=-50&& value<=-26)
        speedflag_lr = -2;
    else if(value >=-75 && value <=-51)
        speedflag_lr =-3;
    else if(value >=-100 && value <=-76)
        speedflag_lr =-4;
    else if(value >=-125 && value <=-101)
        speedflag_lr =-5;
    else if(value >=-150&& value<=-126)
        speedflag_lr = -6;
    else if(value >=-175&& value<=-151)
        speedflag_lr = -7;
    else if(value >=-200 && value <=-176)
        speedflag_lr =-8;
    else
        speedflag_lr = 0;

     ui->label_hspeed->setText(tr("%1\%").arg(speedflag_lr*12.5));

    if(speedflag_lr == 1 && speedtemp_lr!=1)
    {
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B52010000000000000000000B"));
        speedtemp_lr = 1;
        qDebug()<<speedtemp_lr;
    }
    else if(speedflag_lr == 2 && speedtemp_lr !=2){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B52020000000000000000000B"));

        speedtemp_lr = 2;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 3 && speedtemp_lr !=3){
       MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B52030000000000000000000B"));

        speedtemp_lr = 3;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 4 && speedtemp_lr !=4){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B52040000000000000000000B"));

        speedtemp_lr = 4;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 5 && speedtemp_lr !=5){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B52050000000000000000000B"));

        speedtemp_lr =5;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 6 && speedtemp_lr !=6){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B52060000000000000000000B"));

        speedtemp_lr = 6;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 7 && speedtemp_lr !=7){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B52070000000000000000000B"));
                speedtemp_lr = 7;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == 8 && speedtemp_lr !=8){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B52080000000000000000000B"));

        speedtemp_lr = 8;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -1 && speedtemp_lr != -1){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B4C010000000000000000000B"));

        speedtemp_lr = -1;
       qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -2 && speedtemp_lr !=-2){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B4C020000000000000000000B"));

        speedtemp_lr = -2;
       qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -3 && speedtemp_lr !=-3){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B4C030000000000000000000B"));
        speedtemp_lr = -3;
       qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -4 && speedtemp_lr != -4){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B4C040000000000000000000B"));

        speedtemp_lr = -4;
      qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -5 && speedtemp_lr != -5){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B4C050000000000000000000B"));

        speedtemp_lr = -5;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -6 && speedtemp_lr !=-6){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B4C060000000000000000000B"));

        speedtemp_lr = -6;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -7 && speedtemp_lr !=-7){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B4C070000000000000000000B"));
        speedtemp_lr = -7;
        qDebug()<<speedtemp_lr;

    }
    else if(speedflag_lr == -8 && speedtemp_lr != -8){
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B4C080000000000000000000B"));
        speedtemp_lr = -8;
        qDebug()<<speedtemp_lr;
    }
}

void MultiChannel::on_Stop_clicked()//电机停止
{
    if(clientCount ==0)
      return;
    MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B43530000000000000000000B"));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"电机停止指令发送成功");
}

void MultiChannel::on_doubleSpinBox_valueChanged(double arg1)//垂直滑块微调框部件
{
    static double helper = 0;
    if(arg1 > helper)//正值
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B6C460000000000000000000B"));
    else//负值
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B6C520000000000000000000B"));
    helper = arg1;
}

void MultiChannel::on_doubleSpinBox_2_valueChanged(double arg1)//水平滑块微调框部件
{
    static double helper1 = 0;
    if(arg1 > helper1)//正值
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B72460000000000000000000B"));
    else//负值
        MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(QByteArray::fromHex("434D442B72520000000000000000000B"));
    helper1 = arg1;
}

void MultiChannel::on_adSure_clicked()//AD设置确定按键
{
    QString arg1 = ui->adNum->currentText();
    QString begin = "434D44";
    QString distinguish = "10";//分辨率默认16bit
    QString last = "0000000000000005";

    if(clientCount==0)
    {
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"无客户端连接，无法配置");
        return;
    }

    if(arg1!=" ")
    {
        if(arg1 == "1K")//判断采样点数
          point= "01";
        else if(arg1 =="10K")
          point= "0A";
        else if(arg1 =="100K")
          point= "64";
        else if(arg1 =="1M")
          point= "FF";
    }
    else
        point= "64";//默认配置为100K
    adRateChoose();
    adChannelChoose();

    datas = begin+rate+distinguish+channel+point+last;
    qDebug()<<datas;
    MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(multifunc::HexStrToByteArray(datas));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"AD已配置");
    ui->label_display->setText("每帧采集数据长度="+QString::number(adChannelSelectSum)+"*"+arg1);
}

void MultiChannel::on_daSure_clicked()//DA设置确定按键
{
    QString arg2 = ui->daRate->currentText();
    QString begin = "434D44";
    QString distinguish = "10";
    QString last = "000000000000000006";

    if(clientCount==0)
    {
        ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"无客户端连接，无法配置");
        return;
    }

    if(arg2!=" ")
    {
        if(arg2 == "2.000MSPS")//判断采样点数
            changeRate = "07D0";
        else if(arg2 =="1.333MSPS")
           changeRate = "0535";
        else if(arg2 =="0.667MSPS")
           changeRate = "029B";
        else if(arg2 =="0.333MSPS")
           changeRate = "014D";
    }
    else
        changeRate = "07D0";
    daChannelChoose();

    datas = begin+changeRate+distinguish+channel+last;
    qDebug()<<datas;
    MprocessHandle->getClientHandle(ui->tabWidget->currentIndex())->sendData(multifunc::HexStrToByteArray(datas));
    ui->infoShow->append("["+multifunc::GetLongDateTime()+"]"+"DA已配置");
}

void MultiChannel::adRateChoose()//采样率选择函数
{
    if(ui->adRate->currentText()!= " ")
    {
        if(ui->adRate->currentText()=="2.117MSPS")
            rate = "0845";
        else if(ui->adRate->currentText()=="1.411MSPS")
            rate = "0583";
        else if(ui->adRate->currentText()=="0.705MSPS")
            rate = "02C1";
        else if(ui->adRate->currentText()=="0.352MSPS")
            rate = "0160";
    }
    else
        rate = "0845";
}

void MultiChannel::adChannelChoose()//通道选择子函数
{
    int adChannel[8]={0,0,0,0,0,0,0,0};
    adChannelSelectSum=0;
    if(ui->adC1->isChecked()) {adChannel[7]=1;adChannelSelectSum++;}
    if(ui->adC2->isChecked()) {adChannel[6]=1;adChannelSelectSum++;}
    if(ui->adC3->isChecked()) {adChannel[5]=1;adChannelSelectSum++;}
    if(ui->adC4->isChecked()) {adChannel[4]=1;adChannelSelectSum++;}
    if(ui->adC5->isChecked()) {adChannel[3]=1;adChannelSelectSum++;}
    if(ui->adC6->isChecked()) {adChannel[2]=1;adChannelSelectSum++;}
    if(ui->adC7->isChecked()) {adChannel[1]=1;adChannelSelectSum++;}
    if(ui->adC8->isChecked()) {adChannel[0]=1;adChannelSelectSum++;}

    int data=adChannel[0];//data为十进制
    for(int i=1;i<8;i++)
    {
        data=(data<<1)|adChannel[i];
    }
    channel=multifunc::IntDecToStrHex(data);
    qDebug()<<channel;
}

void MultiChannel::daChannelChoose()//da通道选择子函数
{
    int daChannel[8]={0,0,0,0,0,0,0,0};
    if(ui->daC1->isChecked()) daChannel[7]=1;
    if(ui->daC2->isChecked()) daChannel[6]=1;
    if(ui->daC3->isChecked()) daChannel[5]=1;
    if(ui->daC4->isChecked()) daChannel[4]=1;
    if(ui->daC5->isChecked()) daChannel[3]=1;
    if(ui->daC6->isChecked()) daChannel[2]=1;
    if(ui->daC7->isChecked()) daChannel[1]=1;
    if(ui->daC8->isChecked()) daChannel[0]=1;

    int data=daChannel[0];//data为十进制
    for(int i=1;i<8;i++)
    {
        data=(data<<1)|daChannel[i];
    }
    channel=multifunc::IntDecToStrHex(data);
}

void MultiChannel::on_setPath_clicked()//更改文件存储路径
{
    //文件夹路径
   filePath = QFileDialog::
           getExistingDirectory(this,tr("设置文件保存目录"),"/", QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
   if(filePath.size() == 0)
       return;
   filePath += "/";
   ui->currentPath->setText(filePath);
}
