#pragma once

#include "ofMain.h"
#include "ModuleDrawable.h"
#include "ofxSerial.h"
#include "BikeControl.h"
#include "BikeSerial.h"
#include "ofxGui.h"

namespace ofxModule {

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

class OmiCup : public ModuleDrawable{
	
  public:
	
	  OmiCup(string moduleName = "OmiCup");
	
    void update();
	void draw();

	void initSerial();
	void exit();

	void onSerialBuffer(string& msg);
	void onSerialError(const ofxIO::SerialBufferErrorEventArgs& args);
	void onSerialFinish(const ofxIO::SerialBufferEventArgs& args);

	void onFinish();
	void onReset();
	void onStart();
	void onInterim(int player);

	void setState(state s);
	void updateState();

	string stateToString(state s);
	string getInterimString(int player);

	void backupStats();
	void saveImages(int winner);
	void print(string path);

	void mouseMoved(ofMouseEventArgs & mouse) {};
	void mouseDragged(ofMouseEventArgs & mouse) {};
	void mousePressed(ofMouseEventArgs & mouse);
	void mouseReleased(ofMouseEventArgs & mouse) {};
	void mouseScrolled(ofMouseEventArgs & mouse) {};
	void mouseEntered(ofMouseEventArgs & mouse) {};
	void mouseExited(ofMouseEventArgs & mouse) {};

    
	
protected:
    void proceedModuleEvent(ModuleEvent& e);
    
    private:
	state currentState = IDLE;
		long lastStateChange = 0;

		BikeSerial bike;
		ofxIO::BufferedSerialDevice finish;

		BikeControl bike1;
		BikeControl bike2;

		int maxTurns = 1;
		long lastInterim = 0;
		int winner = -1;

		ofJson effectConfig;

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
			ofTrueTypeFont fPrintTitle;
			ofTrueTypeFont fState;

			vector<PlayerStats> stats;


	//stream
			bool isCamsInit=false;
			map<string,shared_ptr<ofTexture>> cams;

};
}

