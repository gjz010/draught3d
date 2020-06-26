#include "draughtserver.h"
#include "QTcpSocket"
#include "packetdecoder.h"
#include "QDebug"
#include "DraughtManager.h"
#include <random>
#include "QVector2D"
std::random_device rd;

struct DraughtServer::Internal{
    DraughtManager* current;
    PacketDecoder* pm[2];
    bool game_started=false;
    bool game_hung=false;
    int game_counter=0;
    int c1_side=0;
    int current_client=0;
    std::vector<DraughtManager::Step*> chess[40];
    DraughtManager::Step* current_step=nullptr;
    int side(){
        return current_client==0?c1_side:(1-c1_side);
    }

};
DraughtServer::DraughtServer(QObject* parent):QObject(parent)
{
    data=new Internal;
    memset(clients,0,sizeof(clients));
}

void DraughtServer::initialize()
{
    server=new QTcpServer(this);
    data->current=new DraughtManager();

}

void DraughtServer::startServer(int port)
{
    server->listen(QHostAddress::Any,port);
    this->port=server->serverPort();
    connect(server,&QTcpServer::newConnection,this,&DraughtServer::onConnection);
    qDebug("Draught Server launched!");

}

int DraughtServer::getPort()
{
    return port;
}

DraughtServer::~DraughtServer()
{
    delete data;
}

void DraughtServer::sendPacket(int idx, QString head, QString body)
{
    qDebug()<<QString("Server sending to %3 :%1|%2").arg(head).arg(body).arg(idx);
    data->pm[idx]->sendPacket(QString("%1|%2").arg(head).arg(body));
}

void DraughtServer::fill(int *board, bool *king)
{
    memcpy(data->current->chessboard,board,sizeof(data->current->chessboard));
    memcpy(data->current->isking,king,sizeof(data->current->isking));
}
DraughtManager::Step* DraughtServer::validate(int idx,int target){
    if(data->current_step!=nullptr){
        if(idx!=data->current_step->src) return nullptr;
        for(DraughtManager::Step* step:data->current_step->substeps){
            if(step->land_place==target) return step;

        }
        return nullptr;

    }else{
        auto& vec=data->chess[idx];
        for(DraughtManager::Step* step:vec){
            if(step->land_place==target) return step;

        }
        return nullptr;
    }

}

void DraughtServer::writeInit(int idx)
{
    QStringList list;
    for(int i=0;i<10;i++){
        for(int j=0;j<10;j++){
            list.append(QString("%1").arg(data->current->chessboard[i][j]));


        }
    }
    for(int i=0;i<40;i++){
        list.append((data->current->isking[i]?"1":"0"));

    }
    QString str(list.join(","));
    sendPacket(idx,"Init",str);

}
void DraughtServer::getAndPost(int idx,int target)
{

    if(target==-1){//

        fflush(stdout);
        auto vec=data->current->SolveTree(data->current->chessboard,data->side(),data->current->isking);
        if(vec.empty()){
            for(int i=0;i<2;i++){
                sendPacket(i,"GameFinish",QString("%1").arg(1-data->side()));
            }
            data->game_started=false;
            //return;

        }
        for(int i=0;i<40;i++){
            for(DraughtManager::Step* step:data->chess[i]) delete step;
            data->chess[i].clear();
        }
        for(std::vector<DraughtManager::Step*>& choice:vec){
            int idx=choice[0]->src;
            data->chess[idx]=std::move(choice);
        }

    }else{
        DraughtManager::Step* step=validate(idx,target);
        if(step==nullptr) return;
        for(int i=0;i<2;i++){
            sendPacket(i,"AnimateStep",QString("%1,%2,%3").arg(step->src).arg(step->land_place).arg(step->killing_place));
        }

        for(int i=0;i<10;i++){
            for(int j=0;j<10;j++){
                if(data->current->chessboard[i][j]==step->src) data->current->chessboard[i][j]=-1;

            }

        }
        data->current->chessboard[step->killing_place/10][step->killing_place%10]=-1;
        data->current->chessboard[step->land_place/10][step->land_place%10]=step->src;


        if(step->substeps.size()==0){
            //Switch side!
            data->current_client=1-data->current_client;
            qDebug("Side Switch!");
            qDebug()<<data->side();
            for (int i = 9; i >= 0; i--) {
                for (int j = 0; j < 10; j++) {
                    int chess = data->current->chessboard[i][j];
                    if (chess < 0) printf("xx ");
                    else printf((chess<10?"0%d ":"%d "),chess);
                }
                printf("\n");

            }

            auto vec=data->current->SolveTree(data->current->chessboard,data->side(),data->current->isking);
            if(vec.empty()){
                for(int i=0;i<2;i++){
                    sendPacket(i,"GameFinish",QString("%1").arg(1-data->side()));
                }
                data->game_started=false;

            }else{


            }
            for(int i=0;i<40;i++){
                for(DraughtManager::Step* step:data->chess[i]) delete step;
                data->chess[i].clear();
            }
            data->current_step=nullptr;
            for(std::vector<DraughtManager::Step*>& choice:vec){
                int idx=choice[0]->src;
                data->chess[idx]=std::move(choice);
            }

            for(int i=0;i<10;i++){
                if(data->current->chessboard[0][i]>=20 && data->current->chessboard[0][i]<40) data->current->isking[data->current->chessboard[0][i]]=true;
                if(data->current->chessboard[9][i]>=0 && data->current->chessboard[9][i]<20) data->current->isking[data->current->chessboard[9][i]]=true;
            }
            for(int i=0;i<2;i++){
                sendPacket(i,"AnimateStepFinish",QString("0"));
            }
        }else{
            data->current_step=step;

        }
    }
    QStringList list;

    if(data->current_step!=nullptr){
        for(int i=0;i<40;i++){
            if(i!=data->current_step->src) list.append("0");
            else{
                list.append(QString("%1").arg(data->current_step->substeps.size()));
                for(DraughtManager::Step* step:data->current_step->substeps){
                    list.append(QString("%1,%2").arg(step->land_place).arg(step->killing_place));

                }

            }

        }

    }else{


        for(int i=0;i<40;i++){
            auto& vec=data->chess[i];
            list.append(QString("%1").arg(vec.size()));
            for(DraughtManager::Step* step:vec){
                list.append(QString("%1,%2").arg(step->land_place).arg(step->killing_place));

            }


        }


    }
    QString reqstep(list.join(","));
    sendPacket(data->current_client,"ReqStep",reqstep);

}

