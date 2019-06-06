#pragma once
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSerial.h"
#include "Effect.h"
#include "Effectory.h"

class BikeControl
{
public:
	BikeControl();
	~BikeControl();

	void setup(int id, ofxIO::BufferedSerialDevice* serial,ofVec2f posGui, ofJson effectConfig);
	void update();
	void draw();

	ofParameterGroup params;
	ofParameter<float> speedBase;
	ofParameter<float> multiplicatorBase;
	ofParameter<bool> isFixedBase;
	ofParameter<float> speed;
	ofParameter<float> multiplicator;
	ofParameter<bool> isFixed;
	ofParameter <string> name;


	void setMultiplicator(float mult);
	void setSpeed(float speed);
	void setFixedSpeed(bool isFixed);

	void addInterim(int time);
	vector<int> getInterims();
	void clearInterims();

	int getTotalTime();
	int getId();

	void reset();
	void start();

	void addEffect(string name);

	ofImage picture;

	//effects
	void onFixedSpeed(bool & isFixed);
	void onEPowerBar();

private:
	ofxIO::BufferedSerialDevice* serial;
	int id;
	ofxPanel gui;

	vector<int> interims;
	long tStart;
	

	vector<Effect> effects;
	ofJson effectConfig;

	// gui
	ofxButton bPowerBar;
};

