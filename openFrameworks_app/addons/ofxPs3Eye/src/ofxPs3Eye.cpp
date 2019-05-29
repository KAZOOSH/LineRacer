#include "ofxPs3Eye.h"

using namespace ps3eye;
static const int ITUR_BT_601_CY = 1220542;
static const int ITUR_BT_601_CUB = 2116026;
static const int ITUR_BT_601_CUG = -409993;
static const int ITUR_BT_601_CVG = -852492;
static const int ITUR_BT_601_CVR = 1673527;
static const int ITUR_BT_601_SHIFT = 20;

static void yuv422_to_rgba(const uint8_t *yuv_src, const int stride, uint8_t *dst, const int width, const int height)
{
	const int bIdx = 2;
	const int uIdx = 0;
	const int yIdx = 0;

	const int uidx = 1 - yIdx + uIdx * 2;
	const int vidx = (2 + uidx) % 4;
	int j, i;

#define _max(a, b) (((a) > (b)) ? (a) : (b))
#define _saturate(v) static_cast<uint8_t>(static_cast<uint32_t>(v) <= 0xff ? v : v > 0 ? 0xff : 0)

	for (j = 0; j < height; j++, yuv_src += stride)
	{
		uint8_t* row = dst + (width * 4) * j; // 4 channels

		for (i = 0; i < 2 * width; i += 4, row += 8)
		{
			int u = static_cast<int>(yuv_src[i + uidx]) - 128;
			int v = static_cast<int>(yuv_src[i + vidx]) - 128;

			int ruv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVR * v;
			int guv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVG * v + ITUR_BT_601_CUG * u;
			int buv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CUB * u;

			int y00 = _max(0, static_cast<int>(yuv_src[i + yIdx]) - 16) * ITUR_BT_601_CY;
			row[2 - bIdx] = _saturate((y00 + ruv) >> ITUR_BT_601_SHIFT);
			row[1] = _saturate((y00 + guv) >> ITUR_BT_601_SHIFT);
			row[bIdx] = _saturate((y00 + buv) >> ITUR_BT_601_SHIFT);
			row[3] = (0xff);

			int y01 = _max(0, static_cast<int>(yuv_src[i + yIdx + 2]) - 16) * ITUR_BT_601_CY;
			row[6 - bIdx] = _saturate((y01 + ruv) >> ITUR_BT_601_SHIFT);
			row[5] = _saturate((y01 + guv) >> ITUR_BT_601_SHIFT);
			row[4 + bIdx] = _saturate((y01 + buv) >> ITUR_BT_601_SHIFT);
			row[7] = (0xff);
		}
	}
}

ofxPs3Eye::ofxPs3Eye()
{
}

ofxPs3Eye::~ofxPs3Eye()
{
	//if (eye!= nullptr && eye->isStreaming()) {
	//	close();
	//}
}

const std::vector<PS3EYECam::PS3EYERef>& ofxPs3Eye::listDevices()
{
	return PS3EYECam::getDevices();
}

//bool ofxPs3Eye::isFrameNew() const
//{
//	if (eye) {
//		return eye->isNewFrame();
//	}
//	return false;
//}
//
//void ofxPs3Eye::close()
//{
//	if(eye) eye->stop();
//}

//bool ofxPs3Eye::setup(int w, int h, bool bTexture)
//{
//	bUseTexture = bTexture;
//	auto devices = listDevices();
//	eye = devices.at(requestedDeviceID);
//
//	if (!eye->init(w, h, desiredFramerate)) {
//		return false;
//	}
//	eye->start();
//
//	pixels.allocate(eye->getWidth(),eye->getHeight(), OF_IMAGE_COLOR_ALPHA);
//	videoFrame = pixels.getData();
//	//videoFrame = new unsigned char[eye->getWidth()*eye->getHeight() * 4];
//	
//	if (bUseTexture) {
//		tex.allocate(eye->getWidth(), eye->getHeight(), GL_RGBA);
//	}
//
//	if (!isRegistered) {
//		//Ps3CamController::instance()->registerDevice();
//		isRegistered = true;
//	}
//	
//
//	return true;
//}

void ofxPs3Eye::setup(int id, int w, int h, int framerate)
{
	// list out the devices
	std::vector<PS3EYECam::PS3EYERef> devices(PS3EYECam::getDevices());

	if(devices.size() > id){
		eye = devices.at(id);
		bool res = eye->init(w,h,framerate);
		eye->start();

		videoFrame = new unsigned char[eye->getWidth()*eye->getHeight() * 4];
		videoTexture.allocate(eye->getWidth(), eye->getHeight(), GL_RGBA);

		//Ps3EyeDeviceManager::instance()->registerDevice();
		eye->setAutogain(true);
	}

	
}

void ofxPs3Eye::update()
{

		bool isNewFrame = eye->isNewFrame();
		if (isNewFrame)
		{
			yuv422_to_rgba(eye->getLastFramePointer(), eye->getRowBytes(), videoFrame, eye->getWidth(), eye->getHeight());
			videoTexture.loadData(videoFrame, eye->getWidth(), eye->getHeight(), GL_RGBA);
		}

}

ofTexture ofxPs3Eye::getTexture()
{
	return videoTexture;
}

void ofxPs3Eye::start()
{
	Ps3EyeDeviceManager::instance()->start();
}

//ofPixelFormat ofxPs3Eye::getPixelFormat() const
//{
//	return ofPixelFormat::OF_PIXELS_RGBA;
//}
//
//ofPixels & ofxPs3Eye::getPixels()
//{
//	return pixels;
//}
//
//const ofPixels & ofxPs3Eye::getPixels() const
//{
//	return pixels;
//}
//
//ofTexture & ofxPs3Eye::getTexture()
//{
//	return tex;
//}
//
//const ofTexture & ofxPs3Eye::getTexture() const
//{
//	return tex;
//}
//
//void ofxPs3Eye::setVerbose(bool bTalkToMe)
//{
//}
//
//void ofxPs3Eye::setDeviceID(int _deviceID)
//{
//	requestedDeviceID = _deviceID;
//}
//
//void ofxPs3Eye::setDesiredFrameRate(int framerate)
//{
//	desiredFramerate = framerate;
//}
//
//void ofxPs3Eye::setUseTexture(bool bUse)
//{
//	bUseTexture = bUse;
//}
//
//bool ofxPs3Eye::isUsingTexture() const
//{
//	return bUseTexture;
//}
//
//void ofxPs3Eye::draw(float x, float y, float w, float h) const
//{
//	tex.draw(x, y, w, h);	
//}
//
//void ofxPs3Eye::draw(float x, float y) const
//{
//	tex.draw(0, 0);
//}
//
//float ofxPs3Eye::getHeight() const
//{
//	return eye->getHeight();
//}
//
//float ofxPs3Eye::getWidth() const
//{
//	return eye->getWidth();
//}
