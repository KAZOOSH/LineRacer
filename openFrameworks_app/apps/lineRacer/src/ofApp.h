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

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void initSerial();

		void exit();

		void drawBigScreen();

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

private:
	state currentState = IDLE;
	long lastStateChange = 0;

	ofxIO::BufferedSerialDevice bike;
	ofxIO::BufferedSerialDevice finish;

	BikeControl bike1;
	BikeControl bike2;

	int maxTurns = 3;
	long lastInterim = 0;
	int winner = -1;

	map<string,ofxPs3Eye> camsPlayer;

	ofJson settings;
	ofJson effectConfig;

	//big screen
	ofTrueTypeFont fState;
	ofTrueTypeFont fInfo;
	ofFbo fboCam1;
	ofFbo fboCam2;
	ofFbo fboPrint1;
	ofFbo fboPrint2;
	ofFbo fboInterim;
	ofTexture picInterim;

	//gui
	ofxPanel gui;
	ofxButton bStart;
	ofxButton bFinish;
	ofxButton bReset;
	ofxButton bPhoto1;
	ofxButton bPhoto2;

	map<string, ofSoundPlayer>  sounds;

	//print
	ofTrueTypeFont fPrint;
};
