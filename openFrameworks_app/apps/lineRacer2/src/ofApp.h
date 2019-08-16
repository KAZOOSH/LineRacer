#pragma once

#include "ofMain.h"
#include "ofxSerial.h"
#include "BikeControl.h"
#include "ofxPs3Eye.h"
#include "ofxGui.h"

enum state {
	IDLE,
	WAIT,
	RACE,
	FINISH
};

class PlayerStats {
public:
	PlayerStats(ofJson json);
	PlayerStats(const BikeControl& b,string img);
	string name;
	string img;
	long timestamp;
	vector<int> interims;

	ofJson toJson();
};

class ScreenConfig {
public:
	map<int, shared_ptr<ofxPs3Eye>> player;
	map<int, shared_ptr<ofVideoGrabber>> car;
};

class Screen {
public:
	void draw();
	void updateFbo(shared_ptr<ofxPs3Eye> img);
	void updateFbo(shared_ptr<ofVideoGrabber> img);

	void updateFbo(ofTexture t);
	
	ofFbo fbo;
	ofVec2f pos;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void initSerial();

		void exit();

		void drawBigScreen();
		void drawBigScreen2();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void setState(state s);
		void updateState();

		void onSerialBuffer(const ofxIO::SerialBufferEventArgs& args);
		void onSerialError(const ofxIO::SerialBufferErrorEventArgs& args);
		void onSerialFinish(const ofxIO::SerialBufferEventArgs& args);


		void onFinish();
		void onReset();
		void onStart();
		void onPrintPlayer1();
		void onPrintPlayer2();
		void onInterim(int player);

		void createFinishingFbo(int player);
		void printFinishingImage(int player);

		void shotPicture(BikeControl& player);
		
		string stateToString(state s);
		string getInterimString(int player);

		void backupStats();
		void print(string path);

private:
	state currentState = IDLE;
	long lastStateChange = 0;

	ofxIO::BufferedSerialDevice bike;
	ofxIO::BufferedSerialDevice finish;

	BikeControl bike1;
	BikeControl bike2;

	int maxTurns = 1;
	long lastInterim = 0;
	int winner = -1;

	map<string,ofxPs3Eye> camsPlayer;
	map<string, ofVideoGrabber> camsCar;

	ofJson settings;
	ofJson effectConfig;

	//big screen
	int currentConfig = 0;
	ofTrueTypeFont fState;
	ofTrueTypeFont fInfo;
	ofTrueTypeFont fBig;
	ofTrueTypeFont fSuperBig;
	ofFbo fboCam1;
	ofFbo fboCam2;
	ofFbo fboPrint1;
	ofFbo fboPrint2;
	ofFbo fboInterim;

	vector<Screen> screens;
	vector<ScreenConfig> screenConfigs;
	//ofTexture picInterim;

	//gui
	ofxPanel gui;
	ofxButton bStart;
	ofxButton bFinish;
	ofxButton bReset;
	ofxButton bPhoto1;
	ofxButton bPhoto2;
	ofParameter<bool> isAutoprint;

	map<string, ofSoundPlayer>  sounds;

	//print
	ofTrueTypeFont fPrint;

	vector<PlayerStats> stats;
};
