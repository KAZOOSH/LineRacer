#include "BikeSerial.h"

BikeSerial::BikeSerial()
{

}

BikeSerial::~BikeSerial()
{
	stop();
	waitForThread(false);
}

void BikeSerial::setup(ofx::IO::SerialDeviceInfo descriptor)
{
	bool success = bike.setup(descriptor, 9600);

	if (success)
	{

		//bike.registerAllEvents(this);
		ofLogNotice("ofApp::setup") << "Successfully setup " << descriptor;
	}
	else
	{
		ofLogNotice("ofApp::setup") << "Unable to setup " << descriptor;
	}

	start();

}

void BikeSerial::start()
{
	startThread();
}

void BikeSerial::stop()
{
	std::unique_lock<std::mutex> lck(mutex);
			stopThread();
			condition.notify_all();
}

void BikeSerial::threadedFunction()
{
	while(isThreadRunning()){
			  //std::unique_lock<std::mutex> lock(mutex);

			   try
				  {
					  while (bike.available() > 0)
					  {
						  auto word = bike.readStringUntil('\n',100);
						  ofNotifyEvent(speedEvent,word);
					  }

				  }
				  catch (const std::exception& exc)
				  {
					  ofLogError("ofApp::update") << exc.what();
				  }

			   //condition.wait(lock);
			   ofSleepMillis(50);
	}
}

size_t BikeSerial::writeBytes(const ofx::IO::AbstractByteSource &buffer)
{
	std::unique_lock<std::mutex> lock(mutex);
	auto ret = bike.writeBytes(buffer);
	condition.notify_all();
	return ret;
}

size_t BikeSerial::writeByte(uint8_t data)
{
	std::unique_lock<std::mutex> lock(mutex);
	auto ret = bike.writeByte(data);
	condition.notify_all();
	return ret;
}
