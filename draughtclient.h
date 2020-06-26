#ifndef DRAUGHTCLIENT_H
#define DRAUGHTCLIENT_H
#include "QTcpSocket"
#include "packetdecoder.h"
#include "QObject"

class DraughtClient :public QObject
{
   Q_OBJECT
public:
    DraughtClient();
    void connectHost(QString ip,int port);
    void sendPacket(QString head,QString body);
private slots:
    void onPacket(int id,QString data);
    void onError(QAbstractSocket::SocketError err);
private:
    QTcpSocket* socket;
    PacketDecoder* decoder;
signals:
    void packet(QString head,QString body);
    void ready();
    void disconnected();
};

#endif // DRAUGHTCLIENT_H
