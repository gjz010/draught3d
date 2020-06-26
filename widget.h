#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include "QOpenGLFunctions"
#include <QOpenGLFunctions_4_2_Core>
#include "glmanager.h"
#include "QMouseEvent"
#include "simpleinputmanager.h"

class Widget : public QOpenGLWidget,public QOpenGLFunctions_4_2_Core
{
    Q_OBJECT

public:
    explicit Widget(QOpenGLWidget *parent = 0);
    ~Widget();
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    QVector3D getClickedMeta(int x,int y);
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    void keyReleaseEvent(QKeyEvent*) override;
    void paintEvent(QPaintEvent*) override;
    void renderModel(ModelData md,QMatrix4x4 modelMat,QVector4D meta,QString texture,QOpenGLShaderProgram* prog,Material);
    SimpleInputManager* input();
private:
    GLManager* gl;
    struct Internal;
    Internal* data;


public slots:
    void doFrame();
};

#endif // WIDGET_H
