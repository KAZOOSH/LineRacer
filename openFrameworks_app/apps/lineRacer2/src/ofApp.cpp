#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	settings = ofLoadJson("settings.json");
	effectConfig = ofLoadJson("effects.json");
	ofLogToConsole();
	initSerial();

	ofJson sj = ofLoadJson("stats.json");
	for (auto& s : sj) {
		stats.push_back(PlayerStats(s));
	}

	bike1.setup(0,&bike,ofVec2f(settings["cams"]["player1"]["pos"][0], settings["cams"]["player1"]["pos"][1] + 500), effectConfig);
	bike2.setup(1,&bike, ofVec2f(settings["cams"]["player2"]["pos"][0], settings["cams"]["player2"]["pos"][1] + 500), effectConfig);

	// list out the devices
	std::vector<PS3EYECam::PS3EYERef> devices(PS3EYECam::getDevices());
	ofVideoGrabber grabber;
	auto nPlayerCams = grabber.listDevices();


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
	if (nPlayerCams.size() > settings["cams"]["car1"]["id"].get<int>()) {
		camsCar.insert(pair<string, ofVideoGrabber>("car1", ofVideoGrabber()));
		camsCar["car1"].setDesiredFrameRate(30);
		camsCar["car1"].setDeviceID(settings["cams"]["car1"]["id"]);
		camsCar["car1"].setup(320, 240);
	}

	if (nPlayerCams.size() > settings["cams"]["car2"]["id"].get<int>()) {
		camsCar.insert(pair<string, ofVideoGrabber>("car2", ofVideoGrabber()));
		camsCar["car2"].setDesiredFrameRate(30);
		camsCar["car2"].setDeviceID(settings["cams"]["car2"]["id"]);
		camsCar["car2"].setup(320, 240);
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
	gui.add(isAutoprint.set("autoprint", true));
	gui.setPosition(settings["cams"]["finish"]["pos"][0], 600);

	//start capturing
	ofxPs3Eye::start();
	
	fState.load("fonts/TitilliumWeb-Bold.ttf", 80);
	fBig.load("fonts/TitilliumWeb-Bold.ttf", 160);
	fSuperBig.load("fonts/TitilliumWeb-Bold.ttf", 400);
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

	ofSetWindowPosition(0, 0);
	ofSetWindowShape(1920 * 2, 1080);

	sounds.insert(pair<string, ofSoundPlayer>("start", ofSoundPlayer()));
	sounds["start"].load(settings["sound"]["start"].get<string>());
	sounds.insert(pair<string, ofSoundPlayer>("finish", ofSoundPlayer()));
	sounds["finish"].load(settings["sound"]["finish"].get<string>());

	////init fbos
	//int maxCams = 0;
	//for (auto& config:settings["bigScreen"]["configs"]){
	//	if (config.size() > 0) {
	//		vector<ofFbo> fboConfig;
	//		
	//		for (auto& settings : config) {
	//			
	//		}
	//	}
	//}
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSoundUpdate();
	for (auto& cam:camsPlayer)
	{
		cam.second.update();
	}

	for (auto& cam : camsCar)
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

	for (auto& cam : camsCar)
	{
		auto info = settings["cams"][cam.first];
		cam.second.draw(info["pos"][0].get<int>(), info["pos"][1].get<int>(), info["dimension"][0].get<int>(), info["dimension"][1].get<int>());
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
				bool success = finish.setup(deviceDescriptor, 115200);

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
			fSuperBig.drawString(countdown, 0.5*(dim.x - fSuperBig.getStringBoundingBox(countdown, 0, 0).width), dim.y*0.7);
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
			string tWin = bike1.name;
			if(winner == 1)tWin = bike2.name;
			tWin +=" gewinnt";
			fBig.drawString(tWin, 0.5*(dim.x - fBig.getStringBoundingBox(tWin, 0, 0).width), dim.y*0.5);
			break;
	}
		ofPopMatrix();
}

