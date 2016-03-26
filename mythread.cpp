// mythread.cpp
#include <string>
#include "mythread.h"
#include "myserver.h"
#include <QtNetwork>



QByteArray ClientsData[10];
int ClientsIDs[10];
int NumberOfClients;
int ClientsMask[10];
long ClientTimeOut[10];
long ClientLastTime[10];


MyThread::MyThread(qintptr ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
}


void MyThread::run()
{

    socket = new QTcpSocket();


    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.


    int i,j;
    i=0;
    j=10;
    for (i=0;i<10;i++)
     {
        if ((ClientsIDs[i]==1) && (timeOut1.elapsed()-ClientTimeOut[i]>6000))
            {
             ClientsIDs[i]=0;NumberOfClients--;
             if (i<j) j=i;
            }
     }

    if (NumberOfClients<10)
    {
     i=0;
     while ((i<10) && (ClientsIDs[i]!=0)) i++;
     ClientsIDs[i]=socketDescriptor;
     ClientLastTime[i]=timeOut1.elapsed();
     socket->write("***");
     socket->write(QByteArray::number(i));
     NumberOfClients++;
     connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
     connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

     // We'll have multiple clients, we want to know which is which

     // make this thread a loop,
     // thread will stay alive so that signal/slot to function properly
     // not dropped out in the middle when thread dies

     exec();
    }
    else
        {
        }
}

void MyThread::readyRead()
{
    // get the information


    QByteArray Data = socket->readAll();
    int i,j,k;
    i=0;j=0;
    while ((i<10) && (ClientsIDs[i]!=socketDescriptor)) i++;
    if (i<10)
    {
        j=i;
        ClientsData[i]=Data;
        ClientLastTime[i]=timeOut1.elapsed();
    }

    for(i=0;i<10;i++)
    {
    if ((ClientsIDs[i]>1) &&  (timeOut1.elapsed()-ClientLastTime[i]>5000))
        {
        qDebug() << "Client"<<i<< " connection lost, disconnecting..";
        ClientsIDs[i]=1; //disconnected not free yet
        ClientsMask[i]=0;
        for (k=0;k<10;k++)
            {
             if (ClientsIDs[k]>1) ClientsMask[i]|=(1<<k);
            }
        ClientTimeOut[i]=timeOut1.elapsed();
        }
    if (ClientsIDs[i]>1) {socket->write(ClientsData[i]);}
    if (ClientsIDs[i]==1)
        {
        if (timeOut1.elapsed()-ClientTimeOut[i]>3000)
        {
            ClientsIDs[i]=0;NumberOfClients--;
        }
        else
         {
        sprintf(tmp1, "*%u1#Disconnected*%u2#0*%u3#0*%u4#0*", i,i,i,i);
        socket->write(tmp1);
        ClientsMask[i]&=  (~ (1<<j));
        if (ClientsMask[i]==0)
            {
            ClientsIDs[i]=0;
            NumberOfClients--;
            }
        }
        }
    }

}

void MyThread::disconnected()
{
    int i,j;
    i=0;
    while ((i<10) && (ClientsIDs[i]!=socketDescriptor)) i++;
    if (i<10)
    {
    ClientsIDs[i]=1; //disconnected not free yet
    ClientsMask[i]=0;
    for (j=0;j<10;j++)
        {
         if (ClientsIDs[j]>1) ClientsMask[i]|=(1<<j);
        }
    ClientTimeOut[i]=timeOut1.elapsed();
    socket->deleteLater();
    exit(0);
    }
}
