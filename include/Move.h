#pragma once
#include "Spot.h"
class Move
{
private:
	const Spot &start, &end;
public:
	Move(Spot startS, Spot endS);
	const Spot& getStart();
	const Spot& getEnd();
};

