#ifndef BIKESERIAL_H
#define BIKESERIAL_H

#include "ofMain.h"
#include "ofxSerial.h"

class BikeSerial: public ofThread
{
public:
	BikeSerial();
	~BikeSerial();


	void setup(ofxIO::SerialDeviceInfo descriptor);
	void start();
	void stop();

	void threadedFunction();

	ofEvent<string> speedEvent;

	std::size_t writeBytes(const ofxIO::AbstractByteSource& buffer);
	std::size_t writeByte(uint8_t data);

private:
	ofxIO::SerialDevice bike;
	std::condition_variable condition;
};

#endif // BIKESERIAL_H
