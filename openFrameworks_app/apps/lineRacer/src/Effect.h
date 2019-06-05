#pragma once
#include "ofMain.h"

class Effect
{
public:
	Effect();
	~Effect();

	void setup(string name, int runTime, ofImage icon, float dMultiplicator, bool fixedSpeed, float speed);

	virtual void applyEffect(ofParameter<float>& multiplicator, ofParameter<bool>& isFixed, ofParameter<float>& speed);

	string getRuntimeString();

	string name;
	long startTime;
	int runTime; // 0 runs forever
	ofImage icon;

	float dMultiplicator = 0;
	bool fixedSpeed = false;
	float speed = 0;
};

