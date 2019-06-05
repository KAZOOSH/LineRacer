#pragma once
#include "ofMain.h"
#include "ps3eye.h"
#include "Ps3EyeDeviceManager.h"

using namespace ps3eye;



class ofxPs3Eye {
public:
	ofxPs3Eye();
	~ofxPs3Eye();

	static const std::vector<PS3EYECam::PS3EYERef>& listDevices();
	bool				isFrameNew() const;
	void				close();
	//bool				setup(int w, int h) { return setup(w, h, bUseTexture); }
	//bool				setup(int w, int h, bool bTexture);
	void				setup(int id, int w = 640, int h = 480, int framerate = 60);
	void				update();
	ofTexture			getTexture();
	static void				start();




	//void					setGrabber(std::shared_ptr<ofBaseVideoGrabber> newGrabber);
	//std::shared_ptr<ofBaseVideoGrabber> getGrabber();
	//const std::shared_ptr<ofBaseVideoGrabber> getGrabber() const;

	//template<typename GrabberType>
	//std::shared_ptr<GrabberType> getGrabber() {
	//	return std::dynamic_pointer_cast<GrabberType>(getGrabber());
	//}

	//template<typename GrabberType>
	//const std::shared_ptr<GrabberType> getGrabber() const {
	//	return std::dynamic_pointer_cast<GrabberType>(getGrabber());
	//}

private:
	//ps3eye::PS3EYECam::PS3EYERef eye;

	//ofTexture tex;
	//bool bUseTexture = true;
	//ofPixels pixels;
	//unsigned char * videoFrame;

	//std::shared_ptr<ofBaseVideoGrabber> grabber;
	//int requestedDeviceID = 0;

	//mutable ofPixelFormat internalPixelFormat;
	//int desiredFramerate = 60;

	//bool isRegistered = false;

	ps3eye::PS3EYECam::PS3EYERef eye;

	

	unsigned char * videoFrame;
	ofTexture videoTexture;

};