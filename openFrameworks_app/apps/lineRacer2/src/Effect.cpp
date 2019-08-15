#include "Effect.h"



Effect::Effect()
{
}


Effect::~Effect()
{
}

void Effect::setup(string _name, int _runTime, ofImage _icon, float _dMultiplicator, bool _fixedSpeed, float _speed)
{
	name = _name;
	runTime = _runTime;
	icon = _icon;
	dMultiplicator = _dMultiplicator;
	fixedSpeed = _fixedSpeed;
	speed = _speed;
	startTime = ofGetElapsedTimeMillis();
}

void Effect::applyEffect(ofParameter<float>& multiplicator, ofParameter<bool>& isFixed, ofParameter<float>& _speed)
{
	multiplicator += dMultiplicator;
	if(!isFixed && fixedSpeed) isFixed = true;
	if (isFixed) _speed = speed;
}

string Effect::getRuntimeString()
{
	if (runTime == 0) return "";
	int tLeft = ofGetElapsedTimeMillis() - startTime - runTime;
	return ofToString(-1*(tLeft / 1000));
}
