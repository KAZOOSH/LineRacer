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
		camsPlayer.insert(pair<string, ofxPs3Eye>("player1", ofxPs3Eye()));
		camsPlayer["player1"].setup(settings["cams"]["player1"]["id"],640,480,30);
	}
	if (devices.size() > settings["cams"]["player2"]["id"].get<int>()) {
		camsPlayer.insert(pair<string, ofxPs3Eye>("player2", ofxPs3Eye()));
		camsPlayer["player2"].setup(settings["cams"]["player2"]["id"], 640, 480, 30);
	}
	if (devices.size() > settings["cams"]["finish"]["id"].get<int>()) {
		camsPlayer.insert(pair<string, ofxPs3Eye>("finish", ofxPs3Eye()));
		camsPlayer["finish"].setup(settings["cams"]["finish"]["id"], 640, 480, 30);
	}

	//gui
	gui.setup();
	bStart.addListener(this, &ofApp::onStart);
	gui.add(bStart.setup("Start"));
	bFinish.addListener(this, &ofApp::onFinish);
	gui.add(bFinish.setup("Finish"));
	bReset.addListener(this, &ofApp::onReset);
	gui.add(bReset.setup("Reset"));
	bPhoto1.addListener(this, &ofApp::onPrintPlayer1);
	gui.add(bPhoto1.setup("print Photo Player 1"));
	bPhoto2.addListener(this, &ofApp::onPrintPlayer2);
	gui.add(bPhoto2.setup("print Photo Player 2"));
	gui.setPosition(settings["cams"]["finish"]["pos"][0], 600);

	//start capturing
	ofxPs3Eye::start();
	
	fState.load("fonts/TitilliumWeb-Bold.ttf", 80);
	fInfo.load("fonts/TitilliumWeb-Bold.ttf", 80);
	fPrint.load("fonts/TitilliumWeb-Bold.ttf", 40);

	fboCam1.allocate(settings["bigScreen"]["dimension"][0].get<int>() / 2, settings["bigScreen"]["dimension"][1]);
	fboCam2.allocate(settings["bigScreen"]["dimension"][0].get<int>() / 2, settings["bigScreen"]["dimension"][1]);
	fboInterim.allocate(settings["bigScreen"]["dimension"][0].get<int>(), settings["bigScreen"]["dimension"][1]);

	float calc = 300.0f / 24.3f;
	float w = 60 * calc;
	float h = 75 * calc;
	fboPrint1.allocate(w, h);
	fboPrint2.allocate(w, h);

	//ofSetWindowPosition(0, 0);
	ofSetWindowShape(1920 * 2, 1080);
}

//--------------------------------------------------------------
void ofApp::update(){
	for (auto& cam:camsPlayer)
	{
		cam.second.update();
	}

	bike1.update();
	bike2.update();

	updateState();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
	for (auto& cam : camsPlayer)
	{
		auto info = settings["cams"][cam.first];
		cam.second.getTexture().draw(info["pos"][0].get<int>(), info["pos"][1].get<int>(), info["dimension"][0].get<int>(), info["dimension"][1].get<int>());
		ofDrawBitmapStringHighlight(cam.first, info["pos"][0].get<int>() + 10, info["pos"][1].get<int>() + 20);
	}
	auto info = settings["cams"]["finish"];
	//camFinish.draw(info["pos"][0], info["pos"][1], info["dimension"][0], info["dimension"][1]);


	bike1.draw();
	bike2.draw();

	ofSetColor(0);
	ofDrawRectangle(0, 0, 300, 100);
	ofSetColor(255);
	fState.drawString(stateToString(currentState), 20, 80);

	gui.draw();

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
				bool success = bike.setup(deviceDescriptor, 9600);

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
				bool success = finish.setup(deviceDescriptor, 9600);

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
		{
			if (camsPlayer.find("player1") != camsPlayer.end()) {
				fboCam1.begin();
				camsPlayer["player1"].getTexture().draw(camSource);
				fboCam1.end();
				fboCam1.draw(0, 0);
			}
			if (camsPlayer.find("player2") != camsPlayer.end()) {
				fboCam2.begin();
				camsPlayer["player2"].getTexture().draw(camSource);
				fboCam2.end();
				fboCam2.draw(dim.x / 2, 0);
			}
			break;
		}
		case WAIT: {
			if (camsPlayer.find("finish") != camsPlayer.end()) {
				camsPlayer["finish"].getTexture().draw(finishSource);
			}
			string countdown = ofToString((4100 -(ofGetElapsedTimeMillis() - lastStateChange)) / 1000);
			fState.drawString(countdown, 0.5*(dim.x - fInfo.getStringBoundingBox(countdown, 0, 0).width), dim.y*0.5);
			break;
		}
		case RACE:
		{
			if (ofGetElapsedTimeMillis() - lastInterim < 4000) {
				if (camsPlayer.find("finish") != camsPlayer.end()) {
					fboInterim.begin();
					camsPlayer["finish"].getTexture().draw(finishSource);
					fboInterim.end();
					fboInterim.draw(0, 0);
				}
			}
			else {
				if (camsPlayer.find("player1") != camsPlayer.end()) {
					fboCam1.begin();
					camsPlayer["player1"].getTexture().draw(camSource);
					fboCam1.end();
					fboCam1.draw(0, 0);
				}
				if (camsPlayer.find("player2") != camsPlayer.end()) {
					fboCam2.begin();
					camsPlayer["player2"].getTexture().draw(camSource);
					fboCam2.end();
					fboCam2.draw(dim.x / 2, 0);
				}
				string textP1 = ofToString(ofMap(bike1.speed, 0, 10, 0, settings["bigScreen"]["vMax"]), 0) + "km/h";
				string textP2 = ofToString(ofMap(bike2.speed, 0, 10, 0, settings["bigScreen"]["vMax"]), 0) + "km/h";
				fInfo.drawString(textP1, 30, 100);
				fInfo.drawString(textP2, dim.x - 30 - fInfo.getStringBoundingBox(textP2, 0, 0).width, 100);

				ofPushMatrix();
				ofTranslate(0, 300);
				ofPushMatrix();
				ofTranslate(50, 0);
				for (auto& e : bike1.getEffects()) {
					e.icon.draw(0, 0, 64, 64);
					fInfo.drawString(e.getRuntimeString(), 74, 64);
					ofTranslate(0, 74);
				}
				ofPopMatrix();
				ofTranslate(970, 0);
				for (auto& e : bike2.getEffects()) {
					e.icon.draw(0, 0, 64, 64);
					fInfo.drawString(e.getRuntimeString(), 74, 64);
					ofTranslate(0, 74);
				}
				ofPopMatrix();
				ofPopMatrix();
			}
			string textI1 = getInterimString(0);
			fInfo.drawString(textI1, 30, dim.y - 80 - fInfo.getStringBoundingBox(textI1, 0, 0).height);

			string textI2 = getInterimString(1);
			fInfo.drawString(textI2, dim.x - 30 - fInfo.getStringBoundingBox(textI2, 0, 0).width, dim.y - 80 - fInfo.getStringBoundingBox(textI1, 0, 0).height);

		}
			break;
		case FINISH:
			fboInterim.draw(0, 0);
			string tWin = "Player ";
			tWin += (winner == 0 ? "1" : "2");
			tWin +=" wins";
			fState.drawString(tWin, 0.5*(dim.x - fInfo.getStringBoundingBox(tWin, 0, 0).width), dim.y*0.5);
			break;
	}
		ofPopMatrix();
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
		onInterim(0);
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
	case WAIT:
		break;
	case RACE:
		bike1.start();
		bike2.start();
		break;
	case FINISH:
		if (camsPlayer.find("player1") != camsPlayer.end()) {
			createFinishingFbo(0);
		}
		if (camsPlayer.find("player2") != camsPlayer.end()) {
			createFinishingFbo(1);
		}
	default:
		break;
	}
	lastStateChange = ofGetElapsedTimeMillis();
}

