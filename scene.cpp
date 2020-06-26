#include "scene.h"

Scene::Scene(GLManager *gl, Widget *parent) : QObject(parent)
{
    this->gl=gl;
    this->context=parent;
}
