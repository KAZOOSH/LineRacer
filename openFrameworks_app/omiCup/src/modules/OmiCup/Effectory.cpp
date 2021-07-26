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
			int runtime = j["runTime"].is_null() ? 0 : j["runTime"].get<int>();
			float multiplicator = j["speedMultiplicator"].is_null() ? 0 : j["speedMultiplicator"].get<float>();
			bool fixed = j["fixedSpeed"].is_null()? false : j["fixedSpeed"].get<bool>();
			float speed = j["speed"].is_null() ? 0 : j["speed"].get<int>();
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
