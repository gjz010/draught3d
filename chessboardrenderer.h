#ifndef CHESSBOARDRENDERER_H
#define CHESSBOARDRENDERER_H

#include <QObject>
#include "chessboard.h"
#include "scene.h"
class ChessBoardRenderer : public Scene
{
    Q_OBJECT
public:
    explicit ChessBoardRenderer(GLManager *gl, Widget *parent);
    void act();
    void draw();
    void paint();
signals:

public slots:
};

#endif // CHESSBOARDRENDERER_H
