#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	settings = ofLoadJson("settings.json");
	effectConfig = ofLoadJson("effects.json");
	ofLogToConsole();
	initSerial();

	bike1.setup(0,&bike,ofVec2f(settings["cams"]["player1"]["pos"][0], settings["cams"]["player1"]["pos"][1] + 500), effectConfig);
	bike2.setup(1,&bike, ofVec2f(settings["cams"]["player2"]["pos"][0], settings["cams"]["player2"]["pos"][1] + 500), effectConfig);

	// list out the devices
	std::vector<PS3EYECam::PS3EYERef> devices(PS3EYECam::getDevices());

	//init cams
	if (devices.size() > settings["cams"]["player1"]["id"].get<int>()) {
		cams.insert(pair<string, ofxPs3Eye>("player1", ofxPs3Eye()));
		cams["player1"].setup(settings["cams"]["player1"]["id"]);
	}
	if (devices.size() > settings["cams"]["player2"]["id"].get<int>()) {
		cams.insert(pair<string, ofxPs3Eye>("player2", ofxPs3Eye()));
		cams["player2"].setup(settings["cams"]["player2"]["id"]);
	}
	if (devices.size() > settings["cams"]["finish"]["id"].get<int>()) {
		cams.insert(pair<string, ofxPs3Eye>("finish", ofxPs3Eye()));
		cams["finish"].setup(settings["cams"]["finish"]["id"]);
	}

	//start capturing
	ofxPs3Eye::start();
	
	fState.load("fonts/TitilliumWeb-Bold.ttf", 80);
	fInfo.load("fonts/TitilliumWeb-Bold.ttf", 80);

	fboCam1.allocate(settings["bigScreen"]["dimension"][0].get<int>() / 2, settings["bigScreen"]["dimension"][1]);
	fboCam2.allocate(settings["bigScreen"]["dimension"][0].get<int>() / 2, settings["bigScreen"]["dimension"][1]);
	fboInterim.allocate(settings["bigScreen"]["dimension"][0].get<int>(), settings["bigScreen"]["dimension"][1]);
}

//--------------------------------------------------------------
void ofApp::update(){
	for (auto& cam:cams)
	{
		cam.second.update();
	}

	bike1.update();
	bike2.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
	for (auto& cam : cams)
	{
		auto info = settings["cams"][cam.first];
		cam.second.getTexture().draw(info["pos"][0].get<int>(), info["pos"][1].get<int>(), info["dimension"][0].get<int>(), info["dimension"][1].get<int>());
		ofDrawBitmapStringHighlight(cam.first, info["pos"][0].get<int>() + 10, info["pos"][1].get<int>() + 20);
	}

	bike1.draw();
	bike2.draw();

	ofSetColor(0);
	ofDrawRectangle(0, 0, 300, 100);
	ofSetColor(255);
	fState.drawString(stateToString(currentState), 20, 80);

	drawBigScreen();
}

void ofApp::initSerial()
{
	auto deviceDescriptors = ofx::IO::SerialDeviceUtils::listDevices();

	if (!deviceDescriptors.empty())
	{
		ofLogNotice("ofApp::setup") << "Connected Devices: ";
		for (auto deviceDescriptor : deviceDescriptors)
		{
			ofLogNotice("ofApp::setup") << "\t" << deviceDescriptor;

			if (deviceDescriptor.getHardwareId() == settings["serial"]["bike"]) {
				// Connect to the first matching device.
				bool success = bike.setup(deviceDescriptor, 9200);

				if (success)
				{
					bike.registerAllEvents(this);
					ofLogNotice("ofApp::setup") << "Successfully setup " << deviceDescriptor;
				}
				else
				{
					ofLogNotice("ofApp::setup") << "Unable to setup " << deviceDescriptor;
				}
			}else if (deviceDescriptor.getHardwareId() == settings["serial"]["finish"]) {
				// Connect to the first matching device.
				bool success = finish.setup(deviceDescriptor, 9200);

				if (success)
				{
					ofAddListener(finish.events.onSerialBuffer, this, &ofApp::onSerialFinish);
					ofLogNotice("ofApp::setup") << "Successfully setup " << deviceDescriptor;
				}
				else
				{
					ofLogNotice("ofApp::setup") << "Unable to setup " << deviceDescriptor;
				}
			}
		}
		
	}
	else
	{
		ofLogNotice("ofApp::setup") << "No devices connected.";
	}
}

void ofApp::exit()
{
	bike.unregisterAllEvents(this);
}

