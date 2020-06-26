#ifndef DIEANIMATION_H
#define DIEANIMATION_H
#include <vector>
#include "animation.h"
class DieAnimation : public Animation
{
public:
    DieAnimation();
    float percentage;
    bool dying_mask[40];
    bool tick();
};

#endif // DIEANIMATION_H
