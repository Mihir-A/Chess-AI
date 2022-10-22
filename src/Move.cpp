#include "Move.h"

Move::Move(Spot startS, Spot endS)
    :start(startS), end(endS)
{
}

const Spot& Move::getStart()
{
    return start;
}

const Spot& Move::getEnd()
{
    return end;
}
