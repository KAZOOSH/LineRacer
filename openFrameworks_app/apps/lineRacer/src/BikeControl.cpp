#include "BikeControl.h"



BikeControl::BikeControl()
{
	params.add(name.set("name"));
	params.add(speedBase.set("base speed", 0.0,0.0,9.99));
	params.add(multiplicatorBase.set("base speed multiplicator", 1.0, 0.0, 2.0));
	params.add(isFixedBase.set("base fixed speed", false));
	params.add(speed.set("speed", 0.0, 0.0, 9.99));
	params.add(multiplicator.set("speed multiplicator", 1.0, 0.0, 2.0));
	params.add(isFixed.set("fixed speed", false));
	gui.setup(params);

	//isFixed.addListener(this, &BikeControl::onFixedSpeed);
	bPowerBar.addListener(this, &BikeControl::onEPowerBar);
	gui.add(bPowerBar.setup("Power Bar"));
}


BikeControl::~BikeControl()
{
}

void BikeControl::setup(int id_, ofxIO::BufferedSerialDevice* serial_, ofVec2f posGui, ofJson effectConfig_)
{
	id = id_;
	serial = serial_;
	gui.setPosition(posGui.x, posGui.y);
	effectConfig = effectConfig_;
	name = "Player" + ofToString(id + 1);
}

void BikeControl::update()
{
	auto speedOld = speed.get();
	auto multiplicatorOld = multiplicator.get();
	bool isFixedOld = isFixed.get();
	

	speed = speedBase.get();
	multiplicator = multiplicatorBase.get();
	isFixed = isFixedBase.get();

	deque<int> toDel;

	// update effects
	long t = ofGetElapsedTimeMillis();
	for (int i = 0; i < effects.size();++i) {
		effects[i].applyEffect(multiplicator, isFixed, speed);
		if (effects[i].startTime + effects[i].runTime < t) {
			toDel.push_front(i);
		}
	}
	// del not used effects
	for (int i = 0; i < toDel.size(); ++i) {
		effects.erase(effects.begin() + toDel[i]);
	}

	//write to Serial
	if (speedOld != speed && isFixed.get()) setSpeed(speed);
	if (multiplicatorOld != multiplicator) setMultiplicator(multiplicator);
	if (isFixedOld != isFixed.get()) setFixedSpeed(isFixed);
}

void BikeControl::draw()
{
	gui.draw();
	ofPushMatrix();
	ofTranslate(gui.getPosition().x + gui.getWidth() + 20, gui.getPosition().y + 15);
	for (auto& e : effects) {
		ofSetColor(255);
		ofDrawRectangle(0, -12, 16, 16);
		e.icon.draw(0, -12, 16, 16);
		ofDrawBitmapString(e.name + "  " + e.getRuntimeString(), 20, 0);
		ofTranslate(0, 18);
	}
	ofPopMatrix();
}

void BikeControl::setMultiplicator(float mult)
{
	int val = ofMap(mult, 0, 2, 0, 254, true);
	
	if (mult == 1.0) val = 255;

	std::string text = "m";
	text += id;
	text += char(val);
	ofx::IO::ByteBuffer textBuffer(text);
	serial->writeBytes(textBuffer);
	serial->writeByte('\n');
}

void BikeControl::setSpeed(float speed)
{
	int val = ofMap(speed, 0, 9.99, 0, 255, true);

	std::string text = "s";
	text += id;
	text += char(val);
	ofx::IO::ByteBuffer textBuffer(text);
	serial->writeBytes(textBuffer);
	serial->writeByte('\n');
}

void BikeControl::setFixedSpeed(bool isFixed)
{
	std::string text = "e";
	text += id;
	text += isFixed;
	ofx::IO::ByteBuffer textBuffer(text);
	serial->writeBytes(textBuffer);
	serial->writeByte('\n');
}

void BikeControl::addInterim(int time)
{
	interims.push_back(time);
}

vector<int> BikeControl::getInterims()
{
	vector<int> times;
	for (int i = 0; i < interims.size(); ++i) {
		int tLast = tStart;
		if (i > 0) tLast = interims[i - 1];
		times.push_back(interims[i] - tLast);
	}
	
	return interims;
}

void BikeControl::clearInterims()
{
	interims.clear();
}


int BikeControl::getTotalTime()
{
	return interims.back() - tStart;
}

int BikeControl::getId()
{
	return id;
}


void BikeControl::reset()
{
	clearInterims();
	setFixedSpeed(true);
	setSpeed(0);
	setMultiplicator(1);
	effects.clear();
}

void BikeControl::start()
{
	setFixedSpeed(false);
}

void BikeControl::addEffect(string name)
{
	effects.push_back(Effectory::getEffect(name, effectConfig));
}

vector<Effect> BikeControl::getEffects()
{
	return effects;
}

void BikeControl::onFixedSpeed(bool & isFixed)
{
	setFixedSpeed(isFixed);
}

void BikeControl::onEPowerBar()
{
	addEffect("Power Bar");
}
