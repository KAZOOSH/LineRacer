#include "OmiCup.h"
#include "GPF.h"

namespace ofxModule {
    
	OmiCup::OmiCup(string moduleName):ModuleDrawable("OmiCup",moduleName){
		ofSetWindowShape(settings["gui"]["size"][0].get<int>(),settings["gui"]["size"][1].get<int>());
			effectConfig = ofLoadJson("effects.json");
			ofLogToConsole();
			initSerial();

			/*ofJson sj = ofLoadJson("stats.json");
			for (auto& s : sj) {
				stats.push_back(PlayerStats(s));
			}*/

			bike1.setup(0,&bike,ofVec2f(settings["gui"]["player1"]["pos"][0].get<int>(), settings["gui"]["player1"]["pos"][1].get<int>()), effectConfig);
			bike2.setup(1,&bike, ofVec2f(settings["gui"]["player2"]["pos"][0].get<int>(), settings["gui"]["player2"]["pos"][1].get<int>()), effectConfig);




			//gui
				gui.setup();
				bStart.addListener(this, &OmiCup::onStart);
				gui.add(bStart.setup("Start"));
				bFinish.addListener(this, &OmiCup::onFinish);
				gui.add(bFinish.setup("Finish"));
				bReset.addListener(this, &OmiCup::onReset);
				gui.add(bReset.setup("Reset"));

				gui.add(isAutoprint.set("autoprint", true));
				gui.setPosition(settings["gui"]["finish"]["pos"][0].get<int>(), settings["gui"]["finish"]["pos"][1].get<int>());

				fState.load("fonts/TitilliumWeb-Bold.ttf", 80);
				fPrint.load("fonts/TitilliumWeb-Bold.ttf", 60);
				fPrintTitle.load("fonts/TitilliumWeb-Bold.ttf", 90);

				sounds.insert(pair<string, ofSoundPlayer>("start", ofSoundPlayer()));
					sounds["start"].load(settings["sound"]["start"].get<string>());
					sounds.insert(pair<string, ofSoundPlayer>("finish", ofSoundPlayer()));
					sounds["finish"].load(settings["sound"]["finish"].get<string>());
				}

  
    
    
    //------------------------------------------------------------------
	void OmiCup::update() {
		ofSetWindowTitle(ofToString(ofGetFrameRate()));
		if(!isCamsInit){
			for(auto& c:settings["gui"]["cams"]){
				notifyEvent("activateCam",ofJson{{"id",c.get<string>()}});
				cams.insert(make_pair(c.get<string>(),make_shared<ofTexture>(ofTexture())));
			}
			isCamsInit = true;
		}

		bike1.update();
			bike2.update();

			updateState();


    }

	void OmiCup::draw()
	{
		ofSetColor(0);
		ofDrawRectangle(0, 0, ofGetWidth(), 100);
		ofSetColor(255);
		fState.drawString(stateToString(currentState), 20, 80);
		if(currentState == RACE){
			int t = ofGetElapsedTimeMillis() - lastStateChange;
			fState.drawString(ofToString(t/1000) + ":" + ofToString((t%1000)/100), 300, 80);
		}

		int x = 0;
		int y = 150;
		for(auto& cam:cams){
			if(cam.second->isAllocated()){
				cam.second->draw(x,y,320,240);
				ofDrawBitmapStringHighlight(cam.first, x+10,y+20);
				x += 340;
				if(x +340 > ofGetWidth()){
					x = 0;
					y += 260;
				}
			}

		}

		bike1.draw();
		bike2.draw();
		gui.draw();
	}

