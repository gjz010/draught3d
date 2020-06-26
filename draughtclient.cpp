#include "draughtclient.h"
#include "QHostAddress"
DraughtClient::DraughtClient()
{
    socket=new QTcpSocket(this);
}

void DraughtClient::connectHost(QString ip,int port)
{
    qDebug()<<ip<<port;

    decoder=new PacketDecoder(socket,0);
    connect(socket,static_cast<void (QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error),this,&DraughtClient::onError);
    connect(socket,&QTcpSocket::connected,this,&DraughtClient::ready);
    connect(decoder,&PacketDecoder::onPacket,this,&DraughtClient::onPacket);
    connect(socket,&QTcpSocket::disconnected,this,&DraughtClient::disconnected);


    socket->connectToHost(ip,port);
    socket->waitForConnected();
    qDebug()<<socket->state();
    //connect(socket,&QTcpSocket::destroyed,this,&QObject::deleteLater);
}

void DraughtClient::sendPacket(QString head, QString body)
{
    decoder->sendPacket(QString("%1|%2").arg(head,body));
}

void DraughtClient::onPacket(int id, QString data)
{
    int index=data.indexOf('|');
    QString header=data.left(index);
    QString tail=data.right(data.size()-index-1);
    emit packet(header,tail);
}

void DraughtClient::onError(QAbstractSocket::SocketError err)
{
    qDebug()<<err;
}
