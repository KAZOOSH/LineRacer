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
	vector<Effect> getEffects();

	ofImage picture;

	//effects
	void onFixedSpeed(bool & isFixed);
	void onEPowerBar();
	void onEBanane();
	void onEKlebePfeil();
	void onEAnglerhut();
	void onEFahrradhelm();
	void onESkibrille();
	void onEBrille();
	void onEWarnweste();
	void onELederjacke();
	void onEGreifer();
	void onESpazierstock();
	void onERadlerhose();
	void onEJogginghose();
	void onETurnschuhe();
	void onESandalen();
	void onESuperman();

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
	ofxButton bBanane;
	ofxButton bKlebepfeil;
	ofxButton bAnglerhut;
	ofxButton bFahrradhelm;
	ofxButton bSkibrille;
	ofxButton bBrille;
	ofxButton bWarnweste;
	ofxButton bLederjacke;
	ofxButton bGreifer;
	ofxButton bSpazierstock;
	ofxButton bRadlerhose;
	ofxButton bJogginghose;
	ofxButton bTurnschuhe;
	ofxButton bSandalen;
	ofxButton bSuperman;
};

