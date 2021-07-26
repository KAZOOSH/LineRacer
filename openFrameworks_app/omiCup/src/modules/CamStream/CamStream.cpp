#include "CamStream.h"
#include "GPF.h"

namespace ofxModule {
    
	CamStream::CamStream(string moduleName):ModuleDrawable("CamStream",moduleName){
		
		ofVideoGrabber grabber;
		vector<ofVideoDevice> devices = grabber.listDevices();

		   for(size_t i = 0; i < devices.size(); i++){
			   if(devices[i].bAvailable){
				   //log the device
				   ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
			   }else{
				   //log the device and note it as unavailable
				   ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
			   }
		   }



		for(auto& cam:settings["cams"]){
			for(auto dev = devices.begin(); dev != devices.end();){
				if (dev->deviceName == cam["device"].get<string>()){
					Cam newCam;

					newCam.grabber.setDeviceID(dev->id);
					if(cam["fps"].is_null()){
						newCam.grabber.setDesiredFrameRate(30);
					}else{
						newCam.grabber.setDesiredFrameRate(cam["fps"].get<int>());
					}

					if(cam["res"].is_null()){
						newCam.grabber.initGrabber(640,480);
					}else{
						newCam.grabber.initGrabber(cam["res"][0].get<int>(),cam["res"][1].get<int>());
					}

					newCam.id = cam["id"].get<string>();
					newCam.tex = make_shared<ofTexture>(newCam.grabber.getTexture());
					cams.insert(make_pair(cam["id"].get<string>(),newCam));

					ofLogNotice("CamStream") << "device connected " << cam["id"].get<string>();

					devices.erase(dev);
					break;
				}else{
					dev++;
				}
			}

		}

		ofLogNotice()<< "connected Cameras: ";
		for (auto& cam:cams){
			ofLogNotice() << cam.first << endl;
		}
    }
  
    
    
    //------------------------------------------------------------------
    void CamStream::update() {
		for(auto& cam:cams){
			if (cam.second.isActive){
				cam.second.grabber.update();
				if(cam.second.grabber.isFrameNew()){
					notifyEvent(cam.second.tex,"camUpdate",ofJson{{"id",cam.second.id}});
				}
			}
		}
    }

	void CamStream::draw()
	{

	}


    
    //------------------------------------------------------------------
    void CamStream::proceedModuleEvent(ModuleEvent& e) {
		
		if (e.address== "activateCam") {
			if(cams.find(e.message["id"].get<string>())!= cams.end()){
				cams[e.message["id"].get<string>()].isActive = true;
			}
		}else if (e.address== "deactivateCam") {
			if(cams.find(e.message["id"].get<string>())!= cams.end()){
				cams[e.message["id"].get<string>()].isActive = false;
			}
		}
    }
    
}