void ofApp::updateState()
{
	switch (currentState)
	{
	case IDLE:
		break;
	case WAIT:
		if (ofGetElapsedTimeMillis() - lastStateChange > 3100) {
			setState(RACE);
		}
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
			onInterim(0);
		}
		else {
			onInterim(1);
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
	if (currentState == IDLE)
	{
		
		setState(WAIT);
	}
}

void ofApp::onPrintPlayer1()
{
	printFinishingImage(0);
}

void ofApp::onPrintPlayer2()
{
	printFinishingImage(1);
}

void ofApp::onInterim(int player)
{
	if (currentState == RACE) {
		if (player == 0) {
			bike1.addInterim(ofGetElapsedTimeMillis());
			if (bike1.getInterims().size() == maxTurns) {
				onFinish();
				if (winner == -1) winner = 0;
			}
		}
		else {
			bike2.addInterim(ofGetElapsedTimeMillis());
			if (bike2.getInterims().size() == maxTurns) {
				onFinish();
				if (winner == -1) winner = 2;
			}
		}


		lastInterim = ofGetElapsedTimeMillis();
		if (camsPlayer.find("finish") != camsPlayer.end()) {
			picInterim = camsPlayer["finish"].getTexture();
		}
	}
}

void ofApp::createFinishingFbo(int player)
{
	float calc = 300.0f / 24.3f;
	float w = 60 * calc;
	float h = 70 * calc;

	ofRectangle finishDist = ofRectangle(50, 50, w - 100, (w - 100) * 480 / 640);

	ofFbo* fbo = player == 0 ? &fboPrint1 : &fboPrint2;
	fbo->begin();
	ofClear(255,255);
	auto pic = player == 0 ? camsPlayer["player1"].getTexture() : camsPlayer["player2"].getTexture();
	pic.draw(finishDist);

	ofSetColor(0);
	ofPushMatrix();
	ofTranslate(finishDist.x, finishDist.y + finishDist.height + 80);
	string l0 = player == 0 ? bike1.name : bike2.name;
	l0 += " won the race!";
	fPrint.drawString(l0, 0, 0);
	ofTranslate(0, 120);
	fPrint.drawString(getInterimString(player), 0, 0);
	ofPopMatrix();
	fbo->end();
}

void ofApp::printFinishingImage(int player)
{
	ofImage img;
	ofPixels px;
	player == 0 ? fboPrint1.readToPixels(px) : fboPrint2.readToPixels(px);
	img.setFromPixels(px);
	img.save("print.png");
}

void ofApp::shotPicture(BikeControl & player)
{

}

string ofApp::stateToString(state s)
{
	string ret;
	switch (s)  {
	case IDLE: return "IDLE";
	case WAIT: return "WAIT";
	case RACE: return "RACE";
	case FINISH: return "FINISH";
	}
}

string ofApp::getInterimString(int player)
{
	auto interims = player == 0 ? bike1.getInterims() : bike2.getInterims();
	string r;
	for (size_t i = 0; i < interims.size(); i++) {
		r += "Round " + ofToString(i + 1) + "\t";
		int t = interims[i] - lastStateChange;
		int minutes = t / 1000 / 60;
		int seconds = t % 60000 / 1000;
		int millis = t % 1000 / 10;

		r += ofToString(minutes) + ":" + (seconds < 10 ? "0":"") + ofToString(seconds) + "." + (millis < 10 ? "0" : "") + ofToString(millis) + "\n";
	}
	return r;
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
