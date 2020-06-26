#include "widget.h"
#include <QApplication>
#include "draughtserver.h"
#include "QDebug"
#include "DraughtManager.h"
#include "cstdio"
int chessboard[10][10];
bool isking[40];
int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    /*
    DraughtManager man;

    memset(isking, 0, sizeof(isking));
    for (int i = 0; i<10; i++) {
        for (int j = 0; j<10; j++) {
            chessboard[i][j]=-1;

        }

    }
    chessboard[2][2] = 0;
    chessboard[4][2] = 1;
    chessboard[1][5] = 21;
    chessboard[3][5] = 2;
    chessboard[7][5] = 3;
    chessboard[9][3] = 22;
    isking[22] = true;
    for (int i = 9; i >= 0; i--) {
        for (int j = 0; j < 10; j++) {
            int chess = chessboard[i][j];
            if (chess < 0) printf("xx ");
            else printf((chess<10?"0%d ":"%d "),chess);
        }
        printf("\n");

    }
    fflush(stdout);
    //DraughtManager man;
    auto vec=man.SolveTree(chessboard, 1, isking);

    Widget w;
    w.setGeometry(100,100,800,600);
    w.show();
    DraughtServer* server=new DraughtServer(NULL);
    server->initialize();
    server->startServer();
    qDebug()<<"Port:"<<server->getPort();

    //return 0;
    printf("Here!");
    QString str=man.serialize(vec);
    qDebug()<<str;
    auto vec2=man.deserialize(str);
    printf("Here!");
    */
    Widget w;
    w.setGeometry(100,100,800,600);
    w.show();
    return a.exec();
}
