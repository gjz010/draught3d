#include "packetdecoder.h"
#include "QTextCodec"
#include "QTextDecoder"
PacketDecoder::PacketDecoder(QTcpSocket* socket,int identifier):socket(socket),QObject(socket),id(identifier)
{
    connect(socket,&QTcpSocket::readyRead,this,&PacketDecoder::readData);
    array=new QByteArray;
}

void PacketDecoder::sendPacket(const QString &str)
{
    QByteArray arr=str.toUtf8();
    QByteArray result=QString("%1|").arg(arr.length()).toUtf8();
    result.append(arr);
    socket->write(result);
    socket->flush();
}

int PacketDecoder::getId()
{
    return id;
}

void PacketDecoder::readData()
{
    QByteArray arr=socket->readAll();
    for(int i=0;i<arr.length();i++){
        char chr=arr.at(i);
        if(state==0){
            if(chr=='|'){
                if(counter>0)
                state=1;
                else{
                    qDebug("Zero-len packet detected, ignoring...");

                }

            }else{
                int delta=chr-'0';
                if(delta<0 || delta>9){
                    qDebug("Wrong packet! restarting...");
                    state=0;
                    counter=0;
                }else{
                    counter=counter*10+delta;

                }

            }

        }else if(state==1){
            array->append(chr);
            counter--;
            if(counter==0){
                state=0;
                QString recv=QString::fromUtf8(*array);
                array->clear();
                emit onPacket(id,recv);
            }

        }
    }
}