	void OmiCup::initSerial()
	{
		auto deviceDescriptors = ofx::IO::SerialDeviceUtils::listDevices();

			if (!deviceDescriptors.empty())
			{
				ofLogNotice("ofApp::setup") << "Connected Devices: ";
				for (auto deviceDescriptor : deviceDescriptors)
				{
					ofLogNotice("ofApp::setup") << "\t" << deviceDescriptor;

					if (deviceDescriptor.getHardwareId() == settings["serial"]["bike"]) {
						bike.setup(deviceDescriptor);
						ofAddListener(bike.speedEvent,this,&OmiCup::onSerialBuffer);

					}else if (deviceDescriptor.getHardwareId() == settings["serial"]["finish"]) {
						// Connect to the first matching device.
						bool success = finish.setup(deviceDescriptor, 115200);

						if (success)
						{
							ofAddListener(finish.events.onSerialBuffer, this, &OmiCup::onSerialFinish);
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

	void OmiCup::exit()
	{
//		bike.unregisterAllEvents(this);
	}

	void OmiCup::onSerialBuffer(string& msg)
	{
			//cout << msg.substr(1, 4) << "   " << msg.substr(6, 4) << endl;

			switch (msg[0]) {
			case 's': { //speed
				if(!bike1.isFixedBase) bike1.speedBase = ofToFloat(msg.substr(1, 4));
				if(!bike2.isFixedBase) bike2.speedBase = ofToFloat(msg.substr(6, 4));
				break;
			}
			default:
				//cout << msg << endl;
				break;
			}
	}

	void OmiCup::onSerialError(const ofx::IO::SerialBufferErrorEventArgs &args)
	{

	}

	void OmiCup::onSerialFinish(const ofx::IO::SerialBufferEventArgs &args)
	{
		string msg = args.buffer().toString();
				if (msg[0] == '1') {
					onInterim(0);
				}
				else {
					onInterim(1);
				}
	}

	void OmiCup::onFinish()
	{
		setState(FINISH);
	}

	void OmiCup::onReset()
	{
		bike1.reset();
			bike2.reset();
			setState(IDLE);
			winner = -1;
	}

	void OmiCup::onStart()
	{
		if (currentState == IDLE)
			{

				setState(WAIT);
		}
	}

	void OmiCup::onInterim(int player)
	{
		if (currentState == RACE) {
				lastInterim = ofGetElapsedTimeMillis();
				/*if (camsPlayer.find("finish") != camsPlayer.end()) {
					ofVec2f dim = ofVec2f(settings["bigScreen"]["dimension"][0], settings["bigScreen"]["dimension"][1]);

					ofRectangle finishSource = ofRectangle(0, 0, 640, 480);
					ofRectangle finishDist = ofRectangle(0, 0, dim.x, dim.y);
					finishSource.scaleTo(finishDist, OF_SCALEMODE_FILL);

					fboInterim.begin();
					camsPlayer["finish"].getTexture().draw(finishSource);
					fboInterim.end();
					fboInterim.draw(0, 0);
					//picInterim = camsPlayer["finish"].getTexture();
				}*/
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


    
    //------------------------------------------------------------------
	void OmiCup::proceedModuleEvent(ModuleEvent& e) {
		
		// receive the communicator message of address "response"
		if (e.address== "camUpdate") {

			string camId = e.message["id"].get<string>();

			if(cams.find(camId) != cams.end()){
				cams[camId] = e.texture;
			}

		}
    }


	string ofxModule::OmiCup::stateToString(state s)
	{
		string ret;
			switch (s)  {
			case IDLE: return "IDLE";
			case WAIT: return "WAIT";
			case RACE: return "RACE";
			case FINISH: return "FINISH";
			}
	}


	void OmiCup::setState(state s)
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

				saveImages(winner);

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

				if (isAutoprint){
					print("winner.png");
					print("looser.png");
				}
				break;
			}
			default:
				break;
			}
			lastStateChange = ofGetElapsedTimeMillis();
	}

	string ofxModule::OmiCup::getInterimString(int player)
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

	void ofxModule::OmiCup::backupStats()
	{
		ofJson js;
			for (auto& s : stats){
				js.push_back(s.toJson());
			}
			ofSaveJson("stats.json", js);



	}

	void ofxModule::OmiCup::saveImages(int winner)
	{
		float calc = 300.0f / 24.3f;
		float w = 60 * calc;
		float h = 70 * calc;

		ofFbo winnerFbo;
		winnerFbo.allocate(w,h);
		winnerFbo.begin();

		string winCam;
		if(winner == 0){
			winCam = "opi player";
		}
		else{
			winCam = "omi player";
		}
		ofSetColor(255);
		int hCam = 0;
			if(cams.find(winCam)!=cams.end()){
				hCam = w * cams[winCam]->getHeight() / cams[winCam]->getWidth();
				cams[winCam]->draw(0,0,w,hCam);
			}

			ofSetColor(0);
			fPrintTitle.drawString(settings["print"]["text"]["winner"]["title"],0,hCam +100);
			fPrint.drawString(settings["print"]["text"]["winner"]["second"],0,hCam +200);
			fPrint.drawString(getInterimString(0),0,hCam +280);

			winnerFbo.end();


			ofFbo looserFbo;
			looserFbo.allocate(w,h);
			looserFbo.begin();

			string looserCam;
			if(winner == 1){
				looserCam = "opi player";
			}
			else{
				looserCam = "omi player";
			}
			ofSetColor(255);
			hCam = 0;
				if(cams.find(looserCam)!=cams.end()){
					hCam = w * cams[looserCam]->getHeight() / cams[looserCam]->getWidth();
					cams[looserCam]->draw(0,0,w,hCam);
				}

				ofSetColor(0);
				fPrintTitle.drawString(settings["print"]["text"]["looser"]["title"],0,hCam +100);
				fPrint.drawString(settings["print"]["text"]["looser"]["second"],0,hCam +200);
				fPrint.drawString(settings["print"]["text"]["looser"]["third"],0,hCam +280);
				looserFbo.end();



		ofPixels p;
		ofImage img;
		winnerFbo.readToPixels(p);
		img.setFromPixels(p);
		img.save("winner.png");

		looserFbo.readToPixels(p);
		img.setFromPixels(p);
		img.save("looser.png");
	}

	void OmiCup::print(string path)
	{
		string cmd = "lp -d ";
		cmd += settings["print"]["printer"].get<string>();
		cmd += " ";
		cmd += ofFilePath::getAbsolutePath(path);
		ofSystem(cmd.c_str());
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

	ofxModule::PlayerStats::PlayerStats(const BikeControl &b, string img_)
	{
		name = b.name;
			img = img_;
			timestamp = ofGetUnixTime();
	}

	ofJson ofxModule::PlayerStats::toJson()
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
	}

	void ofxModule::OmiCup::updateState()
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
