#include "cfilehandle.h"

QString CfileHandle::filePath = QDir::currentPath();  //静态成员需要在类定义体外定义
CfileHandle::CfileHandle(QObject *parent) : QObject(parent),startFlag(true),DataSize(0),fileCount(1),fileSize(0)
{

}

void CfileHandle::WriteFile(QByteArray fileData)//接收下位机数据写进文件里
{
    if(startFlag == true)//解析帧头信息
    {
       qDebug()<<fileData;
        if(fileData.startsWith(QByteArray("\x41\x44\x43"))&&fileData.size()==1024)
        {
            bool ok;
            DataSize = fileData.mid(9,4).toHex().toInt(&ok,16);
            QString fileName = filePath+"/Data"+QDateTime::currentDateTime().toString("yyyyMMddhhmmss.")+QString("%1").arg((fileCount),10,10,QChar('0'))+QString(".txt");
            qDebug()<<"DataSize = " <<DataSize;
            emit signalShowState("正在接收数据,数据大小: "+QString::number(DataSize));//解析帧尾，得到文件大小
            file = new QFile(fileName);//创建文件
            file->open(QIODevice::WriteOnly);
            file->write(fileData,fileData.size());
            fileSize += fileData.size();
            startFlag = false;
        }
    }
    else
    {
        qDebug()<<"进入文件写过程";
        file->write(fileData,fileData.size());
        file->flush();//将任何缓冲数据刷新到文件。如果成功，则返回true；否则返回false。
        fileSize += fileData.size();
        if(fileSize >= DataSize+1024)//如果写的文件大小等于接收的文件大小加上数据包头大小，则接收完毕
        {
            qDebug()<<"close file";
            emit signalShowState("已接收数据大小: "+QString::number(fileSize));
            file->close();
             startFlag = true;
             emit signalShowState("数据接收完成!");
            fileSize = 0;
        }
    }
}

