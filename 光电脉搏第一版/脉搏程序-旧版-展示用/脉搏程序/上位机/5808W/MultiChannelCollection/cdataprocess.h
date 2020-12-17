﻿#ifndef CDATAPROCESS_H
#define CDATAPROCESS_H


#include <QThread>
#include <QSemaphore>
#include <QQueue>
#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include "cfilehandle.h"


class CdataProcess : public QThread
{
    Q_OBJECT

public:
    CdataProcess();
    ~CdataProcess();
    void run(); //只有run里面的内容才会在子进程中运行
    CfileHandle  *fileHandle;


public slots:
    void dataIn(QByteArray sInData);
    void dataOut();

signals:
    void signalWriteFile(QByteArray fileData);

private:
   const static int buffSize   = 1000000;  //数据缓存区大小

    QMutex filein_mutex;
    QMutex fileout_mutex;
    QQueue<QByteArray> socketData;//qqueue类是提供队列的通用容器

    //P-C模式信号量
    QSemaphore freeData;  //producer
    QSemaphore usedData;  //consumer


};

#endif // CDATAPROCESS_H
