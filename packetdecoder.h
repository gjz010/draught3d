#ifndef PACKETDECODER_H
#define PACKETDECODER_H
#include <QString>
#include <QObject>
#include "QTcpSocket"
class PacketDecoder : public QObject
{
    Q_OBJECT
public:
    explicit PacketDecoder(QTcpSocket* socket,int identifier);
    void sendPacket(const QString& str);
    int getId();
private:
    QByteArray* array;
    QTcpSocket* socket;
    int state=0; //0 for numbers 1 for data.
    int counter=0;
    int id;
signals:
    void onPacket(int id,QString str);
private slots:
    void readData();
};

#endif // PACKETDECODER_H
