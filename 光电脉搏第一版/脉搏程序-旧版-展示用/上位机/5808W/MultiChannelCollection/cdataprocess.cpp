#include "cdataprocess.h"
#include <QDebug>


CdataProcess::CdataProcess():freeData(buffSize)  //只能用这种构造方式对该信号量初始化？//Qsemaphore缺点?
{
    fileHandle = new CfileHandle();
    qDebug()<<"creat dataProcess handle";
     //dataHandle.transType = 0;
    // dataHandle.endFlag = 0;
    qDebug()<<freeData.available();//返回信号量当前可用的资源数
}

CdataProcess::~CdataProcess()
{

}


void CdataProcess::run()
{
    while(1)
    {
        dataOut();
    }
}

void CdataProcess::dataIn(QByteArray sInData)//producer
{
    QMutexLocker locker(&(filein_mutex));
    freeData.acquire();
   // qDebug()<<"fr"<<freeData.available();
   // qDebug()<<sData;
    socketData.enqueue(sInData);
    usedData.release();

   // qDebug()<<usedData.available();
}

void CdataProcess::dataOut()//consumer
{
    QMutexLocker locker(&(fileout_mutex));
    usedData.acquire();//获取资源
    QByteArray sOutdata;
    sOutdata = socketData.dequeue();//删除队列中的头项并返回它
   // qDebug()<<"deq = "<<sOutdata.size();

    fileHandle->WriteFile(sOutdata);

   // qDebug()<<"to write";
    freeData.release();
   // qDebug()<<"endflag"<<dataHandle.endFlag;

}
