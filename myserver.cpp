// myserver.cpp

#include "myserver.h"
#include "mythread.h"




MyServer::MyServer(QObject *parent) :
    QTcpServer(parent)
{
}

void MyServer::startServer()
{
    timeOut1.start();
    int port = 1234;

    if(!this->listen(QHostAddress::Any,port))
    {
    }
}

void MyServer::incomingConnection(qintptr socketDescriptor)
{
    MyThread *thread = new MyThread(socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}
