#pragma once

#include "ofMain.h"
#include "ofxSerial.h"
#include "BikeControl.h"
#include "ofxPs3Eye.h"

enum state {
	IDLE,
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

		void onSerialBuffer(const ofxIO::SerialBufferEventArgs& args);
		void onSerialError(const ofxIO::SerialBufferErrorEventArgs& args);
		void onSerialFinish(const ofxIO::SerialBufferEventArgs& args);


		void onFinish();
		void onReset();
		void onStart();
		void onInterim();

		void shotPicture(BikeControl& player);
		
		string stateToString(state s);

private:
	state currentState = IDLE;

	ofxIO::BufferedSerialDevice bike;
	ofxIO::BufferedSerialDevice finish;

	BikeControl bike1;
	BikeControl bike2;

	int maxTurns = 3;
	long lastInterim = 0;

	map<string,ofxPs3Eye> cams;

	ofJson settings;
	ofJson effectConfig;
	int winner = 0;

	ofTrueTypeFont fState;
	ofTrueTypeFont fInfo;

	ofFbo fboCam1;
	ofFbo fboCam2;
	ofFbo fboInterim;

	ofTexture picInterim;

};
