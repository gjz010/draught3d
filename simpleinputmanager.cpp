#include "simpleinputmanager.h"
#include "QDebug"
SimpleInputManager::SimpleInputManager(QObject *parent) : QObject(parent),istap(false),isdown(false)
{
    key_state=new bool[100];
    memset(key_state,0,sizeof(bool)*100);
}

SimpleInputManager::~SimpleInputManager()
{
    delete key_state;
}

void SimpleInputManager::onMouseDown(int x,int y,QVector4D meta)
{
    isdown=true;
    onMouseMove(x,y,meta);
}

void SimpleInputManager::onMouseMove(int x, int y,QVector4D meta)
{
    this->x=x;
    this->y=y;
    this->mouse_meta=meta;
    //qDebug()<<meta;
}

void SimpleInputManager::onMouseUp(int x, int y,QVector4D meta)
{
    isdown=false;
    istap=true;
    tapx=x;
    tapy=y;
    this->clicked_meta=meta;
    onMouseMove(x,y,meta);
}

void SimpleInputManager::onMouseWheel(int delta)
{
    isroll=true;
    roll_delta=delta;
}

void SimpleInputManager::onKeyDown(SimpleInputManager::Key key)
{
    key_state[key]=true;
}

void SimpleInputManager::onKeyUp(SimpleInputManager::Key key)
{
    key_state[key]=false;
}

void SimpleInputManager::translateKey(int keyid, bool down)
{
    Key key=(Key)99;
    if(keyid==Qt::Key_Up) key=Key::UP;
    if(keyid==Qt::Key_Down) key=Key::DOWN;
    if(keyid==Qt::Key_Left) key=Key::LEFT;
    if(keyid==Qt::Key_Right) key=Key::RIGHT;
    (down?onKeyDown(key):onKeyUp(key));
}

int SimpleInputManager::getX() const
{
    return x;
}

int SimpleInputManager::getY() const
{
    return y;
}


bool SimpleInputManager::isDown()
{
    return isdown;
}

bool SimpleInputManager::isTap()
{
    bool t=istap;
    istap=false;
    return t;

}

bool SimpleInputManager::checkTap() const
{
    return istap;
}

void SimpleInputManager::clearTap()
{
    istap=false;
}

float SimpleInputManager::getXPos() const
{
    return (float)x/width;
}

float SimpleInputManager::getYPos() const
{
    return (float)y/height;
}

float SimpleInputManager::getTapXPos() const
{
    return (float)tapx/width;
}

float SimpleInputManager::getTapYPos() const
{
    return (float)tapy/height;
}

bool SimpleInputManager::isRolling()
{
    bool result=isroll;
    isroll=false;
    return result;
}

bool SimpleInputManager::checkRolling() const
{
    return isroll;
}

int SimpleInputManager::getRollDelta()
{
    return roll_delta;
}

void SimpleInputManager::setViewport(int w, int h)
{
    width=w;
    height=h;
}

int SimpleInputManager::getViewWidth()
{
    return width;
}

int SimpleInputManager::getViewHeight()
{
    return height;
}

bool SimpleInputManager::isKeyDown(SimpleInputManager::Key idx)
{
    return key_state[idx];
}

int SimpleInputManager::getTapX() const
{
    return tapx;
}

int SimpleInputManager::getTapY() const
{
    return tapy;
}

QVector4D SimpleInputManager::getTapMeta() const
{
    return clicked_meta;
}

QVector4D SimpleInputManager::getMeta() const
{
    return mouse_meta;
}
