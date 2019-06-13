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
	bBanane.addListener(this, &BikeControl::onEBanane);
	bKlebepfeil.addListener(this, &BikeControl::onEKlebePfeil);
	bAnglerhut.addListener(this, &BikeControl::onEAnglerhut);
	bFahrradhelm.addListener(this, &BikeControl::onEFahrradhelm);
	bSkibrille.addListener(this, &BikeControl::onESkibrille);
	bBrille.addListener(this, &BikeControl::onEBrille);
	bWarnweste.addListener(this, &BikeControl::onEWarnweste);
	bLederjacke.addListener(this, &BikeControl::onELederjacke);
	bGreifer.addListener(this, &BikeControl::onEGreifer);
	bSpazierstock.addListener(this, &BikeControl::onESpazierstock);
	bRadlerhose.addListener(this, &BikeControl::onERadlerhose);
	bJogginghose.addListener(this, &BikeControl::onEJogginghose);
	bTurnschuhe.addListener(this, &BikeControl::onETurnschuhe);
	bSandalen.addListener(this, &BikeControl::onESandalen);
	bSuperman.addListener(this, &BikeControl::onESuperman);
	
	gui.add(bPowerBar.setup("Power Bar"));
	gui.add(bBanane.setup("Banane"));
	gui.add(bKlebepfeil.setup("Klebepfeil"));
	gui.add(bAnglerhut.setup("Anglerhut"));
	gui.add(bFahrradhelm.setup("Fahrradhelm"));
	gui.add(bSkibrille.setup("Skibrille"));
	gui.add(bBrille.setup("Brille"));
	gui.add(bWarnweste.setup("Warnweste"));
	gui.add(bLederjacke.setup("Lederjacke"));
	gui.add(bGreifer.setup("Greifer"));
	gui.add(bSpazierstock.setup("Spazierstock"));
	gui.add(bRadlerhose.setup("Radlerhose"));
	gui.add(bJogginghose.setup("Jogginghose"));
	gui.add(bTurnschuhe.setup("Turnschuhe"));
	gui.add(bSandalen.setup("Sandalen"));
	gui.add(bSuperman.setup("Superman"));
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
		if (effects[i].startTime + effects[i].runTime < t && effects[i].runTime != 0) {
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
	gui.draw();
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
	bool contains = false;
	for (auto& e : effects) {
		if (e.name == name) contains = true;
	}
	if(!contains) effects.push_back(Effectory::getEffect(name, effectConfig));
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
void BikeControl::onEBanane()
{
	addEffect("Banane");
}

void BikeControl::onEKlebePfeil()
{
	addEffect("Klebepfeil");
}

void BikeControl::onEAnglerhut()
{
	addEffect("Anglerhut");
}

void BikeControl::onEFahrradhelm()
{
	addEffect("Fahrradhelm");
}

void BikeControl::onESkibrille()
{
	addEffect("Skibrille");
}

void BikeControl::onEBrille()
{
	addEffect("Brille");
}

void BikeControl::onEWarnweste()
{
	addEffect("Warnweste");
}

void BikeControl::onELederjacke()
{
	addEffect("Lederjacke");
}

void BikeControl::onEGreifer()
{
	addEffect("Greifer");
}

void BikeControl::onESpazierstock()
{
	addEffect("Spazierstock");
}

void BikeControl::onERadlerhose()
{
	addEffect("Radlerhose");
}

void BikeControl::onEJogginghose()
{
	addEffect("Jogginghose");
}

void BikeControl::onETurnschuhe()
{
	addEffect("Turnschuhe");
}

void BikeControl::onESandalen()
{
	addEffect("Sandalen");
}

void BikeControl::onESuperman()
{
	addEffect("Superman");
}
