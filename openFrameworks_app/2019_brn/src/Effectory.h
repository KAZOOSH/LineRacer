#pragma once
#include "Effect.h"

class Effectory
{
public:
	Effectory();
	~Effectory();

	static Effect getEffect(string name, ofJson config);

private:
	static ofImage loadImage(string path);
};

