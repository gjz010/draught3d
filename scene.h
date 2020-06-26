#ifndef SCENE_H
#define SCENE_H
#include "glmanager.h"
#include <QObject>
#include "widget.h"

class Scene : public QObject
{
    Q_OBJECT
public:
    explicit Scene(GLManager* gl,Widget *parent = nullptr);

signals:

public slots:
    virtual void render()=0;
    virtual void act()=0;
    virtual void paint()=0;
protected:
    GLManager* gl;
    Widget* context;
};

#endif // SCENE_H
