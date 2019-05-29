#pragma once
//#include "ofThread.h"
#include "ofMain.h"
#include "ps3eye.h"

class ps3eyeUpdate : public ofThread {

public:
	ps3eyeUpdate() {
	}

	void start() {
		startThread();   // blocking, verbose
		cout << "start thread" << endl;
	}

	void stop() {
		stopThread();
		cout << "stop thread" << endl;
	}

	//--------------------------
	void threadedFunction()
	{
		while (isThreadRunning() != 0)
		{
			//cout << "e";
			bool res = ps3eye::PS3EYECam::updateDevices();
			if (!res)
			{
				break;
			}
		}
		cout << "exit thread" << endl;
	}
};


class Ps3EyeDeviceManager
{
	static Ps3EyeDeviceManager *s_instance;
	Ps3EyeDeviceManager()
	{
		
	}
public:

	void start();
	void registerDevice();
	void unregisterDevice();

	static Ps3EyeDeviceManager *instance()
	{
		if (!s_instance)
			s_instance = new Ps3EyeDeviceManager;
		return s_instance;
	}

private:
	ps3eyeUpdate threadUpdate;
	int deviceCount = 0;
};
