#include "serverdialog.h"
#include "ui_serverdialog.h"
#include "draughtserver.h"
#include "scenemenu.h"
#include "QNetworkInterface"
#include "QHostAddress"
int tmpboard[10][10];
bool tmpking[40];
ServerDialog::ServerDialog(SceneMenu* game,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerDialog),game(game)
{
    this->setAttribute( Qt::WA_DeleteOnClose );
    ui->setupUi(this);
    QStringList slist;
    QString str="Local Ip:";
    slist.append(str);
    QList<QHostAddress> list = QNetworkInterface::allAddresses();

     for(int nIter=0; nIter<list.count(); nIter++)

      {
          if(!list[nIter].isLoopback())
              if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol )
                slist.append(list[nIter].toString());

      }
     ui->localIp->setText(QString(slist.join("\n")));
}

ServerDialog::~ServerDialog()
{
    delete ui;
}

void ServerDialog::accept()
{

    DraughtServer* server=new DraughtServer(this->game);

    server->initialize();
    server->startServer(ui->boxPort->value());

    int u=2;
    if(ui->first->isChecked()) u=0;
    if(ui->second->isChecked()) u=1;
    if(ui->random->isChecked()) u=2;
    server->pref=u;
    QString board=ui->board->toPlainText();
    QTextStream stream(&board);

    int index[2]={0,20};
    //memset(tmpking,0,sizeof(tmpking));
    for(int i=9;i>=0;i--){
        for(int j=0;j<10;j++){
            int number;
            stream>>number;
            number--;

            if(number==-1) tmpboard[i][j]=-1;
            else{

                int team=number%10;
                qDebug()<<"Team:"<<team;
                bool king=number>=10;
                int team_idx=(index[team]++);

                tmpboard[i][j]=team_idx;

                tmpking[team_idx]=king;
                qDebug()<<team_idx;

            }



        }

    }

    server->fill((int*)&tmpboard,(bool*)&tmpking);

    game->connectToServer("127.0.0.1",ui->boxPort->value());

    QDialog::accept();
}
