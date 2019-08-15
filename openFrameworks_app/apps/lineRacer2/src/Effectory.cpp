#include "Effectory.h"



Effectory::Effectory()
{
}


Effectory::~Effectory()
{
}

Effect Effectory::getEffect(string name, ofJson config)
{
	Effect effect;
	for (auto& j : config) {
		if (j["name"] == name) {
			int runtime = j["runTime"] == nullptr ? 0 : j["runTime"];
			float multiplicator = j["speedMultiplicator"] == nullptr ? 0 : j["speedMultiplicator"];
			bool fixed = j["fixedSpeed"] == nullptr ? false : j["fixedSpeed"];
			float speed = j["speed"] == nullptr ? 0 : j["speed"];
			effect.setup(j["name"], runtime, loadImage(j["icon"].get<string>()), multiplicator, fixed, speed);
		}
	}
	return effect;
}

ofImage Effectory::loadImage(string path)
{
	ofImage img;
	img.load(path);
	return img;
}
