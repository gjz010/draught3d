#include "moveanimation.h"
#include <QDebug>
MoveAnimation::MoveAnimation()
{
    type=1;
    percentage=0.f;
    leap=false;
}

bool MoveAnimation::tick()
{
    //qDebug("Move animation ticking!");
    percentage+=0.096;
    if(percentage>1.f) return true;
    return false;
}
