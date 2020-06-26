#ifndef DRAUGHTSERVER_H
#define DRAUGHTSERVER_H

#include <QObject>
#include "QTcpServer"
#include "QTcpSocket"
#include "QSignalMapper"
//Let's implement the C/S in the most simple way - dumb client (i.e. ofo client).
//This means server does all the calculation and client only does the showing work.
//In draught this means server giving out all choices and client rendering and giving choice back.
#include "DraughtManager.h"
class DraughtServer : public QObject
{
    Q_OBJECT
public:
    DraughtServer(QObject* parent);
    void initialize();
    void startServer(int port=0);
    int getPort();
    ~DraughtServer();
    void sendPacket(int idx,QString head,QString body);
    void fill(int* board,bool* king);
    int pref=2;
private:
    QTcpServer* server;
    int port=0;
    bool thishand=0;
    QTcpSocket* clients[2];
    QSignalMapper* mapper; //a lazy and slow implementation
    int client_count=0;
    struct Internal;
    Internal* data;
    void getAndPost(int idx,int target);
    DraughtManager::Step* validate(int idx,int target);
    void writeInit(int idx);

signals:
    void serverLaunched();
private slots:
    void onConnection();
    void onPacket(int idx,QString str);
    void onDisconnect();
};

#endif // DRAUGHTSERVER_H