void DraughtServer::onConnection()
{
    QTcpSocket* socket=server->nextPendingConnection();
    if(client_count>=2){
        socket->close();
        return;
    }
    clients[client_count]=socket;
    PacketDecoder* decoder=new PacketDecoder(socket,client_count);
    connect(decoder,&PacketDecoder::onPacket,this,&DraughtServer::onPacket);
    connect(socket,&QTcpSocket::disconnected,this,&DraughtServer::onDisconnect);
    data->pm[client_count]=decoder;
    client_count++;

}

void DraughtServer::onPacket(int idx, QString str)
{
    qDebug()<<idx<<":"<<str;
    int index=str.indexOf('|');
    QString header=str.left(index);
    QString tail=str.right(str.size()-index-1);
    qDebug()<<"Head:"<<header<<"Body"<<tail;
    if(header=="Fuckyou"){
        writeInit(idx);
        if(client_count==2){
            data->game_started=true;
            if(pref==0) data->c1_side=0;
            if(pref==1) data->c1_side=1;
            if(pref==2) data->c1_side=rd()&1;
            sendPacket(0,"SetSide",QString("%1").arg(data->c1_side));
            sendPacket(1,"SetSide",QString("%1").arg(1-data->c1_side));

            data->current_client=data->c1_side;

            getAndPost(-1,-1);
        }

    }
    if(data->game_started){
        if(data->game_hung){
            if(idx!=data->current_client){
                if(header=="DrawAns"){
                    if(tail=="1"){
                        for(int i=0;i<2;i++){
                            sendPacket(i,"Draw","0");

                        }
                        data->game_started=false;

                    }else{
                        sendPacket(data->current_client,"DrawRejected","0");

                    }
                    data->game_hung=false;

                }

            }


        }else{
            if(header=="ChooseStep" && idx==data->current_client){
                qDebug("Hello!");
                QStringList list=tail.split(",");
                int pidx=list[0].toInt();
                int target=list[1].toInt();
                getAndPost(pidx,target);

            }
            if(idx==data->current_client){
                int cidx=1-idx;
                if(header=="DrawReq"){
                    data->game_hung=true;
                    sendPacket(cidx,"DrawReq","0");
                }
                if(header=="Resign"){
                    for(int i=0;i<2;i++){
                        sendPacket(i,"GameFinish",QString("%1").arg(1-data->side()));
                    }
                    data->game_started=false;

                }


            }

        }


    }

}

void DraughtServer::onDisconnect()
{
    QTcpSocket* socket=(QTcpSocket*)this->sender();
    for(int i=0;i<2;i++){
        if(socket!=clients[i]){
            if(clients[i]==nullptr) continue;
            clients[i]->close();
            delete data->pm[i];
            clients[i]=nullptr;

        }

    }
    client_count=0;
}

