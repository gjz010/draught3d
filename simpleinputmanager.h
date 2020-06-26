#ifndef SIMPLEINPUTMANAGER_H
#define SIMPLEINPUTMANAGER_H

#include <QObject>
#include <QVector4D>
/*
 * This is a simple input manager which handles only mouse/touch input.
 *
 *
 */
class SimpleInputManager : public QObject
{
    Q_OBJECT
public:
    explicit SimpleInputManager(QObject *parent = nullptr);
    ~SimpleInputManager();
    enum Key{
        UP=0,DOWN,LEFT,RIGHT

    };

signals:

public slots:
    void onMouseDown(int,int,QVector4D);
    void onMouseMove(int,int,QVector4D);
    void onMouseUp(int,int,QVector4D);
    void onMouseWheel(int delta);
    void onKeyDown(Key key);
    void onKeyUp(Key key);
    void translateKey(int keyid,bool down);
private:
    bool isdown=false;
    bool istap=false;
    int x;
    int y;
    int tapx;
    int tapy;
    int width=1;
    int height=1;
    QVector4D clicked_meta;
    QVector4D mouse_meta;
    bool isroll=false;
    int roll_delta;
    bool* key_state;
public:
    int getX() const;
    int getY() const;
    bool isDown();
    bool isTap();
    bool checkTap() const;
    void clearTap();
    int getTapX() const;
    int getTapY() const;
    QVector4D getTapMeta() const;
    QVector4D getMeta() const;
    float getXPos() const;
    float getYPos() const;
    float getTapXPos() const;
    float getTapYPos() const;
    bool isRolling();
    bool checkRolling() const;
    int getRollDelta();
    void setViewport(int,int);
    int getViewWidth();
    int getViewHeight();

    bool isKeyDown(Key idx);
};

#endif // SIMPLEINPUTMANAGER_H
