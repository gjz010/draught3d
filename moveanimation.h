#ifndef MOVEANIMATION_H
#define MOVEANIMATION_H
#include "animation.h"

class MoveAnimation : public Animation
{
public:
    MoveAnimation();
    int chess;
    int src;
    int target;
    float percentage;
    bool leap;
    bool tick();
};

#endif // MOVEANIMATION_H
