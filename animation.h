#ifndef ANIMATION_H
#define ANIMATION_H


class Animation
{
public:
    Animation();
    int type=-1;
    virtual bool tick()=0; //true for animation done
};

#endif // ANIMATION_H
