#ifndef SCENEMENU_H
#define SCENEMENU_H

#include <QObject>
#include "scene.h"
class SceneMenu : public Scene
{
    Q_OBJECT
public:
    SceneMenu(GLManager *gl, Widget *parent);
    void act();
    void render();
    void paint();



    void connectToServer(QString ip,int port);
public slots:
    void onDialogClose();

    void connected();
    void onPacket(QString head,QString body);
    void disconnected();
private:
    struct Internal;
    Internal* data;
    void drawSituation(QMatrix4x4 model);
    void drawChessboard(QMatrix4x4 model);
    void drawChess(QMatrix4x4 model,int side,bool isking,QVector4D meta,float explosion_distance=0,QVector3D tone={1.f,1.f,1.f},float delta_em=0.f);

    void initBoard();

    void handleSituationRotation();
    void handleMenuAction();
    void renderMenu();
    void renderIngameMenu();
    void handleSituationStep();
    void handleIngameMenuAction();

    void holdDraw();
    void holdResign();

    void checkSteppable(int* array);
    void drawGreenGrid(QMatrix4x4 model,QVector4D meta);
};

#endif // SCENEMENU_H