void ofApp::drawBigScreen()
{
	ofVec2f pos = ofVec2f(settings["bigScreen"]["pos"][0], settings["bigScreen"]["pos"][1]);
	ofVec2f dim = ofVec2f(settings["bigScreen"]["dimension"][0], settings["bigScreen"]["dimension"][1]);

	ofRectangle camSource= ofRectangle(0, 0, 640,480);
	ofRectangle camDist = ofRectangle(0, 0, dim.x / 2, dim.y);
	camSource.scaleTo(camDist, OF_SCALEMODE_FILL);

	ofRectangle finishSource = ofRectangle(0, 0, 640, 480);
	ofRectangle finishDist = ofRectangle(0, 0, dim.x, dim.y);
	finishSource.scaleTo(finishDist, OF_SCALEMODE_FILL);

		ofPushMatrix();
		ofTranslate(pos);
		switch (currentState) {
		case IDLE:
			if (ofGetElapsedTimeMillis() - lastInterim < 4000) {
				if (cams.find("finish") != cams.end()) {
					fboInterim.begin();
					cams["finish"].getTexture().draw(finishSource);
					fboInterim.end();
					fboInterim.draw(0, 0);
				}
			}
			else {
				if (cams.find("player1") != cams.end()) {
					fboCam1.begin();
					cams["player1"].getTexture().draw(camSource);
					fboCam1.end();
					fboCam1.draw(0, 0);
				}
				if (cams.find("player2") != cams.end()) {
					fboCam2.begin();
					cams["player2"].getTexture().draw(camSource);
					fboCam2.end();
					fboCam2.draw(dim.x / 2, 0);
				}
				fInfo.drawString(ofToString(bike1.speed, 1), 30, dim.y - 80);
				fInfo.drawString(ofToString(bike2.speed, 1), dim.x - 30 - fInfo.getStringBoundingBox(ofToString(bike2.speed, 1), 0, 0).width, dim.y - 80);
			}

			
			break;
		case RACE:
			//cams["player1"].getTexture().drawSubsection(ofRectangle(0,0,1920/2,1080),)
			break;
		case FINISH:
			break;
		ofPopMatrix();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key)
	{
	case '1':
		bike1.setSpeed(0);
		break;
	case '2':
		bike1.setSpeed(1);
		break;
	case '3':
		bike1.setSpeed(2);
		break;
	case '4':
		bike1.setSpeed(3);
		break;
	case '5':
		bike1.setSpeed(4);
		break;
	case '6':
		bike1.setSpeed(5);
		break;
	case '7':
		bike1.setSpeed(6);
		break;
	case 'i':
		onInterim();
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

void ofApp::setState(state s)
{
	currentState = s;
	switch (currentState)
	{
	case IDLE:
		break;
	case RACE:
		break;
	case FINISH:
	default:
		break;
	}
}

void ofApp::onSerialBuffer(const ofxIO::SerialBufferEventArgs & args)
{
	string msg = args.buffer().toString();

	switch (msg[0]) {
	case 's': { //speed
		bike1.speedBase = ofToFloat(msg.substr(1, 4));
		bike2.speedBase = ofToFloat(msg.substr(5, 4));
		break;
	}
	case 'e': { //event
		//bike1.speed = ofToFloat(msg.substr(1, 4));
		//bike2.speed = ofToFloat(msg.substr(5, 4));
		break;
	}
	default:
		cout << msg << endl;
		break;
	}
}

void ofApp::onSerialError(const ofxIO::SerialBufferErrorEventArgs & args)
{
}

void ofApp::onSerialFinish(const ofxIO::SerialBufferEventArgs & args)
{
	string msg = args.buffer().toString();
	if (msg[0] == '0') {
		bike1.addInterim(ofGetElapsedTimeMillis());
		onInterim();
		if (bike1.getInterims().size() == maxTurns) {
			onFinish();
			if (winner == -1) winner = 0;
		}
	}
	else {
		bike2.addInterim(ofGetElapsedTimeMillis());
		onInterim();
		if (bike2.getInterims().size() == maxTurns) {
			onFinish();
			if (winner == -1) winner = 2;
		}
	}

}

void ofApp::onFinish()
{
	setState(FINISH);
}

void ofApp::onReset()
{
	bike1.reset();
	bike2.reset();
	setState(IDLE);
	winner = -1;
}

void ofApp::onStart()
{
	if (currentState != IDLE)
	{
		bike1.start();
		bike2.start();
		setState(RACE);
	}
}

void ofApp::onInterim()
{
	lastInterim = ofGetElapsedTimeMillis();
	if (cams.find("finish") != cams.end()) picInterim = cams["finish"].getTexture();
}

void ofApp::shotPicture(BikeControl & player)
{

}

string ofApp::stateToString(state s)
{
	string ret;
	switch (s)  {
	case IDLE: return "IDLE";
	case RACE: return "RACE";
	case FINISH: return "FINISH";
	}
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
