#include "dieanimation.h"
#include <cstring>
#include <QDebug>
DieAnimation::DieAnimation()
{
    type=2;
    memset(dying_mask,0,sizeof(dying_mask));
    percentage=0.f;
}

bool DieAnimation::tick()
{
    //qDebug("Die animation ticking!");
    percentage+=0.048;
    if(percentage>1.f) return true;
    return false;
}