void ofApp::drawBigScreen2()
{
	ofVec2f pos = ofVec2f(settings["bigScreen"]["pos"][0], settings["bigScreen"]["pos"][1]);
	ofVec2f dim = ofVec2f(settings["bigScreen"]["dimension"][0], settings["bigScreen"]["dimension"][1]);

	ofRectangle camSource = ofRectangle(0, 0, 640, 480);
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
		string countdown = ofToString((4100 - (ofGetElapsedTimeMillis() - lastStateChange)) / 1000);
		fSuperBig.drawString(countdown, 0.5*(dim.x - fSuperBig.getStringBoundingBox(countdown, 0, 0).width), dim.y*0.7);
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
		string tWin = bike1.name;
		if (winner == 1)tWin = bike2.name;
		tWin += " gewinnt";
		fBig.drawString(tWin, 0.5*(dim.x - fBig.getStringBoundingBox(tWin, 0, 0).width), dim.y*0.5);
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
	case 'o':
		onInterim(1);
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
		sounds["start"].play();
		break;
	case RACE: {
		bike1.start();
		bike2.start();
		std::string text = "b";
		text += "1";
		text += "1";
		ofx::IO::ByteBuffer textBuffer(text);
		bike.writeBytes(textBuffer);
		bike.writeByte('\n');
	}
		break;
	case FINISH: {
		sounds["finish"].play();
		if (camsPlayer.find("player1") != camsPlayer.end()) {
			createFinishingFbo(0);
		}
		if (camsPlayer.find("player2") != camsPlayer.end()) {
			createFinishingFbo(1);
		}
		ofPixels p;
		ofImage img;
		string path = "stats/" + ofGetTimestampString() + ".png";
		if (winner == 0) {
			stats.push_back(PlayerStats(bike1, path));
			fboCam1.readToPixels(p);
		}
		else {
			stats.push_back(PlayerStats(bike2, path));
			fboCam1.readToPixels(p);
		}
		img.setFromPixels(p);
		img.save(path);
		backupStats();

		std::string text = "b";
		text += "0";
		text += "0";
		ofx::IO::ByteBuffer textBuffer(text);
		bike.writeBytes(textBuffer);
		bike.writeByte('\n');
		bike1.isFixedBase.set(true);
		bike1.speedBase.set(0);
		bike2.isFixedBase.set(true);
		bike2.speedBase.set(0);

		printFinishingImage(0);
		printFinishingImage(1);
		break;
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
		ofDrawBitmapString("Player " + ofToString(winner) + "wins", 1500, 800);

	default:
		break;
	}
}

void ofApp::onSerialBuffer(const ofxIO::SerialBufferEventArgs & args)
{
	string msg = args.buffer().toString();
	cout << msg.substr(1, 4) << "   " << msg.substr(6, 4) << endl;

	switch (msg[0]) {
	case 's': { //speed
		if(!bike1.isFixedBase) bike1.speedBase = ofToFloat(msg.substr(1, 4));
		if(!bike2.isFixedBase) bike2.speedBase = ofToFloat(msg.substr(6, 4));
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
		if (msg[0] == '1') {
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
		lastInterim = ofGetElapsedTimeMillis();
		if (camsPlayer.find("finish") != camsPlayer.end()) {
			ofVec2f dim = ofVec2f(settings["bigScreen"]["dimension"][0], settings["bigScreen"]["dimension"][1]);

			ofRectangle finishSource = ofRectangle(0, 0, 640, 480);
			ofRectangle finishDist = ofRectangle(0, 0, dim.x, dim.y);
			finishSource.scaleTo(finishDist, OF_SCALEMODE_FILL);

			fboInterim.begin();
			camsPlayer["finish"].getTexture().draw(finishSource);
			fboInterim.end();
			fboInterim.draw(0, 0);
			//picInterim = camsPlayer["finish"].getTexture();
		}
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
				if (winner == -1) winner = 1;
			}
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
	//string l0 = player == 0 ? bike1.name : bike2.name;
	//l0 += " won the race!";
	//fPrint.drawString(l0, 0, 0);
	//ofTranslate(0, 120);
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
	print("print.png");
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
		r += "Total Time\t";
		int t = interims[i] - lastStateChange;
		int minutes = t / 1000 / 60;
		int seconds = t % 60000 / 1000;
		int millis = t % 1000 / 10;

		r += ofToString(minutes) + ":" + (seconds < 10 ? "0":"") + ofToString(seconds) + "." + (millis < 10 ? "0" : "") + ofToString(millis) + "\n";
	}
	return r;
}

void ofApp::backupStats()
{
	ofJson js;
	for (auto& s : stats){
		js.push_back(s.toJson());
	}
	ofSaveJson("stats.json", js);
}

void ofApp::print(string path_)
{
	string syscall = "C:\\IrfanView\\i_view32.exe ";
	string path = ofFilePath::getAbsolutePath(path_);
	path[2] = '\\';
	syscall += path;
	syscall += " /ini=C:\\IrfanView\\ /print=\"";
	syscall += "EPSON TM-T20 Receipt";
	syscall += "\"";
	ofSystem(syscall.c_str());
	ofLogNotice("print", syscall);
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

PlayerStats::PlayerStats(ofJson json)
{
	name = json["name"].get<string>();
	img = json["img"].get<string>();
	timestamp = json["timestamp"].get<long>();
	for (auto& i : json["interims"]) {
		interims.push_back(i.get<int>());
	}
}

PlayerStats::PlayerStats(const BikeControl & b,string img_)
{
	name = b.name;
	img = img_;
	timestamp = ofGetUnixTime();

}

ofJson PlayerStats::toJson()
{
	ofJson ret;
	ret["name"] = name;
	ret["img"] = img;
	ret["timestamp"] = timestamp;
	ofJson times;
	for (auto& v : interims) {
		times.push_back(v);
	}
	ret["interims"] = times;

	return ret;
}
