#include "Ps3EyeDeviceManager.h"

Ps3EyeDeviceManager *Ps3EyeDeviceManager::s_instance = 0;

void Ps3EyeDeviceManager::start()
{
	threadUpdate.start();
}

void Ps3EyeDeviceManager::registerDevice()
{
	threadUpdate.stop();
	deviceCount++;
	ofSleepMillis(100);
	threadUpdate.start();
}

void Ps3EyeDeviceManager::unregisterDevice()
{
	threadUpdate.stop();
	deviceCount--;
	if(deviceCount > 0)	threadUpdate.start();
}
